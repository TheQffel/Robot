#include <Arduino.h>
#include <Adafruit_MCP3008.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_NeoPixel.h>
#include <SimpleDHT.h>

#define PIN_SERIAL_TX 0 // Bluetooth TX Pin
#define PIN_SERIAL_RX 1 // Bluetooth RX Pin

#define PIN_LED_IO 2 // Neopixel Leds
#define PIN_DHT_IO 4 // Temperature Sensor

#define PIN_SPI_SSCS 10 // Expander CS Pin
#define PIN_SPI_MOSI 11 // Expander MOSI Pin
#define PIN_SPI_MISO 12 // Expander MISO Pin
#define PIN_SPI_SCLK 13 // Expander CLK Pin 

#define PIN_I2C_SDA A4 // Expander SDA Pin
#define PIN_I2C_SCL A5 // Expander SCL Pin

#define PIN_DIST_FT A0 // Distance Sensor Front Transmitter
#define PIN_DIST_FR A1 // Distance Sensor Front Reciever
#define PIN_DIST_BT A2 // Distance Sensor Back Transmitter
#define PIN_DIST_BR A3 // Distance Sensor Back Reciever

#define PIN_FIRE_FA A6 // Fire Sensor Front Analog
#define PIN_FIRE_BA A7 // Fire Sensor Back Analog 
#define PIN_FIRE_FD 7 // Fire Sensor Front Digital
#define PIN_FIRE_BD 8 // Fire Sensor Back Digital

#define PIN_MOT_FRS 3 // Motor Front Right Speed
#define PIN_MOT_FLS 5 // Motor Front Left Speed
#define PIN_MOT_RRS 6 // Motor Rear Right Speed
#define PIN_MOT_RLS 9 // Motor Rear Left Speed

#define PIN_SENS_MQ2 0 // MQ2 Sensor Pin
#define PIN_SENS_MQ3 1 // MQ3 Sensor Pin
#define PIN_SENS_MQ4 2 // MQ4 Sensor Pin
#define PIN_SENS_MQ5 3 // MQ5 Sensor Pin
#define PIN_SENS_MQ6 4 // MQ6 Sensor Pin
#define PIN_SENS_MQ7 5 // MQ7 Sensor Pin
#define PIN_SENS_MQ8 6 // MQ8 Sensor Pin
#define PIN_SENS_MQ9 7 // MQ9 Sensor Pin

#define PIN_MOT_FREA 8 // Motor Front Right Encoder A
#define PIN_MOT_FREB 9 // Motor Front Right Encoder B
#define PIN_MOT_FLEA 10 // Motor Front Left Encoder A
#define PIN_MOT_FLEB 11 // Motor Front Left Encoder B
#define PIN_MOT_RREA 12 // Motor Rear Right Encoder A
#define PIN_MOT_RREB 13 // Motor Rear Right Encoder B
#define PIN_MOT_RLEA 14 // Motor Rear Left Encoder A
#define PIN_MOT_RLEB 15 // Motor Rear Left Encoder B

#define PIN_MOT_FRDA 0 // Motor Front Right Direction A
#define PIN_MOT_FRDB 1 // Motor Front Right Direction B
#define PIN_MOT_FLDA 2 // Motor Front Left Direction A
#define PIN_MOT_FLDB 3 // Motor Front Left Direction B
#define PIN_MOT_RRDA 4 // Motor Rear Right Direction A
#define PIN_MOT_RRDB 5 // Motor Rear Right Direction B
#define PIN_MOT_RLDA 6 // Motor Rear Left Direction A
#define PIN_MOT_RLDB 7 // Motor Rear Left Direction B

Adafruit_MCP3008 ExpanderAnalog;
Adafruit_MCP23X17 ExpanderDigital;

Adafruit_NeoPixel Leds(12, PIN_LED_IO, NEO_GRB + NEO_KHZ800);

SimpleDHT11 TempHumSensor(PIN_DHT_IO);

long LedsLastUpdate = 0;
long SensorLastUpdate_RefreshRateHigh = 0;
long SensorLastUpdate_RefreshRateMedium = 0;
long SensorLastUpdate_RefreshRateLow = 0;

int MotorTimeout = 0;
bool CanMove_Forward = true;
bool CanMove_Backward = true;

uint8_t SensorValue_Humudity = 0;
uint8_t SensorValue_Temperature = 0;
uint8_t SensorValue_DistanceFront = 0;
uint8_t SensorValue_DistanceBack = 0;
uint8_t SensorValue_FireFront = 0;
uint8_t SensorValue_FireBack = 0;

int8_t SensorValue_MQ2 = -1;
int8_t SensorValue_MQ3 = -1;
int8_t SensorValue_MQ4 = -1;
int8_t SensorValue_MQ5 = -1;
int8_t SensorValue_MQ6 = -1;
int8_t SensorValue_MQ7 = -1;
int8_t SensorValue_MQ8 = -1;
int8_t SensorValue_MQ9 = -1;

int8_t convertAnalogToPercent(int value, bool invert = false)
{
  int8_t result = ((value + 5) / 10);
  if(result < 0) { result = 0; }
  if(result > 100) { result = 100; }
  if(invert) { result = 100 - result; }
  return result;
}

uint32_t convertPercentToColor(int value, bool invert = false)
{
  if(value < 0 || value > 100)
  { return Leds.Color(0, 0, 10); }

  if(invert) { value = 100 - value; }
  uint8_t colorR = 5; uint8_t colorG = 5;

  if(value >= 55) { colorG--; colorR++; }
  if(value >= 64) { colorG--; colorR++; }
  if(value >= 73) { colorG--; colorR++; }
  if(value >= 82) { colorG--; colorR++; }
  if(value >= 91) { colorG--; colorR++; }
  
  if(value <= 45) { colorG++; colorR--; }
  if(value <= 36) { colorG++; colorR--; }
  if(value <= 27) { colorG++; colorR--; }
  if(value <= 18) { colorG++; colorR--; }
  if(value <= 9) { colorG++; colorR--; }

  return Leds.Color(colorR, colorG, 0);
}

void setMovingDirection(char direction)
{
  if(direction == 'F' && CanMove_Forward)
  {
    ExpanderDigital.digitalWrite(PIN_MOT_FRDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_FRDB, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDB, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDB, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDB, HIGH);
  }
  if(direction == 'B' && CanMove_Backward)
  {
    ExpanderDigital.digitalWrite(PIN_MOT_FRDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_FRDB, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDB, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDB, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDB, LOW);
  }
  if(direction == 'L')
  {
    ExpanderDigital.digitalWrite(PIN_MOT_FRDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_FRDB, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDB, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDB, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDA, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDB, LOW);
  }
  if(direction == 'R')
  {
    ExpanderDigital.digitalWrite(PIN_MOT_FRDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_FRDB, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_FLDB, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RRDB, HIGH);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDA, LOW);
    ExpanderDigital.digitalWrite(PIN_MOT_RLDB, HIGH);
  }
}

void setMovingSpeed(int value)
{
  analogWrite(PIN_MOT_FRS, value * 50);
  analogWrite(PIN_MOT_FLS, value * 50);
  analogWrite(PIN_MOT_RRS, value * 50);
  analogWrite(PIN_MOT_RLS, value * 50);
}

void controlMotors()
{
  bool tempFront = SensorValue_DistanceFront < 25;
  if(tempFront && CanMove_Forward)
  { setMovingSpeed(0); } // stop if moving
  CanMove_Forward = !tempFront;

  bool tempBack = SensorValue_DistanceBack < 25;
  if(tempBack && CanMove_Backward)
  { setMovingSpeed(0); } // stop if moving
  CanMove_Backward = !tempBack;

  if(Serial.available())
  {
    String command = Serial.readStringUntil('\n');

    if(command.charAt(0) == 'A')
    {
      setMovingDirection(command.charAt(2));
      setMovingSpeed(command.charAt(4) - '0');
      MotorTimeout = 25;
    }

    // android <-> arduino <-> raspberry
    Serial.println(command);
  }

  // prevent moving if lost connection
  if(MotorTimeout > 0)
  {
    MotorTimeout--;
    if(MotorTimeout == 0)
    { setMovingSpeed(0); }
  }
}

void refreshSensorValues()
{
  long time = millis();

  if(SensorLastUpdate_RefreshRateHigh + 100 < time) // once per 0.1 second
  {
    SensorLastUpdate_RefreshRateHigh = time;

    digitalWrite(PIN_DIST_FT, HIGH);
    delayMicroseconds(5);
    digitalWrite(PIN_DIST_FT, LOW);

    SensorValue_DistanceFront = pulseIn(PIN_DIST_FR, HIGH, 10000) / 50;
    if(SensorValue_DistanceFront > 100) { SensorValue_DistanceFront = 100; }

    digitalWrite(PIN_DIST_BT, HIGH);
    delayMicroseconds(5);
    digitalWrite(PIN_DIST_BT, LOW);

    SensorValue_DistanceBack = pulseIn(PIN_DIST_BR, HIGH, 10000) / 50;
    if(SensorValue_DistanceBack > 100) { SensorValue_DistanceBack = 100; }

    Serial.print('D');
    Serial.print(' '); Serial.print(SensorValue_DistanceFront);
    Serial.print(' '); Serial.print(SensorValue_DistanceBack);
    Serial.println();

    SensorValue_FireFront = convertAnalogToPercent(analogRead(PIN_FIRE_FA), true);
    SensorValue_FireBack = convertAnalogToPercent(analogRead(PIN_FIRE_BA), true);

    Serial.print('F');
    Serial.print(' '); Serial.print(SensorValue_FireFront);
    Serial.print(' '); Serial.print(SensorValue_FireBack);
    Serial.println();
  }

  if(SensorLastUpdate_RefreshRateMedium + 1000 < time) // once per 1 second
  {
    SensorLastUpdate_RefreshRateMedium = time;

    if(time > 100000) // sensors heats up after few minutes
    {
      SensorValue_MQ2 = convertAnalogToPercent(ExpanderAnalog.readADC(0));
      SensorValue_MQ3 = convertAnalogToPercent(ExpanderAnalog.readADC(1));
      SensorValue_MQ4 = convertAnalogToPercent(ExpanderAnalog.readADC(2));
      SensorValue_MQ5 = convertAnalogToPercent(ExpanderAnalog.readADC(3));
      SensorValue_MQ6 = convertAnalogToPercent(ExpanderAnalog.readADC(4));
      SensorValue_MQ7 = convertAnalogToPercent(ExpanderAnalog.readADC(5));
      SensorValue_MQ8 = convertAnalogToPercent(ExpanderAnalog.readADC(6));
      SensorValue_MQ9 = convertAnalogToPercent(ExpanderAnalog.readADC(7));
    }

    Serial.print('C');
    Serial.print(' '); Serial.print(SensorValue_MQ2);
    Serial.print(' '); Serial.print(SensorValue_MQ3);
    Serial.print(' '); Serial.print(SensorValue_MQ4);
    Serial.print(' '); Serial.print(SensorValue_MQ5);
    Serial.print(' '); Serial.print(SensorValue_MQ6);
    Serial.print(' '); Serial.print(SensorValue_MQ7);
    Serial.print(' '); Serial.print(SensorValue_MQ8);
    Serial.print(' '); Serial.print(SensorValue_MQ9);
    Serial.println();
  }

  if(SensorLastUpdate_RefreshRateLow + 10000 < time) // once per 10 seconds
  {
    SensorLastUpdate_RefreshRateLow = time;

    TempHumSensor.read(&SensorValue_Temperature, &SensorValue_Humudity, NULL);

    if(SensorValue_Temperature == 0 && SensorValue_Humudity == 0) // not ready
    {
      SensorValue_Temperature = 25;
      SensorValue_Humudity = 50;
    }

    Serial.print('E');
    Serial.print(' '); Serial.print(SensorValue_Temperature);
    Serial.print(' '); Serial.print(SensorValue_Humudity);
    Serial.println();
  }
}

void displaySensorValues()
{
  long time = millis();

  if(LedsLastUpdate + 1000 < time) // once per 1 second
  {
    LedsLastUpdate = time;

    Leds.setPixelColor(0, convertPercentToColor(SensorValue_MQ2));
    Leds.setPixelColor(1, convertPercentToColor(SensorValue_MQ3));
    Leds.setPixelColor(2, convertPercentToColor(SensorValue_MQ4));
    Leds.setPixelColor(3, convertPercentToColor(SensorValue_MQ5));
    Leds.setPixelColor(4, convertPercentToColor(SensorValue_MQ6));
    Leds.setPixelColor(5, convertPercentToColor(SensorValue_MQ7));
    Leds.setPixelColor(6, convertPercentToColor(SensorValue_MQ8));
    Leds.setPixelColor(7, convertPercentToColor(SensorValue_MQ9));

    Leds.setPixelColor(8, convertPercentToColor(SensorValue_DistanceFront, true));
    Leds.setPixelColor(9, convertPercentToColor(SensorValue_DistanceBack, true));
    Leds.setPixelColor(10, convertPercentToColor(SensorValue_FireFront));
    Leds.setPixelColor(11, convertPercentToColor(SensorValue_FireBack));

    Leds.show();
  }
}

void setup()
{
  delay(1000);

  pinMode(PIN_FIRE_FA, INPUT);
  pinMode(PIN_FIRE_FD, INPUT);
  pinMode(PIN_FIRE_BA, INPUT);
  pinMode(PIN_FIRE_BD, INPUT);

  pinMode(PIN_DIST_FT, OUTPUT);
  pinMode(PIN_DIST_FR, INPUT);
  pinMode(PIN_DIST_BT, OUTPUT);
  pinMode(PIN_DIST_BR, INPUT);

  pinMode(PIN_MOT_FRS, OUTPUT);
  pinMode(PIN_MOT_FLS, OUTPUT);
  pinMode(PIN_MOT_RRS, OUTPUT);
  pinMode(PIN_MOT_RLS, OUTPUT);

  Wire.begin();

  Serial.begin(9600);
  
  Leds.begin();

  ExpanderDigital.begin_I2C(0x20);

  ExpanderAnalog.begin(PIN_SPI_SSCS);

  ExpanderDigital.pinMode(PIN_MOT_FRDA, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_FRDB, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_FLDA, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_FLDB, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_RRDA, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_RRDB, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_RLDA, OUTPUT);
  ExpanderDigital.pinMode(PIN_MOT_RLDB, OUTPUT);

  ExpanderDigital.pinMode(PIN_MOT_FREA, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_FREB, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_FLEA, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_FLEB, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_RREA, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_RREB, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_RLEA, INPUT);
  ExpanderDigital.pinMode(PIN_MOT_RLEB, INPUT);

  delay(1000);
}

void loop()
{
  controlMotors();

  refreshSensorValues();

  displaySensorValues();

  delay(10);
}
import RPi.GPIO as GPIO
import serial
import time

in1 = 22
in2 = 23
in3 = 24
in4 = 27

in6 = 5
in7 = 6
in8 = 12
in9 = 13

step_sequence = [[1,0,0,1], [1,0,0,0], [1,1,0,0], [0,1,0,0], [0,1,1,0], [0,0,1,0], [0,0,1,1], [0,0,0,1]]

arduino_port = "/dev/ttyUSB0"
baud_rate = 9600

time.sleep(10)

GPIO.setmode(GPIO.BCM)
GPIO.setup(in1, GPIO.OUT)
GPIO.setup(in2, GPIO.OUT)
GPIO.setup(in3, GPIO.OUT)
GPIO.setup(in4, GPIO.OUT)
GPIO.setup(in6, GPIO.OUT)
GPIO.setup(in7, GPIO.OUT)
GPIO.setup(in8, GPIO.OUT)
GPIO.setup(in9, GPIO.OUT)

GPIO.output(in1, GPIO.LOW)
GPIO.output(in2, GPIO.LOW)
GPIO.output(in3, GPIO.LOW)
GPIO.output(in4, GPIO.LOW)
GPIO.output(in6, GPIO.LOW)
GPIO.output(in7, GPIO.LOW)
GPIO.output(in8, GPIO.LOW)
GPIO.output(in9, GPIO.LOW)

motor_pins_a = [in1,in2,in3,in4]
motor_pins_b = [in6,in7,in8,in9]
motor_step_counter_a = 0
motor_step_counter_b = 0
move_a = 0
move_b = 0

ser = serial.Serial(arduino_port, baud_rate, timeout=1)

time.sleep(10)

while True:
    if ser.in_waiting > 0:
        command = ser.readline().decode().strip()

        if command[0] == 'B':
            if command[2] == 'L':
                if command[4] == 'C':
                    move_a = 1
                    move_b = 0
                if command[4] == 'O':
                    move_a = -1
                    move_b = 0
            if command[2] == 'R':
                if command[4] == 'C':
                    move_a = 0
                    move_b = -1
                if command[4] == 'O':
                    move_a = 0
                    move_b = 1
            if command[2] == 'B':
                if command[4] == 'C':
                    move_a = 1
                    move_b = -1
                if command[4] == 'O':
                    move_a = -1
                    move_b = 1

            for i in range(8):
                motor_step_counter_a = (motor_step_counter_a + move_a) % 8
                motor_step_counter_b = (motor_step_counter_b + move_b) % 8
                time.sleep(0.005)
                for pin in range(4):
                    GPIO.output(motor_pins_a[pin], step_sequence[motor_step_counter_a][pin])
                    GPIO.output(motor_pins_b[pin], step_sequence[motor_step_counter_b][pin])

            move_a = 0
            move_b = 0

    time.sleep(0.01)

GPIO.output(in1, GPIO.LOW)
GPIO.output(in2, GPIO.LOW)
GPIO.output(in3, GPIO.LOW)
GPIO.output(in4, GPIO.LOW)
GPIO.output(in6, GPIO.LOW)
GPIO.output(in7, GPIO.LOW)
GPIO.output(in8, GPIO.LOW)
GPIO.output(in9, GPIO.LOW)
GPIO.cleanup()
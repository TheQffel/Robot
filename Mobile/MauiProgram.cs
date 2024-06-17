using Microsoft.Extensions.Logging;
using LibVLCSharp.MAUI;
using Plugin.BLE.Abstractions.Contracts;
using Plugin.BLE;
using System.Text;

namespace Mobile
{
    public static class MauiProgram
    {
        private static bool loop = false;
        private static IDevice? dev = null;

        public static MauiApp CreateMauiApp()
        {
            var builder = MauiApp.CreateBuilder();
            builder.UseMauiApp<App>()
            .ConfigureMauiHandlers(handlers =>
            {
                handlers.AddHandler(typeof(VideoView), typeof(VideoViewHandler));
            })
            .ConfigureFonts(fonts =>
            {
                fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
                fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
            });
            builder.Logging.AddDebug();
            return builder.Build();
        }

        public async static void SerialLoop_Start(IDevice device)
        {
            try
            {
                if(dev != null)
                {
                    await Task.Delay(1000);
                }

                dev = device;
                loop = true;

                var ble = CrossBluetoothLE.Current;
                var adapter = CrossBluetoothLE.Current.Adapter;

                await adapter.ConnectToDeviceAsync(device);

                var service = await device.GetServiceAsync(Guid.Parse("0000ffe0-0000-1000-8000-00805f9b34fb"));
                var characteristic = await service.GetCharacteristicAsync(Guid.Parse("0000ffe1-0000-1000-8000-00805f9b34fb"));

                bool nowMoving = false;
                bool wasMoving = false;

                int wasTime = -1;
                int nowTime = -1;

                while (loop)
                {
                    nowTime = DateTime.Now.Millisecond / 10;

                    var stream = (await characteristic.ReadAsync()).data;
                    var commands = Encoding.ASCII.GetString(stream).Replace('\r', '\n').Split('\n');

                    foreach (string command in commands)
                    {
                        if(command.Length > 1)
                        {
                            ConsolePage.LogMessage(command);

                            string[] data = command.Split(' ');

                            if (command[0] == 'C' && data.Length > 8)
                            {
                                int.TryParse(data[1], out SensorsPage.SensorValue_MQ2);
                                int.TryParse(data[2], out SensorsPage.SensorValue_MQ3);
                                int.TryParse(data[3], out SensorsPage.SensorValue_MQ4);
                                int.TryParse(data[4], out SensorsPage.SensorValue_MQ5);
                                int.TryParse(data[5], out SensorsPage.SensorValue_MQ6);
                                int.TryParse(data[6], out SensorsPage.SensorValue_MQ7);
                                int.TryParse(data[7], out SensorsPage.SensorValue_MQ8);
                                int.TryParse(data[8], out SensorsPage.SensorValue_MQ9);
                            }

                            if (command[0] == 'D' && data.Length > 2)
                            {
                                int.TryParse(data[1], out SensorsPage.SensorValue_DistanceFront);
                                int.TryParse(data[2], out SensorsPage.SensorValue_DistanceBack);
                            }

                            if (command[0] == 'E' && data.Length > 2)
                            {
                                int.TryParse(data[1], out SensorsPage.SensorValue_Temperature);
                                int.TryParse(data[2], out SensorsPage.SensorValue_Humidity);
                            }

                            if (command[0] == 'F' && data.Length > 2)
                            {
                                int.TryParse(data[1], out SensorsPage.SensorValue_FlamesFront);
                                int.TryParse(data[2], out SensorsPage.SensorValue_FlamesBack);
                            }
                        }
                    }

                    if(wasTime != nowTime)
                    {
                        wasTime = nowTime;

                        nowMoving = false;

                        if (ControlPage.ButtonState_MoveForward)
                        {
                            nowMoving = true;
                            wasMoving = true;

                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("A F " + SettingsPage.Speeds[SettingsPage.Speed] + '\n'));
                        }

                        if (ControlPage.ButtonState_MoveBackward)
                        {
                            nowMoving = true;
                            wasMoving = true;

                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("A B " + SettingsPage.Speeds[SettingsPage.Speed] + '\n'));
                        }

                        if (ControlPage.ButtonState_MoveLeft)
                        {
                            nowMoving = true;
                            wasMoving = true;

                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("A L " + SettingsPage.Speeds[SettingsPage.Speed] + '\n'));
                        }

                        if (ControlPage.ButtonState_MoveRight)
                        {
                            nowMoving = true;
                            wasMoving = true;

                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("A R " + SettingsPage.Speeds[SettingsPage.Speed] + '\n'));
                        }

                        if (!nowMoving && wasMoving)
                        {
                            wasMoving = false;

                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("A 0 0" + '\n'));
                        }

                        if (ControlPage.ButtonState_PutDownLeft)
                        {
                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("B L C" + '\n'));
                        }
                        
                        if (ControlPage.ButtonState_PickUpLeft)
                        {
                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("B L O" + '\n'));
                        }

                        if (ControlPage.ButtonState_PutDownBoth)
                        {
                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("B B C" + '\n'));
                        }

                        if (ControlPage.ButtonState_PickUpBoth)
                        {
                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("B B O" + '\n'));
                        }

                        if (ControlPage.ButtonState_PutDownRight)
                        {
                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("B R C" + '\n'));
                        }

                        if (ControlPage.ButtonState_PickUpRight)
                        {
                            await characteristic.WriteAsync(Encoding.ASCII.GetBytes("B R O" + '\n'));
                        }
                    }
                }
            }
            catch (Exception ex) { }

            loop = false;
        }

        public async static void SerialLoop_Stop()
        {
            try
            {
                loop = false;

                if (dev != null)
                {
                    await Task.Delay(500);

                    var ble = CrossBluetoothLE.Current;
                    var adapter = CrossBluetoothLE.Current.Adapter;

                    await adapter.DisconnectDeviceAsync(dev);
                }

                dev = null;
            }
            catch(Exception ex) { }

            loop = false;
        }
    }
}

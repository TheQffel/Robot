using System.Timers;

namespace Mobile
{
    public partial class SensorsPage : ContentPage
    {
        public static int SensorValue_MQ2 = -1;
        public static int SensorValue_MQ3 = -1;
        public static int SensorValue_MQ4 = -1;
        public static int SensorValue_MQ5 = -1;
        public static int SensorValue_MQ6 = -1;
        public static int SensorValue_MQ7 = -1;
        public static int SensorValue_MQ8 = -1;
        public static int SensorValue_MQ9 = -1;

        public static int SensorValue_DistanceFront = 75;
        public static int SensorValue_DistanceBack = 75;
        public static int SensorValue_FlamesFront = 25;
        public static int SensorValue_FlamesBack = 25;

        public static int SensorValue_Temperature = 0;
        public static int SensorValue_Humidity = 50;

        private System.Timers.Timer timer = new();

        public SensorsPage()
        {
            InitializeComponent();
        }

        protected override void OnAppearing()
        {
            if (SettingsPage.Devices.ContainsKey(SettingsPage.Device))
            {
                Task.Run(() => MauiProgram.SerialLoop_Start(SettingsPage.Devices[SettingsPage.Device]));
            }

            timer = new System.Timers.Timer(100);
            timer.Elapsed += UpdateValues;
            timer.AutoReset = true;
            timer.Enabled = true;

            base.OnAppearing();
        }

        protected override void OnDisappearing()
        {
            Task.Run(() => MauiProgram.SerialLoop_Stop());

            timer.Stop();
            timer.Dispose();

            base.OnDisappearing();
        }

        private void UpdateValues(object? sender, ElapsedEventArgs? e)
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                MQ2Value.Text = SensorValue_MQ2 + " %";
                MQ3Value.Text = SensorValue_MQ3 + " %";
                MQ4Value.Text = SensorValue_MQ4 + " %";
                MQ5Value.Text = SensorValue_MQ5 + " %";
                MQ6Value.Text = SensorValue_MQ6 + " %";
                MQ7Value.Text = SensorValue_MQ7 + " %";
                MQ8Value.Text = SensorValue_MQ8 + " %";
                MQ9Value.Text = SensorValue_MQ9 + " %";

                TemperatureValue.Text = SensorValue_Temperature + " °C";
                HumidityValue.Text = SensorValue_Humidity + " %";

                DistanceFront.Text = SensorValue_DistanceFront > 50 ? "Nie wykryto przeszkody z przodu." : "Wykryto przeszkodę z przodu!";
                DistanceBack.Text = SensorValue_DistanceBack > 50 ? "Nie wykryto przeszkody z tyłu." : "Wykryto przeszkodę z tyłu!";
                FlamesFront.Text = SensorValue_FlamesFront < 50 ? "Nie wykryto płomieni z przodu." : "Wykryto płomienie z przodu!";
                FlamesBack.Text = SensorValue_FlamesBack < 50 ? "Nie wykryto płomieni z tyłu." : "Wykryto płomienie z tyłu!";
            });
        }
    }
}

using LibVLCSharp.Shared;

namespace Mobile
{
    public partial class ControlPage : ContentPage
    {
        public static bool ButtonState_MoveLeft = false;
        public static bool ButtonState_MoveForward = false;
        public static bool ButtonState_MoveRight = false;
        public static bool ButtonState_MoveBackward = false;

        public static bool ButtonState_PickUpLeft = false;
        public static bool ButtonState_PutDownLeft = false;
        public static bool ButtonState_PickUpBoth = false;
        public static bool ButtonState_PutDownBoth = false;
        public static bool ButtonState_PickUpRight = false;
        public static bool ButtonState_PutDownRight = false;

        private bool video = false;

        public ControlPage()
        {
            InitializeComponent();
        }

        protected override void OnAppearing()
        {
            if (SettingsPage.Devices.ContainsKey(SettingsPage.Device))
            {
                Task.Run(() => MauiProgram.SerialLoop_Start(SettingsPage.Devices[SettingsPage.Device]));
            }

            LoadVideo(new object(), new EventArgs());

            base.OnAppearing();
        }

        protected override void OnDisappearing()
        {
            Task.Run(() => MauiProgram.SerialLoop_Stop());

            UnLoadVideo(new object(), new EventArgs());

            base.OnDisappearing();
        }

        private void LoadVideo(object? sender, EventArgs e)
        {
            if (!video)
            {
                try
                {
                    LibVLC lib = new LibVLC(["--video-filter=transform", "--transform-type=270"]);
                    Camera.MediaPlayer = new MediaPlayer(lib);

                    Media media = new Media(lib, new Uri(SettingsPage.Protocols[SettingsPage.Protocol] + "://" + SettingsPage.Ip + ":" + SettingsPage.Port));

                    media.AddOption(":demux=h264");
                    media.AddOption(":network-caching=100");
                    media.AddOption(":clock-jitter=0");
                    media.AddOption(":clock-syncro=0");

                    Camera.MediaPlayer.Play(media);
                }
                catch { }

                video = true;
            }
        }

        private void UnLoadVideo(object? sender, EventArgs e)
        {
            if (Camera.MediaPlayer != null)
            {
                Camera.MediaPlayer.Stop();
            }
            video = false;
        }

        private void ButtonPressed(object sender, EventArgs e)
        {
            if (sender == MoveLeft) { ButtonState_MoveLeft = true; }
            if (sender == MoveForward) { ButtonState_MoveForward = true; }
            if (sender == MoveRight) { ButtonState_MoveRight = true; }
            if (sender == MoveBackward) { ButtonState_MoveBackward = true; }

            if (sender == PickUpLeft) { ButtonState_PickUpLeft = true; }
            if (sender == PutDownLeft) { ButtonState_PutDownLeft = true; }
            if (sender == PickUpBoth) { ButtonState_PickUpBoth = true; }
            if (sender == PutDownBoth) { ButtonState_PutDownBoth = true; }
            if (sender == PickUpRight) { ButtonState_PickUpRight = true; }
            if (sender == PutDownRight) { ButtonState_PutDownRight = true; }
        }

        private void ButtonReleased(object sender, EventArgs e)
        {
            if (sender == MoveLeft) { ButtonState_MoveLeft = false; }
            if (sender == MoveForward) { ButtonState_MoveForward = false; }
            if (sender == MoveRight) { ButtonState_MoveRight = false; }
            if (sender == MoveBackward) { ButtonState_MoveBackward = false; }

            if (sender == PickUpLeft) { ButtonState_PickUpLeft = false; }
            if (sender == PutDownLeft) { ButtonState_PutDownLeft = false; }
            if (sender == PickUpBoth) { ButtonState_PickUpBoth = false; }
            if (sender == PutDownBoth) { ButtonState_PutDownBoth = false; }
            if (sender == PickUpRight) { ButtonState_PickUpRight = false; }
            if (sender == PutDownRight) { ButtonState_PutDownRight = false; }
        }
    }
}
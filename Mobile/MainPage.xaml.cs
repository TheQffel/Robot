using LibVLCSharp.Shared;

namespace Mobile
{
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();

            DeviceDisplay.Current.KeepScreenOn = true;

            Core.Initialize();

            Permissions.RequestAsync<Permissions.LocationWhenInUse>();
        }
    }
}

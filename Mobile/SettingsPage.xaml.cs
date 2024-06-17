using Plugin.BLE;
using Plugin.BLE.Abstractions.Contracts;

namespace Mobile
{
    public partial class SettingsPage : ContentPage
    {
        public static string Device = "BT05";
        public static string Ip = "192.168.1.10";
        public static int Port = 5000;
        public static int Protocol = 1;
        public static int Speed = 1;

        public static Dictionary<string, IDevice> Devices = [];
        public static string[] Protocols = ["udp", "tcp", "rtsp"];
        public static string[] Speeds = ["1", "2", "3"];

        private int index = -1;

        public SettingsPage()
        {
            InitializeComponent();

            WifiValue.Items.Add("Robot");
            WifiValue.SelectedIndex = 0;
            WifiValue.SelectedItem = "Robot";
        }

        protected override void OnAppearing()
        {
            try
            {
                try
                {
                    index = 0;
                    Devices.Clear();
                    BlueValue.Items.Clear();
                }
                catch { }

                var ble = CrossBluetoothLE.Current;
                var adapter = CrossBluetoothLE.Current.Adapter;

                adapter.ScanMode = ScanMode.LowLatency;
                adapter.DeviceDiscovered += (s, a) => DeviceFound(a.Device);
                adapter.StartScanningForDevicesAsync();
            }
            catch (Exception e) { }

            base.OnAppearing();
        }

        protected override void OnDisappearing()
        {
            try
            {
                var ble = CrossBluetoothLE.Current;
                var adapter = CrossBluetoothLE.Current.Adapter;

                adapter.DeviceDiscovered -= (s, a) => DeviceFound(a.Device);
                adapter.StopScanningForDevicesAsync();
            }
            catch (Exception e) { }

            base.OnDisappearing();
        }

        private void DeviceFound(IDevice device)
        {
            if (device != null)
            {
                var name = device.Name;
                if(name == null)
                {
                    name = "Unknown Device " + index++;
                }

                Devices[name] = device;
                if(!BlueValue.Items.Contains(name))
                {
                    BlueValue.Items.Add(name);
                }
            }
        }

        private void OptionChanged(object sender, EventArgs e)
        {
            Ip = AddrValue.Text;
            Device = (string)BlueValue.SelectedItem;
            int.TryParse(PortValue.Text, out Port);

            Protocol = ProtoValue.SelectedIndex;
            Speed = SpeedValue.SelectedIndex;
            if (Speed == -1) { Speed = 1; }
        }
    }
}

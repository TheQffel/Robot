namespace Mobile
{
    public partial class ConsolePage : ContentPage
    {
        private static string[] messages =
        {
            "", "", "", "", "", "", "", "", "", "",
            "", "", "", "", "", "", "", "", "", "",
        };

        public ConsolePage()
        {
            InitializeComponent();
        }

        protected override void OnAppearing()
        {
            base.OnAppearing();

            MessageA.Text = messages[0];
            MessageB.Text = messages[1];
            MessageC.Text = messages[2];
            MessageD.Text = messages[3];
            MessageE.Text = messages[4];
            MessageF.Text = messages[5];
            MessageG.Text = messages[6];
            MessageH.Text = messages[7];
            MessageI.Text = messages[8];
            MessageJ.Text = messages[9];
            MessageK.Text = messages[10];
            MessageL.Text = messages[11];
            MessageM.Text = messages[12];
            MessageN.Text = messages[13];
            MessageO.Text = messages[14];
            MessageP.Text = messages[15];
            MessageR.Text = messages[16];
            MessageS.Text = messages[17];
            MessageT.Text = messages[18];
            MessageU.Text = messages[19];
        }

        public static void LogMessage(string msg)
        {
            var date = DateTime.Now.ToString("HH:mm:ss");
            date = "[" + date + "]";
            msg = date + ": " + msg;

            for (int i = 1; i < messages.Length; i++)
            {
                messages[i - 1] = messages[i];
            }

            messages[messages.Length - 1] = msg;
        }
    }
}
using FirstFloor.ModernUI.Windows;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using FirstFloor.ModernUI.Windows.Navigation;
using FirstFloor.ModernUI.Windows.Controls;

namespace Trivia_Client.Pages.Options
{
    /// <summary>
    /// Interaction logic for BestScores.xaml
    /// </summary>
    public partial class BestScores : UserControl, IContent
    {
        private ModernFrame frame;

        public BestScores()
        {
            InitializeComponent();
            InitializeScores();
        }

        public void InitializeScores()
        {
            ClientReceivedMessage ClientMessage = new ClientReceivedMessage(ClientCodes.BestScores);
            ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.BestScores, Session.CurrentUser.SendBackToServer(ClientMessage));

            switch (ServerMessage._Values.Length)
            {
                case 2:
                    FirstScore.Text = ServerMessage._Values[0] + " - " + Convert.ToInt16(ServerMessage._Values[1]).ToString();
                    SecondScore.Text = null;
                    ThirdScore.Text = null;
                    break;

                case 4:
                    FirstScore.Text = ServerMessage._Values[0] + " - " + Convert.ToInt16(ServerMessage._Values[1]).ToString();
                    SecondScore.Text = ServerMessage._Values[2] + " - " + Convert.ToInt16(ServerMessage._Values[3]).ToString();
                    ThirdScore.Text = null;
                    break;

                case 6:
                    FirstScore.Text = ServerMessage._Values[0] + " - " +  Convert.ToInt16(ServerMessage._Values[1]).ToString();
                    SecondScore.Text = ServerMessage._Values[2] + " - " + Convert.ToInt16(ServerMessage._Values[3]).ToString();
                    ThirdScore.Text = ServerMessage._Values[4] + " - " + Convert.ToInt16(ServerMessage._Values[5]).ToString();
                    break;

                default:
                    break;
            }
        }

        public void OnFragmentNavigation(FirstFloor.ModernUI.Windows.Navigation.FragmentNavigationEventArgs e)
        {
        }

        public void OnNavigatedFrom(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            FirstScore.Text = null;
            SecondScore.Text = null;
            ThirdScore.Text = null;
        }

        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            if (frame == null)
                frame = e.Frame;
            else
                InitializeScores();
        }

        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
        }
    }
}

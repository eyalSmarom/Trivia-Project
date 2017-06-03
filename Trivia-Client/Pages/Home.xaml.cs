using FirstFloor.ModernUI.Windows.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
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

namespace Trivia_Client.Pages
{
    /// <summary>
    /// Interaction logic for Home.xaml
    /// </summary>
    public partial class Home : UserControl
    {
        public Home()
        {
            InitializeComponent();
            if (Connection())
            {
                if (!IsLoggedIn())
                    SetConnectionOptions();
                else
                    SetMenuPage();
            }
        }

        private bool Connection()
        {
            Socket ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                ClientSocket.Connect(IPAddress.Loopback, 8820);
                Session.CurrentUser = new User(ClientSocket);
                return true;
            }
            catch (Exception e) { Console.WriteLine(e.ToString()); return false; }
        }

        private bool IsLoggedIn()
        {
            if (Session.Logged)
                return true;
            else
                return false;
        }

        private void SetConnectionOptions()
        {
            var Window = Application.Current.MainWindow as ModernWindow;

            for(int i = 0; i < Window.MenuLinkGroups.Count; i++)
            {
                Window.MenuLinkGroups.RemoveAt(0);
            }

            Window.MenuLinkGroups.Add(new FirstFloor.ModernUI.Presentation.LinkGroup()
            {
                DisplayName = "Connect"
            });

            MainFrame.Navigate(new Connect());
        }

        private void SetMenuPage()
        {

        }
    }
}

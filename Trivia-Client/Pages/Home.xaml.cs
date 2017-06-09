using FirstFloor.ModernUI.Windows.Controls;
using System;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Windows;
using System.Windows.Controls;
using FirstFloor.ModernUI.Windows;
using Trivia_Client.Pages;
using FirstFloor.ModernUI.Windows.Navigation;

namespace Trivia_Client.Pages
{
    /// <summary>
    /// Interaction logic for Home.xaml
    /// </summary>
    public partial class Home : UserControl, IContent
    {
        private ModernFrame frame;


        public Home()
        {
            if (Connection())
            {
                if (!IsLoggedIn())
                {
                    SetConnectionOptions();
                }
                else
                    SetMenuPage();
            }
            InitializeComponent();
        }

        private bool Connection()
        {
            if ((Session.CurrentUser != null && Session.CurrentUser.GetSocket() != null) || Session.JustSignedUp)
                return true;
        
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

            Window.MenuLinkGroups.Clear();

            Window.TitleLinks.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "Settings",
                Source = new Uri("./Pages/SettingsPage.xaml", UriKind.Relative)
            });

            Window.MenuLinkGroups.Add(new FirstFloor.ModernUI.Presentation.LinkGroup()
            {
                DisplayName = "Connect"
            });

            Window.MenuLinkGroups.ElementAt(0).Links.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "Login",
                Source = new Uri("/Pages/Connection/Login.xaml", UriKind.Relative)
            });

            Window.MenuLinkGroups.ElementAt(0).Links.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "SignUp",
                Source = new Uri("/Pages/Connection/Signup.xaml", UriKind.Relative)
            });
        }

        private void SetMenuPage()
        {
            var Window = Application.Current.MainWindow as ModernWindow;

            Window.MenuLinkGroups.Clear();

            Window.MenuLinkGroups.Add(new FirstFloor.ModernUI.Presentation.LinkGroup()
            {
                DisplayName = "Menu"
            });

            Window.MenuLinkGroups.ElementAt(0).Links.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "Create Room",
                Source = new Uri("./Pages/Options/CreateRoom.xaml", UriKind.Relative)
            });

            Window.MenuLinkGroups.ElementAt(0).Links.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "Join Room",
                Source = new Uri("./Pages/Options/JoinRoom.xaml", UriKind.Relative)
            });

            Window.MenuLinkGroups.ElementAt(0).Links.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "My Scores",
                Source = new Uri("./Pages/Options/MyScores.xaml", UriKind.Relative)
            });

            Window.MenuLinkGroups.ElementAt(0).Links.Add(new FirstFloor.ModernUI.Presentation.Link()
            {
                DisplayName = "Personal State",
                Source = new Uri("./Pages/Options/PersonalState.xaml", UriKind.Relative)
            });
        }


        public void OnFragmentNavigation(FragmentNavigationEventArgs e)
        {
        }
        public void OnNavigatedFrom(NavigationEventArgs e)
        {
        }
        public void OnNavigatedTo(NavigationEventArgs e)
        {
            if (frame == null)
                frame = e.Frame;
        }
        public void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
        }
    }
}

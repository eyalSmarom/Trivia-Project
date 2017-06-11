using FirstFloor.ModernUI.Windows;
using FirstFloor.ModernUI.Windows.Controls;
using FirstFloor.ModernUI.Windows.Navigation;
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
using Trivia_Client.Pages;

namespace Trivia_Client.Pages
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : UserControl, IContent
    {
        public string _Username;
        public string _Password;
        private ModernFrame frame;

        public Login()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Signing in the user
        /// </summary>
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            string[] Values = new string[2]; // Setting an array for parameters.

            Values[0] = _Username = Username.Text;
            Values[1] = _Password = Password.Password;

            ClientReceivedMessage Message = new ClientReceivedMessage(ClientCodes.SignIn, Values);
            if(HandleSignIn(Session.CurrentUser.SendBackToServer(Message).Replace("\0", String.Empty)))
            {
                frame.Source = new Uri(Paths.Home, UriKind.Relative);
            }
        }

        /// <summary>
        /// Checking if the user was properly signed in
        /// </summary>
        /// <param name="ReturnedMessage">The string returned from the server</param>
        /// <returns>True if was signed in properly, false if not</returns>
        private bool HandleSignIn(string ReturnedMessage)
        {
            var Window = Application.Current.MainWindow as ModernWindow;

            if (ReturnedMessage.Equals(ServerCodes.SignInSuccess))
            {
                Session.CurrentUser.SetDetails(Username.Text, Password.Password);
                Session.Logged = true;

                Window.TitleLinks.Add(new FirstFloor.ModernUI.Presentation.Link
                {
                    DisplayName = Username.Text + " - Sign Out",
                    Source = new Uri(Paths.SignOut, UriKind.Relative)
                });

                Password.Password = null;
                Username.Text = null;
                return true;
            }

            else if (ReturnedMessage.Equals(ServerCodes.SignInWrong))
                ErrorMessage.Content = "Wrong Details. Try Again";
            else if (ReturnedMessage.Equals(ServerCodes.SignInAlreadyConnected))
                ErrorMessage.Content = "This User is Already Connected";
            else if (ReturnedMessage.Equals(ServerCodes.SignInFail))
                ErrorMessage.Content = "Login Failed... Please Try Again";
            return false;
        }
        #region Control Interface Implemented
        public void OnFragmentNavigation(FirstFloor.ModernUI.Windows.Navigation.FragmentNavigationEventArgs e)
        {
        }
        public void OnNavigatedFrom(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
        }
        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            if (!Session.Logged)
                frame = e.Frame;
            else
                e.Frame.Source = new Uri(Paths.ErrorPage, UriKind.Relative);
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
        }
        #endregion
    }
}

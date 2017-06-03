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

namespace Trivia_Client.Pages
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    public partial class Login : UserControl
    {
        public string _Username;
        public string _Password;

        public Login()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            string[] Values = new string[2]; // Setting an array for parameters.
            string ReturnedMessage;

            Values[0] = _Username = Username.Text;
            Values[1] = _Password = Password.Password;

            ClientRecievedMessage Message = new ClientRecievedMessage(ClientCodes.SignIn, Values);
            HandleSignIn(Session.CurrentUser.SendBackToServer(Message).Replace("\0", String.Empty));
        }

        private void HandleSignIn(string ReturnedMessage)
        {
            if (ReturnedMessage.Equals(ServerCodes.SignInSuccess))
                Username.Text = "Success";
            else
                Username.Text = "Wrong";
        }
    }
}

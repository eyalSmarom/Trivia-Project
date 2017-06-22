using FirstFloor.ModernUI.Windows;
using FirstFloor.ModernUI.Windows.Controls;
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
using Trivia_Client.Pages.Options;

namespace Trivia_Client.Pages
{
    /// <summary>
    /// Interaction logic for CreateRoom.xaml
    /// </summary>
    public partial class CreateRoom : UserControl, IContent
    {
        private ModernFrame frame;

        public CreateRoom()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Creates the room with the values inserted by the user
        /// </summary>
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string _RoomName, _PlayersNumber, _QuestionNumber, _QuestionTime;
                string[] Values = new string[4];
                _RoomName = RoomName.Text;
                _PlayersNumber = PlayersNumber.Text;
                _QuestionNumber = QuestionsNumber.Text;
                _QuestionTime = QuestionTime.Text;
                if (CheckValidity(_PlayersNumber, _QuestionNumber, _QuestionTime))
                {
                    Room CurrRoom = new Room(Convert.ToInt32(_QuestionNumber), Convert.ToInt32(_PlayersNumber), Convert.ToInt32(_QuestionTime), true, _RoomName);

                    Values[0] = _RoomName;
                    Values[1] = _PlayersNumber;
                    Values[2] = _QuestionNumber;
                    Values[3] = _QuestionTime;

                    ClientReceivedMessage Message = new ClientReceivedMessage(ClientCodes.CreateRoom, Values);
                    if (HandleCreateRoom(Session.CurrentUser.SendBackToServer(Message).Replace("\0", String.Empty), CurrRoom))
                    {
                        Session.CurrentUser.SetRoom(CurrRoom);
                        frame.Source = new Uri(Paths.RoomPage, UriKind.Relative);
                    }
                    else
                    {
                        ErrorMessage.Content = "Failed Creating The Room...";
                    }
                }
            }
            catch(Exception Exc) { ErrorMessage.Content = "Server Error... Please go out of this page and try again..."; }
        }

        /// <summary>
        /// Checks the validity of the values inserted by the user
        /// </summary>
        /// <returns>True if valid, false if not</returns>
        private bool CheckValidity(string PlayersNumber, string QuestionNumber, string QuestionTime)
        {
            if (Convert.ToInt16(PlayersNumber) > 9)
            {
                ErrorMessage.Content = "Wrong Players Number";
                return false;
            }
            else if (Convert.ToInt16(QuestionNumber) > 10)
            {
                ErrorMessage.Content = "Wrong Questions Number";
                return false;
            }
            else if (Convert.ToInt16(QuestionTime) >= 100)
            {
                ErrorMessage.Content = "Wrong Question Time";
                return false;
            }
            return true;
        }

        /// <summary>
        /// Checks if the room was created properly, if he is sets the current room's Id.
        /// </summary>
        /// <param name="ReturnedMessage">The string returned from the server</param>
        /// <param name="CurrRoom">The current room that was created with users values</param>
        /// <returns>True if created properly, false if not</returns>
        public bool HandleCreateRoom(string ReturnedMessage, Room CurrRoom)
        {
            byte[] bytes = new byte[1024];

            if (ReturnedMessage.Equals(ServerCodes.CloseRoom))
            {
                Session.CurrentUser.GetSocket().Receive(bytes);
                ReturnedMessage = Encoding.ASCII.GetString(bytes);
            }
            if (ReturnedMessage.Substring(0, 4).Equals(ServerCodes.CreateRoomSuccess))
            {
                ReturnedMessage = ReturnedMessage.Substring(4);
                CurrRoom.Id = Convert.ToInt16(ReturnedMessage.Substring(0, 4));
                return true;
            }
            else
                return false;
        }

        #region Control Interface Implementation
        public void OnFragmentNavigation(FirstFloor.ModernUI.Windows.Navigation.FragmentNavigationEventArgs e)
        {
        }
        public void OnNavigatedFrom(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            PlayersNumber.Text = "";
            RoomName.Text = "";
            QuestionsNumber.Text = "";
            QuestionTime.Text = "";
        }
        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            if (frame == null)
                frame = e.Frame;
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
  
        }
        #endregion

        /// <summary>
        /// Default values for the room
        /// </summary>
        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            RoomName.Text = Session.CurrentUser.GetUsername() + "'s Room";
            PlayersNumber.Text = "5";
            QuestionsNumber.Text = "4";
            QuestionTime.Text = "30";
        }

        /// <summary>
        /// Prevents from the textbox to accept non-numerical values.
        /// </summary>
        private void Number_KeyDown(object sender, KeyEventArgs e)
        {
            // Determine whether the keystroke is a number from the top of the keyboard.
            if (e.Key < Key.D0 || e.Key > Key.D9)
            {
                // Determine whether the keystroke is a number from the keypad.
                if (e.Key < Key.NumPad0 || e.Key > Key.NumPad9)
                {
                    // Determine whether the keystroke is a backspace.
                    if (e.Key != Key.Back)
                    {
                        // A non-numerical keystroke was pressed.
                        // Set the flag to true and evaluate in KeyPress event.
                        e.Handled = true;
                    }
                }
            }

            // Determine wheter the keystroke is a space
            if(e.Key == Key.Space)
            {
                e.Handled = true;
            }
        }
    }
}

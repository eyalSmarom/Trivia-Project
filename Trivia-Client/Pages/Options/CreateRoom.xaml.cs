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

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            string _RoomName, _PlayersNumber, _QuestionNumber, _QuestionTime;
            string[] Values = new string[4];
            _RoomName = RoomName.Text;
            _PlayersNumber = PlayersNumber.Text;
            _QuestionNumber = QuestionsNumber.Text;
            _QuestionTime = QuestionTime.Text;
            if (CheckValidity(_PlayersNumber, _QuestionNumber, _QuestionTime))
            {
                Room CurrRoom = new Room(1, Convert.ToInt16(_PlayersNumber));

                Values[0] = _RoomName;
                Values[1] = _PlayersNumber;
                Values[2] = _QuestionNumber;
                Values[3] = _QuestionTime;

                ClientRecievedMessage Message = new ClientRecievedMessage(ClientCodes.CreateRoom, Values);
                if (HandleCreateRoom(Session.CurrentUser.SendBackToServer(Message).Replace("\0", String.Empty)))
                {
                    Session.CurrentUser.SetRoom(CurrRoom);
                    frame.Source = new Uri("./Pages/Options/RoomPage.xaml", UriKind.Relative);
                }
                else
                {
                    ErrorMessage.Content = "Failed Creating The Room...";
                }
            }
        }

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

        public bool HandleCreateRoom(string ReturnedMessage)
        {
            if (ReturnedMessage.Equals(ServerCodes.CreateRoomSuccess))
                return true;
            else
                return false;
        }

        #region Control Interface Implementation
        public void OnFragmentNavigation(FirstFloor.ModernUI.Windows.Navigation.FragmentNavigationEventArgs e)
        {
        }
        public void OnNavigatedFrom(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
        }
        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            if (frame == null)
                frame = e.Frame;

            if (Session.CurrentUser != null && Session.CurrentUser.GetRoom() != null) // Also Need to add handle Close Room
            {
                Session.CurrentUser.SetRoom(null);
            }
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
        }
        #endregion
    }
}

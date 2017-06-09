using FirstFloor.ModernUI.Windows;
using FirstFloor.ModernUI.Windows.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
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

namespace Trivia_Client.Pages.Options
{
    /// <summary>
    /// Interaction logic for RoomPage.xaml
    /// </summary>
    public partial class RoomPage : UserControl, IContent
    {
        private ModernFrame frame;

        public RoomPage()
        {
            InitializeComponent();
            InitiateDetails();
        }

        /// <summary>
        /// Initiates the room's details (Name, Questions Details And Players Connected)
        /// </summary>
        public void InitiateDetails()
        {
            Room ThisRoom = Session.CurrentUser.GetRoom();
            ClientReceivedMessage Message = new ClientReceivedMessage(ClientCodes.AllRoomUsers, new string[] { ThisRoom.Id.ToString().PadLeft(4, '0') });
            ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.AllRoomUsers, Session.CurrentUser.SendBackToServer(Message).Replace("\0", String.Empty)); // Getting all the room Users

            FFlush();

            RoomName.Text = ThisRoom.RoomName; // Initiating The Room's Name.

            int NumberOfPlayers = Convert.ToInt16(ServerMessage._Values[0]);
            PlayersNum.Text = "Number of connected users: " + NumberOfPlayers + "/" + ThisRoom.MaxUsers; // initiating Number of Players.

            QuestionNum.Text = "Number of question in the trivia: " + ThisRoom.QuestionNumber; // Initiating Questions Number.

            QuestionTime.Text = "Seconds to answer a question: " + ThisRoom.QuestionTime; // Initiating Question Time.

            List<string> Users = new List<string>(Convert.ToInt16(ServerMessage._Values[0]));

            for (int i = 1; i <= Convert.ToInt16(ServerMessage._Values[0]); i++)
            {
                Users.Add(ServerMessage._Values[i]);
            }

            ListViewItem temp;
            foreach (var item in Users)
            {
                temp = new ListViewItem();
                temp.Content = item;
                Players.Items.Add(temp);
            }

        }

        public bool HandleCloseRoom(string ReturnedMessage)
        {
            if (ReturnedMessage.Equals(ServerCodes.CloseRoom))
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
            ClientReceivedMessage Message;

            if (Session.CurrentUser != null && Session.CurrentUser.GetRoom() != null && Session.CurrentUser.GetRoom().IsAdmin) // Also Need to add handle Close Room Only If the Player Was Admin
            {
                Session.CurrentUser.SetRoom(null);
                Message = new ClientReceivedMessage(ClientCodes.CloseRoom);
                HandleCloseRoom(Session.CurrentUser.SendBackToServer(Message).Replace("\0", String.Empty));
            }
        }
        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
            if (frame == null)
                frame = e.Frame;

            if (Session.CurrentUser != null && Session.CurrentUser.GetRoom() == null)
                e.Frame.Source = new Uri("./Pages/Options/CreateRoom.xaml", UriKind.Relative);
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
            
        }
        #endregion

        private void FFlush()
        {
            byte[] bytes = new byte[1024];
            Session.CurrentUser.GetSocket().Receive(bytes);
        }
    }
}

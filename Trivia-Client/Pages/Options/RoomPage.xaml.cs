using FirstFloor.ModernUI.Windows;
using FirstFloor.ModernUI.Windows.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
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
        private Thread HandleRequests;

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
            var Window = Application.Current.MainWindow as ModernWindow;

            Window.TitleLinks.RemoveAt(0);

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
            Players.Items.Clear();
            foreach (var item in Users)
            {
                temp = new ListViewItem();
                temp.Content = item;
                Players.Items.Add(temp);
            }

            if (!ThisRoom.IsAdmin)
                StartGame.Visibility = Visibility.Collapsed;

            HandleRequests = new Thread(new ThreadStart(HandlePlayers)); // A Thread to listen to other players join/Exit

            HandleRequests.Start();
        }

        /// <summary>
        /// Checks if the the room was closed properly
        /// </summary>
        /// <param name="ReturnedMessage">The string returned from the server</param>
        /// <returns>True if closed properly, false if not</returns>
        public bool HandleCloseRoom(string ReturnedMessage)
        {
            if (ReturnedMessage.Equals(ServerCodes.CloseRoom + "0"))
            {
                Session.CurrentUser.SetRoom(null);
                return true;
            }
            else
                return false;
        }

        /// <summary>
        /// Checks if the user left the room properly
        /// </summary>
        /// <param name="ReturnedMessage">The string returned from the server</param>
        /// <returns>True if left properly, false if not</returns>
        public bool HandleLeaveRoom(string ReturnedMessage)
        {
            if (ReturnedMessage.Equals(ServerCodes.LeaveRoom + "0"))
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
            else
                InitiateDetails();
            if (Session.CurrentUser != null && Session.CurrentUser.GetRoom() == null)
                e.Frame.Source = new Uri("./Pages/Options/CreateRoom.xaml", UriKind.Relative);
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
            ClientReceivedMessage Message;

            if (Session.CurrentUser != null && Session.CurrentUser.GetRoom() != null && Session.CurrentUser.GetRoom().IsAdmin) // Also Need to add handle Close Room Only If the Player Was Admin
            {
                Session.CurrentUser.SetRoom(null);
                Message = new ClientReceivedMessage(ClientCodes.CloseRoom);
                Session.CurrentUser.GetSocket().Send(Encoding.ASCII.GetBytes(Message._StringedMessage));
            }
            else if (Session.CurrentUser != null && Session.CurrentUser.GetRoom() != null && !Session.CurrentUser.GetRoom().IsAdmin)
            {
                Session.CurrentUser.SetRoom(null);
                Message = new ClientReceivedMessage(ClientCodes.LeaveRoom);
                Session.CurrentUser.GetSocket().Send(Encoding.ASCII.GetBytes(Message._StringedMessage));
            }
        }
        #endregion

        /// <summary>
        /// Get's the server Response another time in order to clean the buffer
        /// </summary>
        private void FFlush()
        {
            byte[] bytes = new byte[1024];
            Session.CurrentUser.GetSocket().Receive(bytes);
        }

        #region Thread
        /// <summary>
        /// The main function for the thread which listens to new Requests
        /// </summary>
        private void HandlePlayers()
        {
            while (true)
            {
                byte[] bytes = new byte[1024];
                string Response;

                Session.CurrentUser.GetSocket().Receive(bytes);
                Response = Encoding.ASCII.GetString(bytes).Replace("\0", String.Empty);
                if (Response.Substring(0, 3).Equals(ServerCodes.CloseRoom)) // Closing the Room
                {
                    HandleCloseRoom(Response);
                    if (Session.CurrentUser.GetRoom() != null && !Session.CurrentUser.GetRoom().IsAdmin)
                        frame.Dispatcher.Invoke(new ChangeFrameCallback(ChangeFrame), new Uri(Paths.Home, UriKind.Relative));
                    break;
                }
                else if (Response.Substring(0, 3).Equals(ServerCodes.LeaveRoom)) // Leaving the Room
                {
                    HandleLeaveRoom(Response);
                    break;
                }
                else
                {
                    ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.AllRoomUsers, Response);
                    Players.Dispatcher.Invoke(new UpdateViewCallback(UpdateView), ServerMessage);
                }
            }
        }

        public delegate void UpdateViewCallback(ServerReceivedMessage ServerMessage);
        public delegate void ChangeFrameCallback(Uri Path);

        /// <summary>
        /// Changing the frame (Navigation)
        /// </summary>
        /// <param name="Path">The path to navigate to</param>
        private void ChangeFrame(Uri Path)
        {
            frame.Source = Path;
        }

        /// <summary>
        /// Updating the view according to the new message from the server
        /// </summary>
        /// <param name="ServerMessage">The message from the server</param>
        private void UpdateView(ServerReceivedMessage ServerMessage)
        {
            Room ThisRoom = Session.CurrentUser.GetRoom();

            List<string> Users = new List<string>(Convert.ToInt16(ServerMessage._Values[0]));

            for (int i = 1; i <= Convert.ToInt16(ServerMessage._Values[0]); i++)
            {
                Users.Add(ServerMessage._Values[i]);
            }

            PlayersNum.Text = "Number of connected users: " + Users.Count + "/" + ThisRoom.MaxUsers; // initiating Number of Players.

            ListViewItem temp;
            Players.Items.Clear();

            foreach (var item in Users)
            {
                temp = new ListViewItem();
                temp.Content = item;
                Players.Items.Add(temp);
            }
        }
    #endregion
    }
}

using FirstFloor.ModernUI.Windows;
using FirstFloor.ModernUI.Windows.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
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
    /// Interaction logic for JoinRoom.xaml
    /// </summary>
    public partial class JoinRoom : UserControl, IContent
    {
        private ModernFrame frame;

        public JoinRoom()
        {
            InitializeComponent();
            InitializeRooms();
        }

        private void InitializeRooms()
        {
            ClientReceivedMessage ClientMessage = new ClientReceivedMessage(ClientCodes.AllRoomsList);
            ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.AllRooms, Session.CurrentUser.SendBackToServer(ClientMessage).Replace("/0", String.Empty));

            Dictionary<string, int> ExistingRooms = new Dictionary<string, int>(Convert.ToInt16(ServerMessage._Values[0]));

            for(int i = 1; i <= Convert.ToInt16(ServerMessage._Values[0]) + 1 && Convert.ToInt16(ServerMessage._Values[0]) != 0; i += 2)
            {
                ExistingRooms.Add(ServerMessage._Values[i + 1], Convert.ToInt32(ServerMessage._Values[i]));
            }

            ListViewItem temp;
            Rooms.Items.Clear();

            foreach (var item in ExistingRooms)
            {
                temp = new ListViewItem();
                temp.Content = item.Key;
                temp.Uid = item.Value.ToString();
                Rooms.Items.Add(temp);
            }
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
                InitializeRooms();
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
            
        }
        #endregion

        private void JoinRoomButton_Click(object sender, RoutedEventArgs e)
        {
            ListViewItem temp = (ListViewItem)Rooms.SelectedItem;

            ClientReceivedMessage ClientMessage = new ClientReceivedMessage(ClientCodes.JoinRoom, new string[] { temp.Uid.PadLeft(4, '0') });
            ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.JoinRoom, Session.CurrentUser.SendBackToServer(ClientMessage).Replace("/0", String.Empty));

            ServerReceivedMessage Disposal = new ServerReceivedMessage(ServerCodes.AllRoomUsers, GetUsers());

            if(ServerMessage.ErrorMessage == null)
            {
                Room Joined = new Room(Convert.ToInt16(ServerMessage._Values[0]), Convert.ToInt16(ServerMessage._Values[2]), Convert.ToInt16(ServerMessage._Values[1]), false, temp.Content.ToString());
                Joined.Id = Convert.ToInt16(temp.Uid);

                Session.CurrentUser.SetRoom(Joined);
                frame.Source = new Uri("./Pages/Options/RoomPage.xaml", UriKind.Relative);
            }
        }

        private string GetUsers()
        {
            byte[] bytes = new byte[1024];
            Session.CurrentUser.GetSocket().Receive(bytes);

            return Encoding.ASCII.GetString(bytes).Replace("/0", String.Empty);
        }
    }
}

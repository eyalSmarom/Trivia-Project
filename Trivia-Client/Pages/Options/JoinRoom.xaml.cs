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

            List<string> RoomsNames = new List<string>(Convert.ToInt16(ServerMessage._Values[0]));

            for(int i = 0; i < Convert.ToInt16(ServerMessage._Values[0]); i++)
            {
                RoomsNames.Add(ServerMessage._Values[i]);
            }

            ListViewItem temp;

            foreach (var item in RoomsNames)
            {
                temp = new ListViewItem();
                temp.Content = item;
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
        }
        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {

        }
        #endregion
    }
}

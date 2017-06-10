﻿using FirstFloor.ModernUI.Windows.Controls;
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

namespace Trivia_Client.Pages.Connection
{
    /// <summary>
    /// Interaction logic for SignOut.xaml
    /// </summary>
    public partial class SignOut : UserControl
    {
        private ModernFrame frame;

        public SignOut()
        {
            InitializeComponent();
            SignUserOut();
        }

        public void SignUserOut()
        {
            ClientReceivedMessage ClientMessage = new ClientReceivedMessage(ClientCodes.SignOut);
            Session.CurrentUser.GetSocket().Send(Encoding.ASCII.GetBytes(ClientMessage._StringedMessage));

            Session.CurrentUser = null;
            Session.JustSignedUp = false;
            Session.Logged = false;

            App.Current.Shutdown();
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

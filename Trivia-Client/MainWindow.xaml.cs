﻿using FirstFloor.ModernUI.Windows.Controls;
using FirstFloor.ModernUI.Windows.Navigation;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
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
using Trivia_Client.Pages.Settings;

namespace Trivia_Client
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : ModernWindow
    {
        public MainWindow()
        {
            InitializeComponent();
            AppearanceViewModel appearence = new AppearanceViewModel();
        }

        /// <summary>
        /// Ending communication between the server and the client.
        /// </summary>
        private void ModernWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if(Session.CurrentUser != null && Session.CurrentUser.GetSocket() != null)
            {
                string EndCommunication = ClientCodes.LeaveApp;
                Session.CurrentUser.GetSocket().Send(Encoding.ASCII.GetBytes(EndCommunication));
            }
        }
    }
}

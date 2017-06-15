using FirstFloor.ModernUI.Windows;
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
using FirstFloor.ModernUI.Windows.Navigation;

namespace Trivia_Client.Pages.Options
{
    /// <summary>
    /// Interaction logic for Scores.xaml
    /// </summary>
    public partial class Scores : UserControl, IContent
    {
        private NavigationWindow NavWindow;

        public Scores(ServerReceivedMessage ServerMessage, NavigationWindow MyWindow)
        {
            InitializeComponent();
            InitializeScores(ServerMessage);
            NavWindow = MyWindow;
        }

        private void InitializeScores(ServerReceivedMessage ServerMessage)
        {
            ListViewItem Item;
            UIScores.Items.Clear();
            for(int i = 0; i < ServerMessage._Values.Length; i += 2)
            {
                Item = new ListViewItem();
                Item.Content = ServerMessage._Values[i] + " - " + Convert.ToInt16(ServerMessage._Values[i + 1]);
                UIScores.Items.Add(Item);
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            NavWindow.Close();
        }

        public void OnFragmentNavigation(FirstFloor.ModernUI.Windows.Navigation.FragmentNavigationEventArgs e)
        {
        }

        public void OnNavigatedFrom(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
        }

        public void OnNavigatedTo(FirstFloor.ModernUI.Windows.Navigation.NavigationEventArgs e)
        {
        }

        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
        }
    }
}

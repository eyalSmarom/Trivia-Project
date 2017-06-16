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
using FirstFloor.ModernUI.Windows.Controls;
using System.Windows.Threading;
using System.Threading;

namespace Trivia_Client.Pages.Options
{
    /// <summary>
    /// Interaction logic for Game.xaml
    /// </summary>
    public partial class Game : UserControl, IContent
    {
        private Thread Timer;
        private ModernFrame frame;
        private int QuestionCounter, CorrectAnswers;

        public Game()
        {
            InitializeComponent();
            QuestionCounter = CorrectAnswers = 0;
            SetDetails();
        }

        private void SetDetails()
        {
            QuestionCounter++;

            ResetButtons();

            Trivia_Client.Game CurrentGame = Session.CurrentUser.GetGame();

            Title.Text = "Question Number " + QuestionCounter.ToString();

            Question.Text = CurrentGame.CurrentQuestion._Question;

            Answer1.Content = CurrentGame.CurrentQuestion._Answers[0];
            Answer2.Content = CurrentGame.CurrentQuestion._Answers[1];
            Answer3.Content = CurrentGame.CurrentQuestion._Answers[2];
            Answer4.Content = CurrentGame.CurrentQuestion._Answers[3];

            Clock.Text = CurrentGame.QuestionTime.ToString();

            Timer = new Thread(new ThreadStart(Tick));
            Timer.Start();
        }


        #region Thread
        public delegate void TickCallback();
        public delegate int ClockCallback();
        /// <summary>
        /// The Main Function for the thread.
        /// </summary>
        private void Tick() 
        {
            while (!Clock.Dispatcher.Invoke(new ClockCallback(ClockTime)).Equals(0))
            {
                Thread.Sleep(1000);
                Clock.Dispatcher.Invoke(new TickCallback(Timer_Tick));
            }
            DefaultAnswer();
        }   

        void Timer_Tick()
        {
            Clock.Text = (Convert.ToInt16(Clock.Text) - 1).ToString();
        }

        int ClockTime()
        {
            return Convert.ToInt32(Clock.Text);
        }
        #endregion

        private void Answer_Click(int QuestionNumber)
        {
            Timer.Abort(); // Stopping the timer.
            string[] Values = new string[2];

            Values[0] = QuestionNumber.ToString();
            Values[1] = Clock.Text.PadLeft(2, '0');

            ClientReceivedMessage ClientMessage = new ClientReceivedMessage(ClientCodes.Answer, Values);
            ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.TrueFalse, Session.CurrentUser.SendBackToServer(ClientMessage));

            Answer1.IsEnabled = false;
            Answer2.IsEnabled = false;
            Answer3.IsEnabled = false;
            Answer4.IsEnabled = false;

            if (ServerMessage._StringedMessage.Equals(ServerCodes.TrueFalse + "1"))
            {
                CorrectAnswers++;
                UpdateButtonColor(QuestionNumber, true);
            }
            else
            {
                UpdateButtonColor(QuestionNumber, false);
            }
            Answer1.Dispatcher.Invoke((Action)(() => { }), DispatcherPriority.Render); // Forcing wpf to process the UI modification before the thread sleeps.
            Thread.Sleep(500);
            GetNextQuestion();
        }

        private void DefaultAnswer()
        {
            string[] Values = new string[2];
            Values[0] = "5";
            Values[1] = Session.CurrentUser.GetGame().QuestionTime.ToString().PadLeft(2, '0');

            ClientReceivedMessage ClientMessage = new ClientReceivedMessage(ClientCodes.Answer, Values);
            ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.TrueFalse, Session.CurrentUser.SendBackToServer(ClientMessage));
        }

        private void GetNextQuestion()
        {
            byte[] bytes = new byte[1024];
            string Response;

            Session.CurrentUser.GetSocket().Receive(bytes);
            Response = Encoding.ASCII.GetString(bytes);

            if (Response.Substring(0, 3).Equals(ServerCodes.SendQuestions))
            {
                Trivia_Client.Game CurrentGame = Session.CurrentUser.GetGame();
                Question Next = new Trivia_Client.Question();

                ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.SendQuestions, Response);

                Next._Question = ServerMessage._Values[0];
                for (int i = 0; i < 4; i++)
                {
                    Next._Answers.Add(ServerMessage._Values[i + 1]);
                }
                CurrentGame.CurrentQuestion = Next;
                SetDetails();
            }
            else if (Response.Substring(0, 3).Equals(ServerCodes.EndGame))
            {
                ServerReceivedMessage ServerMessage = new ServerReceivedMessage(ServerCodes.EndGame, Response);

                HandleEndGame(ServerMessage);

                Session.CurrentUser.GetSocket().Send(Encoding.ASCII.GetBytes(ClientCodes.LeaveGame));
                frame.Source = new Uri(Paths.Home, UriKind.Relative);
            }
        }

        private void UpdateButtonColor(int QuestionNum, bool Indicator)
        {
            if (Indicator)
            {
                switch (QuestionNum)
                {
                    case 1:
                        Answer1.Background = Brushes.ForestGreen;
                        break;

                    case 2:
                        Answer2.Background = Brushes.ForestGreen;
                        break;

                    case 3:
                        Answer3.Background = Brushes.ForestGreen;
                        break;

                    case 4:
                        Answer4.Background = Brushes.ForestGreen;
                        break;
                }
            }
            else
            {
                switch (QuestionNum)
                {
                    case 1:
                        Answer1.Background = Brushes.Red;
                        break;

                    case 2:
                        Answer2.Background = Brushes.Red;
                        break;

                    case 3:
                        Answer3.Background = Brushes.Red;
                        break;

                    case 4:
                        Answer4.Background = Brushes.Red;
                        break;
                }
            }
        }

        private void ResetButtons()
        {
            Answer1.IsEnabled = true;
            Answer1.Background = Brushes.DimGray;

            Answer2.IsEnabled = true;
            Answer2.Background = Brushes.DimGray;

            Answer3.IsEnabled = true;
            Answer3.Background = Brushes.DimGray;

            Answer4.IsEnabled = true;
            Answer4.Background = Brushes.DimGray;
        }

        private void HandleEndGame(ServerReceivedMessage ServerMessage)
        {
            NavigationWindow GameScores = new NavigationWindow();
            GameScores.Navigate(new Scores(ServerMessage, GameScores));
            GameScores.Show();

            ResetButtons();

            Session.CurrentUser.SetGame(null);
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
            {
                QuestionCounter = CorrectAnswers = 0;
                SetDetails();
            }

            if (Session.CurrentUser.GetGame() == null)
                e.Frame.Source = new Uri(Paths.Home, UriKind.Relative);
        }

        public void OnNavigatingFrom(FirstFloor.ModernUI.Windows.Navigation.NavigatingCancelEventArgs e)
        {
            
        }
        #endregion

        #region Navigation Functions
        private void Answer1_Click(object sender, RoutedEventArgs e)
        {
            Answer_Click(1);
        }

        private void Answer2_Click(object sender, RoutedEventArgs e)
        {
            Answer_Click(2);
        }

        private void Answer3_Click(object sender, RoutedEventArgs e)
        {
            Answer_Click(3);
        }

        private void Answer4_Click(object sender, RoutedEventArgs e)
        {
            Answer_Click(4);
        }
        #endregion
    }
}

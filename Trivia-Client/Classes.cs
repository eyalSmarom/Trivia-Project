using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;
using FirstFloor.ModernUI.Windows;
using System.Windows.Controls;
using FirstFloor.ModernUI.Windows.Navigation;
using FirstFloor.ModernUI.Windows.Controls;

namespace Trivia_Client
{

    public static class Session
    {
        public static User CurrentUser = null;
        public static bool Logged = false;
        public static bool JustSignedUp = false;
    }
    #region User Room Game Question
    public class User
    {
        private string Username;
        private string Password;
        private Socket ClientSocket;
        private Room CurrRoom;
        private Game CurrGame;
        

        public User()
        {
            Username = null;
            Password = null;
        }

        public User(string Username, string Password)
        {
            this.Username = Username;
            this.Password = Password;
        }

        public User(User other)
        {
            Username = other.Username;
            Password = other.Password;
        }

        public User(Socket ClientSocket)
        {
            this.ClientSocket = ClientSocket;
        }

        public void SetDetails(string Username, string Password)
        {
            this.Username = Username;
            this.Password = Password;
        }

        public Socket GetSocket()
        {
            return ClientSocket;
        }

        public void SetGame(Game ToChange)
        {
            CurrGame = ToChange;
        }

        public Game GetGame()
        {
            return CurrGame;
        }

        public void SetRoom(Room ToChange)
        {
            CurrRoom = ToChange;
        }

        public Room GetRoom()
        {
            return CurrRoom;
        }

        public string SendBackToServer(ClientReceivedMessage Message)
        {
            byte[] ToSend = Encoding.ASCII.GetBytes(Message._StringedMessage);
            byte[] ToRecieve = new byte[1024];
            try
            {
                ClientSocket.Send(ToSend);
                int bytesRec = ClientSocket.Receive(ToRecieve);
                return Encoding.ASCII.GetString(ToRecieve).Replace("\0", String.Empty);
            }
            catch(Exception e) { return null; }
        }

        public string GetUsername()
        {
            return Username;
        }
    }

    public class Room
    {
        public int MaxUsers { get; set; }
        public int QuestionNumber { get; set; }
        public int QuestionTime { get; set; }
        public List<string> Users { get; private set; }
        public bool IsAdmin { get; set; }
        public int Id { get; set; }
        public string RoomName { get; set; }

        public Room(int QuestionNumber, int MaxUsers, int QuestionTime, bool IsAdmin, string RoomName)
        {
            this.QuestionNumber = QuestionNumber;
            this.QuestionTime = QuestionTime;
            this.MaxUsers = MaxUsers;
            this.IsAdmin = IsAdmin;
            this.RoomName = RoomName;
        }

        public void SetUsers(List<string> other)
        {
            this.Users = new List<string>(other);
        }
    }

    public class Game
    {
        public int NumberOfUsers { get; private set; }
        public int QuestionNumber { get; private set; }
        public int QuestionTime { get; private set; }
        public Question CurrentQuestion { get; set; }

        public Game(int NumberOfUsers, int QuestionNumber, int QuestionTime)
        {
            this.NumberOfUsers = NumberOfUsers;
            this.QuestionNumber = QuestionNumber;
            this.QuestionTime = QuestionTime;
        }
    }

    public class Question
    {
        public string _Question;
        public List<string> _Answers;

        public Question()
        {
            _Answers = new List<string>(4);
        }
    }
    #endregion

    #region ReceivedMessages
    public class ClientReceivedMessage
    {
        public string[] _Values { get; }
        public string _MessageCode { get; }
        public string _StringedMessage { get; set; }

        public ClientReceivedMessage(string Code)
        {
            _MessageCode = Code;
            _Values = new string[0];
            _StringedMessage = _MessageCode;
        }

        public ClientReceivedMessage(string Code, string[] Values)
        {
            _MessageCode = Code;
            _Values = new string[Values.Length];

            for (int i = 0; i < Values.Length; i++)
                _Values[i] = Values[i];

            _StringedMessage = _MessageCode;

            switch (_MessageCode)
            {
                case ClientCodes.SignIn:
                    StringedSignIn();
                    break;
                case ClientCodes.SignUp:
                    StringedSignUp();
                    break;
                case ClientCodes.AllRoomUsers:
                    StringedAllRoomUsers();
                    break;
                case ClientCodes.JoinRoom:
                    StringedJoinRoom();
                    break;
                case ClientCodes.CreateRoom:
                    StringedCreateRoom();
                    break;
                case ClientCodes.Answer:
                    StringedAnswer();
                    break;
            }
        }

        public void StringedSignIn()
        {
            _StringedMessage += _Values[0].Length.ToString().PadLeft(2, '0'); // username size
            _StringedMessage += _Values[0]; // username

            _StringedMessage += _Values[1].Length.ToString().PadLeft(2, '0'); // password size
            _StringedMessage += _Values[1]; // password
        }

        public void StringedSignUp()
        {
            _StringedMessage += _Values[0].Length.ToString().PadLeft(2, '0'); // username size
            _StringedMessage += _Values[0]; // username

            _StringedMessage += _Values[1].Length.ToString().PadLeft(2, '0'); // password size
            _StringedMessage += _Values[1]; // password

            _StringedMessage += _Values[2].Length.ToString().PadLeft(2, '0'); // email size
            _StringedMessage += _Values[2]; // email
        }

        public void StringedAllRoomUsers()
        {
            _StringedMessage += _Values[0].PadLeft(4, '0');
        }

        public void StringedJoinRoom()
        {
            _StringedMessage += _Values[0].PadLeft(4, '0');
        }

        public void StringedCreateRoom()
        {
            _StringedMessage += _Values[0].Length.ToString().PadLeft(2, '0'); // Room Name size
            _StringedMessage += _Values[0]; // Room Name

            _StringedMessage += _Values[1]; // Players

            _StringedMessage += _Values[2].PadLeft(2, '0'); // Questions number

            _StringedMessage += _Values[3].PadLeft(2, '0'); // Questions Answer Time
        }

        public void StringedAnswer()
        {
            _StringedMessage += _Values[0]; // Answer Number

            _StringedMessage += _Values[1].PadLeft(2, '0'); // Answer Time
        }
    }

    public class ServerReceivedMessage
    {
        public string[] _Values { get; private set; }
        public string _MessageCode { get; private set; }
        public string _StringedMessage { get; set; }
        public Exception ErrorMessage { get; private set; }

        public ServerReceivedMessage(string Code)
        {
            _MessageCode = _StringedMessage = Code;
            _Values = new string[0];
        }

        public ServerReceivedMessage(string Code, string StringedMessage)
        {
            _StringedMessage = StringedMessage;

            switch(Code)
            {
                case ServerCodes.AllRooms:
                    ParameteredAllRooms();
                    break;

                case ServerCodes.AllRoomUsers:
                    ParameteredAllRoomUsers();
                    break;

                case ServerCodes.JoinRoom:
                    ParameteredJoinRoom();
                    break;

                case ServerCodes.SendQuestions:
                    ParameteredSendQuestion();
                    break;

                case ServerCodes.EndGame:
                    ParameteredEndGame();
                    break;

                case ServerCodes.BestScores:
                    ParameteredBestScores();
                    break;

                case ServerCodes.PersonalState:
                    ParameteredPersonalState();
                    break;
            }
        }

        public void ParameteredAllRooms()
        {
            string temp = _StringedMessage;
            int RoomNameSize;
            temp = temp.Substring(3);

            _Values = new string[1024]; // max rooms

            _Values[0] = temp.Substring(0, 4); // Number of Rooms
            temp = temp.Substring(4);

            for(int i = 1; i <= Convert.ToInt16(_Values[0]) * 2 && Convert.ToInt16(_Values[0]) != 0; i += 2)
            {
                _Values[i] = temp.Substring(0, 4); // Id
                temp = temp.Substring(4); // skip[ing the Id

                RoomNameSize = Convert.ToInt16(temp.Substring(0, 2));
                temp = temp.Substring(2); // skipping the Room name size

                _Values[i + 1] = temp.Substring(0, RoomNameSize); //RoomName
                temp = temp.Substring(RoomNameSize); // Skipping the RoomName
            }

        }

        public void ParameteredAllRoomUsers()
        {
            string temp = _StringedMessage;
            int NumberOfPlayers, UserLength;
            temp = temp.Substring(3); // skipping the code.

            NumberOfPlayers = Convert.ToInt16(temp.Substring(0, 1));
            _Values = new string[1 + (2 * NumberOfPlayers)];
            _Values[0] = NumberOfPlayers.ToString();
            temp = temp.Substring(1); // skipping the number of players.

            for(int i = 1; i <= NumberOfPlayers; i++)
            {
                UserLength = Convert.ToInt16(temp.Substring(0, 2));
                temp = temp.Substring(2); // Skipping the user length

                _Values[i] = temp.Substring(0, UserLength);
                temp = temp.Substring(UserLength); // Skiping the User Name.
            }
        }

        public void ParameteredJoinRoom()
        {
            string temp = _StringedMessage, Indicator;

            temp = temp.Substring(3); // Skipping the Message Code.

            Indicator = temp.Substring(0, 1); // getting the status.
            temp = temp.Substring(1);

            switch (Indicator)
            {
                case "0":
                    _Values = new string[3];

                    _Values[0] = temp.Substring(0, 2);
                    temp = temp.Substring(2);

                    _Values[1] = temp.Substring(0, 2);
                    temp = temp.Substring(2);

                    _Values[2] = temp.Substring(0, 1);
                    break;

                case "1":
                    ErrorMessage = new Exception("Room is Full");
                    break;

                case "2":
                    ErrorMessage = new Exception("Room doesn't exist or Other");
                    break;

                default:
                    ErrorMessage = new Exception("Unknown Error");
                    break;
            }
        }

        public void ParameteredSendQuestion()
        {
            string temp = _StringedMessage;
            int Length;
            _MessageCode = temp.Substring(0, 3);
            temp = temp.Substring(3); // Skipping the Message Code.

            _Values = new string[5];

            for(int i = 0; i < 5; i++)
            {
                Length = Convert.ToInt16(temp.Substring(0, 3));
                temp = temp.Substring(3); // Skipping the length

                _Values[i] = temp.Substring(0, Length);
                temp = temp.Substring(Length); // Skipping the name
            }
        }

        public void ParameteredEndGame()
        {
            string temp = _StringedMessage;
            int Length;
            _MessageCode = temp.Substring(0, 3);
            temp = temp.Substring(3);

            int NumberOfUsers = Convert.ToInt16(temp.Substring(0, 1));
            temp = temp.Substring(1);

            _Values = new string[NumberOfUsers * 2];

            for(int i = 1; i < NumberOfUsers * 2; i += 2)
            {
                Length = Convert.ToInt16(temp.Substring(0, 2));
                temp = temp.Substring(2);

                _Values[i - 1] = temp.Substring(0, Length);
                temp = temp.Substring(Length);

                _Values[i] = temp.Substring(0, 2);
                temp = temp.Substring(2);
            }
        }

        public void ParameteredBestScores()
        {
            string temp = _StringedMessage.Replace("\0", String.Empty);
            int Length, i;
            temp = temp.Substring(3); // Skipping the Message code.

            string[] TempValues = new string[6];

            for(i = 0; i < 6 && temp != String.Empty; i += 2)
            {
                Length = Convert.ToInt16(temp.Substring(0, 2));
                temp = temp.Substring(2);

                TempValues[i] = temp.Substring(0, Length); // Username
                temp = temp.Substring(Length);

                TempValues[i + 1] = temp.Substring(0, 6); // Scores
                temp = temp.Substring(6);
            }

            _Values = new string[i];
            
            for(int j = 0; j < i; j++)
            {
                _Values[j] = TempValues[j];
            }
        }

        public void ParameteredPersonalState()
        {
            
        }
    }
    #endregion

    #region Codes
    public static class ClientCodes
    {
        public const string SignIn = "200";
        public const string SignOut = "201";
        public const string SignUp = "203";
        public const string AllRoomsList = "205";
        public const string AllRoomUsers = "207";
        public const string JoinRoom = "209";
        public const string LeaveRoom = "211";
        public const string CreateRoom = "213";
        public const string CloseRoom = "215";
        public const string StartGame = "217";
        public const string Answer = "219";
        public const string LeaveGame = "222";
        public const string BestScores = "223";
        public const string PersonalState = "225";
        public const string LeaveApp = "299";
    }

    public static class ServerCodes
    {
        #region SignIn
        public const string SignInSuccess = "1020";
        public const string SignInWrong = "1021";
        public const string SignInAlreadyConnected = "1022";
        public const string SignInFail = "1023";
        #endregion // using Regions to make code more easier to read.
        #region SignUp
        public const string SignUpSuccess = "1040";
        public const string SignUpPassIllegal = "1041";
        public const string SignUpUsernameExists = "1042";
        public const string SignUpUsernameIllegal = "1043";
        public const string SignUpOther = "1044";
        #endregion
        #region JoinRoom
        public const string JoinRoom = "110";
        public const string JoinRoomSuccess = "1100";
        public const string JoinRoomFull = "1101";
        public const string JoinRoomFail = "1102";
        #endregion
        #region CreateRoom
        public const string CreateRoom = "114";
        public const string CreateRoomSuccess = "1140";
        public const string CreateRoomFail = "1141";
        #endregion
        public const string AllRooms = "106";
        public const string AllRoomUsers = "108";
        public const string LeaveRoom = "112";
        public const string CloseRoom = "116";
        public const string SendQuestions = "118";
        public const string TrueFalse = "120";
        public const string EndGame = "121";
        public const string BestScores = "124";
        public const string PersonalState = "126";
    }
    #endregion

    public static class Paths
    {
        public const string SettingsPage = "./Pages/SettingsPage.xaml";
        public const string Home = "./Pages/Home.xaml";
        public const string ErrorPage = "./Pages/ErrorPage.xaml";
        public const string RoomPage = "./Pages/Options/RoomPage.xaml";
        public const string JoinRoom = "./Pages/Options/JoinRoom.xaml";
        public const string CreateRoom = "./Pages/Option/CreateRoom.xaml";
        public const string SignUp = "./Pages/Connection/SignUp.xaml";
        public const string SignOut = "./Pages/Connection/SignOut.xaml";
        public const string Login = "./Pages/Connection/Login.xaml";
        public const string GamePage = "./Pages/Options/Game.xaml";
        public const string Scores = "./Pages/Options/Scores.xaml";
        public const string BestScores = "./Pages/Options/BestScores.xaml";
    }
}

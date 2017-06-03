using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net.Sockets;

namespace Trivia_Client
{
    public static class Session
    {
        public static User CurrentUser = null;
        public static bool Logged = false;
    }

    public class User
    {
        private string Username
        {
            get
            {
                return this.Username;
            }
            set
            {
                if (value is string)
                    this.Username = value;
            }
        }
        private string Password
        {
            get
            {
                return this.Password;
            }
            set
            {
                if (value is string)
                    this.Password = value;
            }
        }
        private Socket ClientSocket;
        

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

        public string SendBackToServer(ClientRecievedMessage Message)
        {
            byte[] ToSend = Encoding.ASCII.GetBytes(Message._StringedMessage);
            byte[] ToRecieve = new byte[1024];
            try
            {
                ClientSocket.Send(ToSend);
                int bytesRec = ClientSocket.Receive(ToRecieve);
                return Encoding.ASCII.GetString(ToRecieve);
            }
            catch(Exception e) { return null; }
        }
    }

    public class ClientRecievedMessage
    {
        public string[] _Values { get; }
        public string _MessageCode { get; }
        public string _StringedMessage { get; set; }

        public ClientRecievedMessage(string Code)
        {
            _MessageCode = Code;
            _Values = new string[0];
        }

        public ClientRecievedMessage(string Code, string[] Values)
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
        public const string JoinRoomSuccess = "1100";
        public const string JoinRoomFull = "1101";
        public const string JoinRoomFail = "1102";
        #endregion
        #region CreateRoom
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
}

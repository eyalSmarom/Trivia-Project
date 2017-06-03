#include "TriviaServer.h"
#include "Validator.h"
#include "Game.h"

int TriviaServer::_roomIdSequence = 0;

TriviaServer::TriviaServer()
{
	DataBase* tempDB = new DataBase();
	_db = *tempDB;
}

TriviaServer::~TriviaServer()
{

}

#pragma region Basic Connection


void TriviaServer::server()
{
	bindAndListen();
}

void TriviaServer::bindAndListen()
{
	try
	{
		int port = 8820;
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_socket == INVALID_SOCKET)
		{
			throw std::exception(__FUNCTION__ " - socket");
		}

		struct sockaddr_in sa = { 0 };

		sa.sin_port = htons(port); // port that server will listen for
		sa.sin_family = AF_INET;   // must be AF_INET
		sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

											// again stepping out to the global namespace
											// Connects between the socket and the configuration (port and etc..)
		if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - bind");

		// Start listening for incoming requests of clients
		if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - listen");
		cout << "Listening on port " << port << endl;

		while (true)
		{
			// the main thread is only accepting clients 
			// and add then to the list of handlers
			cout << "Waiting for client connection request" << endl;
			accept();
		}
	}
	catch (...)
	{
		cout << "Can't create socket.." << endl;
		exit(1);
	}
}

void TriviaServer::accept()
{
	try
	{
		while (true)
		{
			// this accepts the client and create a specific socket from server to this client
			SOCKET client_socket = ::accept(_socket, NULL, NULL);

			if (client_socket == INVALID_SOCKET)
				throw std::exception(__FUNCTION__);

			cout << "Client accepted. Server and client can speak" << endl;

			// Every time when a client has been accepted open a thread and detach it from the main program.
			thread newThread(&TriviaServer::clientHandler, this, client_socket);
			newThread.detach();
		}
	}
	catch (...)
	{
		cout << "Can't connect to client..." << endl;
		exit(1);
	}
}
#pragma endregion

void TriviaServer::clientHandler(SOCKET socket)
{
	int code;
	code = Helper::getMessageTypeCode(socket);
	while (code != 0)
	{
		try
		{
			handleRecievedMessage(buildRecieveMessage(socket, code));
			code = Helper::getMessageTypeCode(socket);
		}
		catch (exception& e) { cout << e.what() << endl; }
	}
}

void TriviaServer::safeDeleteUser(RecievedMessage* message)
{
	try
	{
		SOCKET clientSocket = message->getSock();
		handleSignout(message);
		closesocket(clientSocket);
	}
	catch (exception& e) { cout << e.what() << endl; }
}

#pragma region Handles


User* TriviaServer::handleSignIn(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();

	User* user = nullptr;
	vector<string> parameters = message->getValues();
	string username = parameters[0];
	string password = parameters[1];
	string Message = to_string(Sign_In_Response);

	if (_db.isUserAndPassMatch(username, password))
		if (getUserByName(username) == nullptr) // if the user isn't connected
		{
			try
			{
				user = new User(username, clientSocket);
				mtxUsers.lock();
				_connectedUsers.insert(pair<SOCKET, User*>(clientSocket, user));
				mtxUsers.unlock();
				Message += to_string(Sign_In_Success);
			}
			catch (...) { Message += to_string(Sign_In_Fail); }
		}
		else
			Message += to_string(Sign_In_Already_Connected);
	else
		Message += to_string(Sign_In_Wrong_Details);

	Helper::sendData(clientSocket, Message);

	return user;
}

bool TriviaServer::handleSignUp(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	string username = message->getValues()[0];
	string password = message->getValues()[1];
	string email = message->getValues()[2];

	string Message = to_string(Sign_Up_Response);

	if (!Validator::isUsernameValid(username))
	{
		Message += to_string(Sign_Up_Username_Illegal);
		Helper::sendData(clientSocket, Message);
		return false;
	}

	if (!Validator::isPasswordValid(password))
	{
		Message += to_string(Sign_Up_Pass_Illegal);
		Helper::sendData(clientSocket, Message);
		return false;
	}

	if (_db.isUserExists(username))
	{
		Message += to_string(Sign_Up_Username_Exists);
		Helper::sendData(clientSocket, Message);
		return false;
	}

	if (!_db.addNewUser(username, password, email))
	{
		Message += to_string(Sign_Up_Other);
		Helper::sendData(clientSocket, Message);
		return false;
	}
	else
	{
		Message += to_string(Sign_Up_Success);
		Helper::sendData(clientSocket, Message);
		return true;
	}
}

void TriviaServer::handleSignout(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock(); // getting the client's socket
	User* user;

	user = getUserBySocket(clientSocket); // trying to find the user.

	if (user != nullptr) // if the user has been found.
		_connectedUsers.erase(clientSocket);
}

void TriviaServer::handleLeaveGame(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	
	User* user = getUserBySocket(clientSocket);

	if (user != nullptr)
		if (user->leaveGame())
			user->setGame(nullptr);
}

void TriviaServer::handleStartGame(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	int roomId = user->getRoom()->getId(); // get the roomId

	try
	{
		Game* newGame = new Game(user->getRoom()->getUsers(), user->getRoom()->getQuestionNo(), _db); // create a new game.
		_roomsList.erase(roomId); // erase the room
		newGame->sendFirstQuestion(); // send the first question to the players.
	}
	catch (...) {}
}

void TriviaServer::handlePlayerAnswer(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	string answerNo = message->getValues()[0];
	string answerTime = message->getValues()[1];

	if (user != nullptr)
	{
		if (user->getGame() != nullptr)
		{
			if (!user->getGame()->handleAnswerFromUser(user, stoi(answerNo, 0, 10), stoi(answerTime, 0, 10)))
				user->setGame(nullptr);
		}
	}
}

bool TriviaServer::handleCreateRoom(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	vector<string> values = message->getValues();
	string Message = to_string(Create_Room_Response);
	bool ans = false;

	if (user != nullptr)
	{
		_roomIdSequence++;
		if (user->createRoom(_roomIdSequence, values[0], stoi(values[1], 0, 10), stoi(values[2], 0, 10), stoi(values[3], 0, 10)))
		{
			_roomsList.insert(pair<int, Room*>(_roomIdSequence, user->getRoom()));
			Message += to_string(Create_Room_Success);
			ans = true;
		}
		else
			Message += to_string(Create_Room_Fail);
	}
	else
		Message += to_string(Create_Room_Fail);
	Helper::sendData(clientSocket, Message);
	return ans;
}

bool TriviaServer::handleCloseRoom(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	string Message = to_string(Close_Room_Response);
	bool ans = false;
	int roomId;

	if (user != nullptr)
	{
		if (user->getUsername() == user->getRoom()->getAdmin()->getUsername())
		{
			roomId = user->getRoom()->getId();
			user->closeRoom();
			_roomsList.erase(roomId);
			ans = true;
		}
		else
			ans = false;
	}
	Helper::sendData(clientSocket, Message);
	return ans;
}

bool TriviaServer::handleJoinRoom(RecievedMessage * message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	int roomId = stoi(message->getValues()[0], 0, 10);
	string Message = to_string(Join_Room_Response);

	if (user != nullptr)
	{
		Room* room = getRoomById(roomId);
		if (room == nullptr) {
			Message += to_string(Join_Room_Fail_NotExist);
			return false;
		}
		user->joinRoom(room);
		Message += to_string(Join_Room_Success);
	}

	Helper::sendData(clientSocket, Message);
	return false;
}

bool TriviaServer::handleLeaveRoom(RecievedMessage * message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);

	if (user != nullptr)
	{
		Room* room = user->getRoom();
		if (room != nullptr)
		{
			user->leaveRoom();
			return true;
		}
	}
	return false;
}

void TriviaServer::handleGetUsersInRoom(RecievedMessage* message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	int roomId = stoi(message->getValues()[0], 0, 10);
	Room* room = getRoomById(roomId);
	string Message = to_string(All_Rooms_Users_Response);

	if (room != nullptr)
	{
		Message += room->getUsersListMessage();
		Helper::sendData(clientSocket, Message);
	}

	Message += to_string(0);
	Helper::sendData(clientSocket, Message);
}

void TriviaServer::handleGetRooms(RecievedMessage * message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	string Message = to_string(All_Rooms_List_Request);
	Message += Helper::getPaddedNumber(_roomsList.size(), 4);
	Room* temp;

	for (map<int, Room*>::iterator it = _roomsList.begin(); it != _roomsList.end(); it++)
	{
		temp = it->second;
		Message += Helper::getPaddedNumber(temp->getId(), 4);
		Message += Helper::getPaddedNumber(temp->getName().size(), 2);
		Message += temp->getName();
	}

	Helper::sendData(clientSocket, Message);
}

void TriviaServer::handleGetPersonalStatus(RecievedMessage * message)
{
	/*To Do ...*/
}

void TriviaServer::handleGetBestScores(RecievedMessage * message)
{
	SOCKET clientSocket = message->getSock();
	User* user = getUserBySocket(clientSocket);
	string Message = to_string(Best_Scores_Response);
	// ToDo...
}

void TriviaServer::handleRecievedMessage(RecievedMessage* message)
{
	int messageCode = message->GetMessageCode();

	try 
	{
		switch (messageCode) // calling the right function base on the Message Code.
		{
		case Sign_In_Request:
			handleSignIn(message);
			break;

		case Sign_Out_Request:
			handleSignout(message);
			break;

		case Sign_Up_Request:
			handleSignUp(message);
			break;

		case Leave_Game_Request:
			handleLeaveGame(message);
			break;

		case Start_Game_Request:
			handleStartGame(message);
			break;

		case Client_Answer:
			handlePlayerAnswer(message);
			break;

		case Create_Room_Request:
			handleCreateRoom(message);
			break;

		case Close_Room_Request:
			handleCloseRoom(message);
			break;

		case Join_Room_Request:
			handleJoinRoom(message);
			break;

		case Leave_Room_Request:
			handleLeaveRoom(message);
			break;

		case All_Room_Users_Request:
			handleGetUsersInRoom(message);
			break;

		case All_Rooms_List_Request:
			handleGetRooms(message);
			break;

		case Best_Scores_Request:
			handleGetBestScores(message);
			break;

		case Personal_State_Request:
			handleGetPersonalStatus(message);
			break;

		default:
			safeDeleteUser(message);
			break;
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		safeDeleteUser(message);
	}
}
#pragma endregion

RecievedMessage* TriviaServer::buildRecieveMessage(SOCKET socket, int num)
{
	int bytes;
	vector<string> parameters;

	switch (num)
	{
		//Requests that does not need any vector of parameters.
		case Sign_Out_Request: case All_Rooms_List_Request: case Leave_Room_Request:
		case Close_Room_Request: case Leave_Game_Request: case Best_Scores_Request:
		case Personal_State_Request: case Leave_App_Request: case Start_Game_Request:
			return new RecievedMessage(socket, num);
			break;
			
		/* Requests that needs vector of parameters. */
		case Sign_In_Request:
			for(int i = 0; i < 2; i++)
			{
				bytes = Helper::getIntPartFromSocket(socket, Two_Bytes_Int_Num);
				parameters.push_back(string(Helper::getStringPartFromSocket(socket, bytes))); // name/password/email
			}
			return new RecievedMessage(socket, num, parameters);
			break;

		case Sign_Up_Request:
			for (int i = 0; i < 3; i++)
			{
				bytes = Helper::getIntPartFromSocket(socket, Two_Bytes_Int_Num);
				parameters.push_back(string(Helper::getStringPartFromSocket(socket, bytes))); // name/password/email
			}
			return new RecievedMessage(socket, num, parameters);
			break;

		case All_Room_Users_Request: case Join_Room_Request:
			bytes = 4;
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, bytes))); // roomId
			return new RecievedMessage(socket, num, parameters);
			break;

		case Create_Room_Request:
			bytes = Helper::getIntPartFromSocket(socket, Two_Bytes_Int_Num);
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, bytes))); // Room Name
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, One_Byte_Int_Num))); // Number of Players
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, Two_Bytes_Int_Num))); // Number of Questions
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, Two_Bytes_Int_Num))); // Answer time
			return new RecievedMessage(socket, num, parameters);
			break;

		case Client_Answer:
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, One_Byte_Int_Num))); // Answer Number
			parameters.push_back(string(Helper::getStringPartFromSocket(socket, Two_Bytes_Int_Num))); // Answer Time
			return new RecievedMessage(socket, num, parameters);
			break;
			
		default:
			return nullptr;
			break;
	}
}

#pragma region Getters

User* TriviaServer::getUserByName(string name)
{
	for (map<SOCKET, User*>::iterator it = _connectedUsers.begin(); it != _connectedUsers.end(); it++) // seeks the user with the same name given.
	{
		if (it->second->getUsername() == name)
			return it->second;
	}

	return nullptr;
}

User* TriviaServer::getUserBySocket(SOCKET socket)
{
	for (map<SOCKET, User*>::iterator it = _connectedUsers.begin(); it != _connectedUsers.end(); it++) // seeks the user with the same socket given.
	{
		if (it->first == socket)
			return it->second;
	}

	return nullptr;
}

Room * TriviaServer::getRoomById(int id)
{
	for (map<int, Room*>::iterator it = _roomsList.begin(); it != _roomsList.end(); it++) // seeks for the room with the same id given.
	{
		if (it->second->getId() == id)
			return it->second;
	}

	return nullptr;
}
#pragma endregion
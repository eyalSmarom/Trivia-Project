#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <map>
#include <queue>
#include <mutex>
#include "User.h"
#include "Room.h"
#include "sqlite3.h"
#include "Database.h"
#include "ReceivedMessage.h"
#include "WSAInitializer.h"
#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class TriviaServer
{
	public:
		//Functions
		TriviaServer();
		~TriviaServer();

		void server();
	private:
		//Functions
		void bindAndListen();
		void accept();
		void clientHandler(SOCKET socket);
		void safeDeleteUser(ReceivedMessage* message);

		User* handleSignIn(ReceivedMessage* message);
		bool handleSignUp(ReceivedMessage* message);
		void handleForgotPassword(ReceivedMessage* message);
		void handleSignout(ReceivedMessage* message);

		void handleLeaveGame(ReceivedMessage* message);
		void handleStartGame(ReceivedMessage* message);
		void handlePlayerAnswer(ReceivedMessage* message);

		bool handleCreateRoom(ReceivedMessage* message);
		bool handleCloseRoom(ReceivedMessage* message);
		bool handleJoinRoom(ReceivedMessage* message);
		bool handleLeaveRoom(ReceivedMessage* message);
		void handleGetUsersInRoom(ReceivedMessage* message);
		void handleGetRooms(ReceivedMessage* message);
		void handleGetPersonalStatus(ReceivedMessage* message);

		void handleGetBestScores(ReceivedMessage* message);
		void handleReceivedMessage(ReceivedMessage* message);
		ReceivedMessage* buildRecieveMessage(SOCKET socket, int num);

		User* getUserByName(string name);
		User* getUserBySocket(SOCKET socket);
		Room* getRoomById(int id);

		//Variables
		map<SOCKET, User*> _connectedUsers;
		map<int, Room*> _roomsList;
		queue<ReceivedMessage*> _queRcvMessages;
		static int _roomIdSequence;
		SOCKET _socket;
		DataBase _db;
		mutex mtxUsers;
};
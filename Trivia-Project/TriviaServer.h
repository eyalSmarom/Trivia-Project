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
		void safeDeleteUser(RecievedMessage* message);

		User* handleSignIn(RecievedMessage* message);
		bool handleSignUp(RecievedMessage* message);
		void handleSignout(RecievedMessage* message);

		void handleLeaveGame(RecievedMessage* message);
		void handleStartGame(RecievedMessage* message);
		void handlePlayerAnswer(RecievedMessage* message);

		bool handleCreateRoom(RecievedMessage* message);
		bool handleCloseRoom(RecievedMessage* message);
		bool handleJoinRoom(RecievedMessage* message);
		bool handleLeaveRoom(RecievedMessage* message);
		void handleGetUsersInRoom(RecievedMessage* message);
		void handleGetRooms(RecievedMessage* message);
		void handleGetPersonalStatus(RecievedMessage* message);

		void handleGetBestScores(RecievedMessage* message);
		void handleRecievedMessage(RecievedMessage* message);
		RecievedMessage* buildRecieveMessage(SOCKET socket, int num);

		User* getUserByName(string name);
		User* getUserBySocket(SOCKET socket);
		Room* getRoomById(int id);

		//Variables
		map<SOCKET, User*> _connectedUsers;
		map<int, Room*> _roomsList;
		queue<RecievedMessage*> _queRcvMessages;
		static int _roomIdSequence;
		SOCKET _socket;
		DataBase _db;
		mutex mtxUsers;
};
#pragma once

#include "Helper.h"

using namespace std;

class Game;
class Room;
class DataBase;

class User
{
public:
	User(string, SOCKET);
	User(string, SOCKET, Room*, Game*);
	void send(string);
	string getUsername();
	SOCKET getSocket();
	Room* getRoom();
	Game* getGame();
	void setGame(Game*);
	void clearRoom();
	bool createRoom(int roomId, string roomName, int maxUsers, int questionNo, int questionTime, DataBase& db);
	bool joinRoom(Room*);
	void leaveRoom();
	int closeRoom();
	bool leaveGame();
	~User();

private:
	string _username;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;
};
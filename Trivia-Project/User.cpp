#include "User.h"
#include "Room.h"
#include "Game.h"

User::User(string uName, SOCKET sock) : _username(uName), _sock(sock)
{
	this->_currGame = nullptr;
	this->_currRoom = nullptr;
}

User::User(string uName, SOCKET sock, Room* room, Game* game) : _username(uName), _sock(sock)
{
	this->_currRoom = new Room(*room);
	this->_currGame = new Game(*game);
}

User::~User()
{
}

void User::send(string message)
{
	Helper::sendData(this->_sock, message);
}

string User::getUsername()
{
	return this->_username;
}

SOCKET User::getSocket()
{
	return this->_sock;
}

Room* User::getRoom()
{
	return this->_currRoom;
}

Game* User::getGame()
{
	return this->_currGame;
}

void User::setGame(Game* gm)
{
	this->_currRoom = nullptr;
	this->_currGame = new Game(*gm);
}

void User::clearRoom()
{
	this->_currRoom = nullptr;
}

bool User::createRoom(int roomId, string roomName, int maxUsers, int questionsNo, int questionTime)
{
	if (this->_currRoom != nullptr)
		return false;
	this->_currRoom = new Room(roomId, this, roomName, maxUsers, questionsNo, questionTime);
	return true;
}

bool User::joinRoom(Room* newRoom)
{
	if (this->_currGame != nullptr)
		return false;
	newRoom->joinRoom(this);
	return true;
}

void User::leaveRoom()
{
	if (this->_currRoom != nullptr)
	{
		this->clearRoom();
		this->leaveRoom();
	}
}

int User::closeRoom()
{
	if (this->_currRoom == nullptr)
		return -1;
	if (!this->_currRoom->getAdmin()->getUsername().compare(this->_username))
		return this->_currRoom->closeRoom(this);
	delete this->_currRoom;
	this->_currRoom = nullptr;

}

bool User::leaveGame()
{
	if (this->_currGame != nullptr)
	{
		if (this->_currGame->leaveGame(this))
		{
			return true;
		}
		return false;
	}

}
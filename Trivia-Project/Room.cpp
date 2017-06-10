#include "Room.h"
#include "User.h"

Room::Room(int id, User* admin, string name, int maxUsers, int questionsNo, int questionTime) : _id(id), _admin(admin), _name(name), _maxUsers(maxUsers), _questionNo(questionsNo), _questionTime(questionTime)
{
	_users.push_back(admin);
}

bool Room::joinRoom(User* user)
{
	this->_users.push_back(user);
	this->sendMessage(this->getUsersListMessage());
	return true;
}

void Room::leaveRoom(User* user)
{
	vector<User*>::iterator locatedUser = find(this->_users.begin(), this->_users.end(), user);
	if (locatedUser != this->_users.end())//search if user presents in vector
	{
		this->_users.erase(locatedUser);
		this->sendMessage(this->getUsersListMessage());
	}
}

int Room::closeRoom(User* user)
{
	if (user != this->_admin)
		return -1;
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); ++it)
	{
		(*it)->send(to_string(Close_Room_Response));
		(*it)->clearRoom();
	}
	return _id;
}

vector<User*>& Room::getUsers()
{
	return _users;
}

string Room::getUsersListMessage()
{
	string message = "";
	message += to_string(All_Rooms_Users_Response);
	message += to_string(_users.size());
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); ++it)
	{
		message += Helper::getPaddedNumber((*it)->getUsername().size(), 2);
		message += (*it)->getUsername();
	}
	return message;
}

int Room::getQuestionNo()
{
	return _questionNo;
}

int Room::getId()
{
	return _id;
}

string Room::getName()
{
	return string(_name);
}

User * Room::getAdmin()
{
	return _admin;
}

Room::~Room()
{
	
}

void Room::sendMessage(string message)
{
	sendMessage(nullptr, message);
}

void Room::sendMessage(User* excludeUser, string message)
{
	for (vector<User*>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if ((*it) == excludeUser)
			continue;
		(*it)->send(message);
	}
}

int Room::getQuestionTime()
{
	return _questionTime;
}

int Room::getMaxUsers()
{
	return _maxUsers;
}
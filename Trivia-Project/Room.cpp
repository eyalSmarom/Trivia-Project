#include "Room.h"
#include "User.h"

Room::Room(int id, User* admin, string name, int maxUsers, int questionsNo, int questionTime) : _id(id), _admin(admin), _name(name), _maxUsers(maxUsers), _questionNo(questionsNo), _questionTime(questionTime)
{
	_users.push_back(admin);
}

bool Room::joinRoom(User* user)
{
	if (this->_users.size() > 1)
		return false;
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
		if ((*it) == _admin)
			continue;
		(*it)->send("Closing Room");
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
	string message = "108";
	message += to_string(_users.size());
	for (vector<User*>::iterator it = this->_users.begin(); it != this->_users.end(); ++it)
	{
		message += (*it)->getUsername().size();
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
	for (vector<User*>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		delete (*it);
	}
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

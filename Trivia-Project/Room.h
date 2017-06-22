#pragma once

#include "Helper.h"

class User;
class Question;
class DataBase;

class Room
{
public:
	Room(int id, User* admin, string name, int maxUsers, int questionsNo, int questionTime, DataBase& db);
	bool joinRoom(User* user);
	void leaveRoom(User* user);
	int closeRoom(User* user);
	vector<User*>& getUsers();
	string getUsersListMessage();
	int getQuestionNo();
	int getQuestionTime();
	int getId();
	int getMaxUsers();
	vector<Question*> getQuestions();
	string getName();
	User* getAdmin();
	~Room();

private:
	vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionNo;
	string _name;
	int _id;
	vector<Question*> _questions;

	void sendMessage(string message);
	void sendMessage(User* excludeUser, string message);
};
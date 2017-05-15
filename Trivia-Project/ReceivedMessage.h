#pragma once

#include "Helper.h"
#include "User.h"

using namespace std;
class RecievedMessage
{
public:
	RecievedMessage(SOCKET, int);
	RecievedMessage(SOCKET, int, vector<string>);
	SOCKET getSock();
	User* getUser();
	void setUser(User*);
	int GetMessageCode();
	vector<string>& getValues();
	~RecievedMessage();

private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	vector<string> _values;
};
#pragma once

#include "Helper.h"
#include "User.h"

using namespace std;
class ReceivedMessage
{
public:
	ReceivedMessage(SOCKET, int);
	ReceivedMessage(SOCKET, int, vector<string>);
	SOCKET getSock();
	User* getUser();
	void setUser(User*);
	int GetMessageCode();
	vector<string>& getValues();
	~ReceivedMessage();

private:
	SOCKET _sock;
	User* _user;
	int _messageCode;
	vector<string> _values;
};
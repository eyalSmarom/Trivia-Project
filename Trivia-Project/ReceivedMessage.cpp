#include "ReceivedMessage.h"

RecievedMessage::RecievedMessage(SOCKET soc, int mCode) : _sock(soc), _messageCode(mCode)
{
}

RecievedMessage::RecievedMessage(SOCKET soc, int mCode, vector<string> values) : _sock(soc), _messageCode(mCode), _values(values)
{
}

RecievedMessage::~RecievedMessage()
{
}

SOCKET RecievedMessage::getSock()
{
	return this->_sock;
}

User* RecievedMessage::getUser()
{
	return this->_user;
}

void RecievedMessage::setUser(User* newUser)
{
	delete this->_user;
	this->_user = new User(*newUser);
}

int RecievedMessage::GetMessageCode()
{
	return this->_messageCode;
}

vector<string>& RecievedMessage::getValues()
{
	return this->_values;
}


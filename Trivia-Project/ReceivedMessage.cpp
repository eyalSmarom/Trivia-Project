#include "ReceivedMessage.h"

ReceivedMessage::ReceivedMessage(SOCKET soc, int mCode) : _sock(soc), _messageCode(mCode)
{
}

ReceivedMessage::ReceivedMessage(SOCKET soc, int mCode, vector<string> values) : _sock(soc), _messageCode(mCode), _values(values)
{
}

ReceivedMessage::~ReceivedMessage()
{
}

SOCKET ReceivedMessage::getSock()
{
	return this->_sock;
}

User* ReceivedMessage::getUser()
{
	return this->_user;
}

void ReceivedMessage::setUser(User* newUser)
{
	delete this->_user;
	this->_user = new User(*newUser);
}

int ReceivedMessage::GetMessageCode()
{
	return this->_messageCode;
}

vector<string>& ReceivedMessage::getValues()
{
	return this->_values;
}


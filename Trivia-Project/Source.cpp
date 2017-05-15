#include "TriviaServer.h"

int main()
{
	try
	{
		WSAInitializer wsaInit;
		TriviaServer server;
		server.server();
	}
	catch(...)
	{
		exit(1);
	}
}
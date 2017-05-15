#include "TriviaServer.h"

TriviaServer::TriviaServer()
{
	DataBase* tempDb = new DataBase();
	_db = *tempDb;
}

TriviaServer::~TriviaServer()
{

}

void TriviaServer::server()
{
	bindAndListen();
}

void TriviaServer::bindAndListen()
{
	try
	{
		int port = 8860;
		_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (_socket == INVALID_SOCKET)
		{
			throw std::exception(__FUNCTION__ " - socket");
		}

		struct sockaddr_in sa = { 0 };

		sa.sin_port = htons(port); // port that server will listen for
		sa.sin_family = AF_INET;   // must be AF_INET
		sa.sin_addr.s_addr = INADDR_ANY;    // when there are few ip's for the machine. We will use always "INADDR_ANY"

											// again stepping out to the global namespace
											// Connects between the socket and the configuration (port and etc..)
		if (::bind(_socket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - bind");

		// Start listening for incoming requests of clients
		if (::listen(_socket, SOMAXCONN) == SOCKET_ERROR)
			throw std::exception(__FUNCTION__ " - listen");
		cout << "Listening on port " << port << endl;

		while (true)
		{
			// the main thread is only accepting clients 
			// and add then to the list of handlers
			cout << "Waiting for client connection request" << endl;
			accept();
		}
	}
	catch (...)
	{
		cout << "Can't create socket.." << endl;
		exit(1);
	}
}

void TriviaServer::accept()
{
	try
	{
		while (true)
		{
			// this accepts the client and create a specific socket from server to this client
			SOCKET client_socket = ::accept(_socket, NULL, NULL);

			if (client_socket == INVALID_SOCKET)
				throw std::exception(__FUNCTION__);

			cout << "Client accepted. Server and client can speak" << endl;

			// Every time when a client has been accepted open a thread and detach it from the main program.
			thread newThread(&TriviaServer::clientHandler, this, client_socket);
			newThread.detach();
		}
	}
	catch (...)
	{
		cout << "Can't connect to client..." << endl;
		exit(1);
	}
}

void TriviaServer::clientHandler(SOCKET socket)
{

}

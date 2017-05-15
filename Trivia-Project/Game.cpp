#include "Game.h"
#include "Room.h"
#include "User.h"
#include "Database.h"

Game::Game(const vector<User*>& players, int questionNo, DataBase& db)
{
	try
	{
		_db = DataBase(db);
		
		// inserting the new game into the database.
		if (_db.insertNewGame() == -1)
			throw exception("Couldn't insert game into the database");
		else
			_db.initQuestions(questionNo);

		// initializing the players
		_players = players;

		// initializing the results
		_results = map<string, int>();

		// initializing for every player his current game
		for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
		{
			(*it)->setGame(this);
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

Game::~Game()
{
	// deleting all the question
	for (vector<Question*>::iterator it = _questions.begin(); it != _questions.end(); it++)
	{
		delete (*it);
	}

	// deleting all the players
	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		delete (*it);
	}
}

void Game::sendFirstQuestion()
{

}

bool Game::leaveGame(User * user)
{
	return false;
}

void Game::sendQuestionToAllUsers()
{

}

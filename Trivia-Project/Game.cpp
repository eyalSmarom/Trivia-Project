#include "Game.h"
#include "Room.h"
#include "User.h"
#include "Database.h"
#include "Question.h"

Game::Game(const vector<User*>& players, int questionNo, DataBase& db)
{
	try
	{
		_db = DataBase(db);
		
		_gameId = _db.insertNewGame();

		// inserting the new game into the database.
		if (_gameId == -1)
			throw exception("Couldn't insert game into the database");
		else
			_db.initQuestions(questionNo);

		// initiate the players
		_players = players;

		// initiate the results
		_results = map<string, int>();

		// initiate the current question index
		_currQuestionIndex = 0;

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
	sendQuestionToAllUsers();
}

void Game::handleFinishGame()
{
	string message = "";
	_db.updateGameStatus(_gameId);
	
	message += End_Game_Response; // Message Code.
	message += _players.size(); // Number of users

	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		try
		{
			message += (*it)->getUsername().size();
			message += (*it)->getUsername();
			message += _results.find((*it)->getUsername())->second;
		}
		catch(...){}
	}
}

bool Game::handleNextTurn()
{
	if (_players.size() == 0) // If there isn't any player in the game, end the game.
	{
		handleFinishGame();
		return false;
	}

	if (_players.size() == _currentTurnAnswers) // If everyone answered
	{
		if (_questionNo = _currQuestionIndex - 1) // If it is the last turn
		{
			handleFinishGame();
			return false;
		}
		else // If it isn't the last turn
		{
			_currQuestionIndex++;
			sendQuestionToAllUsers();
			return true;
		}
	}
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	Question* currQuestion = _questions[_currQuestionIndex];

	_currentTurnAnswers++;

	if (answerNo == currQuestion->getCorrectAnswerIndex()) // if the answer is correct, raise the correct answer count in the results map.
	{
		_results.find(user->getUsername())->second += 1;
		_db.addAnswerToPlayer(_gameId, user->getUsername(), _currQuestionIndex, currQuestion->getAnswers()[answerNo], true, time); // Inserting the db record (true answer)
	}

	_db.addAnswerToPlayer(_gameId, user->getUsername(), _currQuestionIndex, currQuestion->getAnswers()[answerNo], false, time); // Inserting the db record (false answer)

	if (handleNextTurn()) // Play Next Turn
		return true;
	return false;
}

bool Game::leaveGame(User* user)
{
	vector<User*>::iterator toRemove = find(_players.begin(), _players.end(), user);

	if (toRemove != _players.end()) // if the player has been found.
	{
		_players.erase(toRemove);
		if (handleNextTurn())
			return true;
		return false;
	}

	return false; // if the player hasn't been found return false.
}

int Game::getID()
{
	return _gameId;
}

void Game::sendQuestionToAllUsers()
{
	string message = "";
	string* answers;
	Question* question = _questions[_currQuestionIndex];

	message += Send_Question_With_Answers; // Message Code

	message += question->getQuestion().size(); // Question size
	message += question->getQuestion(); // Question
	
	// Possible Answers
	answers = question->getAnswers();
	for (int i = 0; i < 4; i++)
	{
		message += answers[i].size();
		message += answers[i];
	}


	// Send the message to all the users
	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		try { (*it)->send(message); } catch (...) {}
	}
}
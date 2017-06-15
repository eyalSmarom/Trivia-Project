#include "Game.h"
#include "Room.h"
#include "User.h"
#include "Database.h"
#include "Question.h"

int Game::_currentTurnAnswers = 0;

Game::Game(const vector<User*>& players, int questionNo, DataBase& db)
{
	try
	{
		DataBase* temp = new DataBase(db);
		_db = *temp;
		
		_currentTurnAnswers = 0;

		_questionNo = questionNo;
		_gameId = _db.insertNewGame();

		// inserting the new game into the database.
		if (_gameId == -1)
			throw exception("Couldn't insert game into the database");
		else
			_questions = _db.initQuestions(questionNo);

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

Game::Game(Game& other)
{
	_gameId = other._gameId;
	_db = other._db;
	_questions = vector<Question*>(other._questions);
	_results = other._results;
	_currQuestionIndex = other._currQuestionIndex;
	_currentTurnAnswers = other._currentTurnAnswers;
	_players = other._players;
	_questionNo = other._questionNo;
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
	
	message += to_string(End_Game_Response); // Message Code.
	message += to_string(_players.size()); // Number of users

	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		try
		{
			int result = _results.find((*it)->getUsername())->second;
			message += Helper::getPaddedNumber((*it)->getUsername().size(), 2);
			message += (*it)->getUsername();
			message += Helper::getPaddedNumber(result, 2);
		}
		catch(...){}
	}

	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		try { (*it)->send(message); }
		catch (exception& e) { cout << e.what() << endl; }
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
		if (_questionNo == _currQuestionIndex + 1) // If it is the last turn
		{
			handleFinishGame();
			return false;
		}
		else // If it isn't the last turn
		{
			_currQuestionIndex++;
			_currentTurnAnswers = 0;
			sendQuestionToAllUsers();
			return true;
		}
	}
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	Question* currQuestion = _questions[_currQuestionIndex];
	string Code = to_string(True_False_Response);
	string message = "0";

	_currentTurnAnswers++;

	if (_results.find(user->getUsername()) == _results.end())
	{
		mtxResult.lock();
		_results.insert(pair<string, int>(user->getUsername(), 0));
		mtxResult.unlock();
	}

	if (answerNo == currQuestion->getCorrectAnswerIndex() + 1) // if the answer is correct, raise the correct answer count in the results map.
	{
		message = "1";
		_results.find(user->getUsername())->second += 1;
		_db.addAnswerToPlayer(_gameId, user->getUsername(), _currQuestionIndex, currQuestion->getAnswers()[answerNo - 1], true, time); // Inserting the db record (true answer)
	}
	else
		_db.addAnswerToPlayer(_gameId, user->getUsername(), currQuestion->getId(), currQuestion->getAnswers()[answerNo - 1], false, time); // Inserting the db record (false answer)

	user->send(Code + message);

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

	message += to_string(Send_Question_With_Answers); // Message Code

	message += Helper::getPaddedNumber(question->getQuestion().size(), 3); // Question size
	message += question->getQuestion(); // Question
	
	// Possible Answers
	answers = question->getAnswers();
	for (int i = 0; i < 4; i++)
	{
		message += Helper::getPaddedNumber(answers[i].size(), 3);
		message += answers[i];
	}


	// Send the message to all the users
	for (vector<User*>::iterator it = _players.begin(); it != _players.end(); it++)
	{
		try { (*it)->send(message); } catch (...) {}
	}
}
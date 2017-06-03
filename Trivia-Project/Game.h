#pragma once

#include "Helper.h"
#include "Database.h"
#include <map>
#include <mutex>

class Room;
class User;
class Question;

class Game
{
public:
	Game(const vector<User*>& players, int questionNo, DataBase& db);
	Game(Game& other);
	~Game();
	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* user);
	int getID();

private:
	mutex mtxResult;
	vector<Question*> _questions;
	vector<User*> _players;
	int _questionNo;
	int _currQuestionIndex;
	int _currentTurnAnswers;
	int _gameId;
	DataBase _db;
	map<string, int> _results;

	void sendQuestionToAllUsers();
};
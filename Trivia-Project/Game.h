#pragma once

#include "Helper.h"
#include "Database.h"
#include <map>

class Room;
class User;
class Question;

class Game
{
public:
	Game(const vector<User*>& players, int questionNo, DataBase& db);
	~Game();
	void sendFirstQuestion();
	void handleFinishGame();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* user);
	int getID();

private:
	vector<Question*> _questions;
	vector<User*> _players;
	int _questionNo;
	int _currQuestionIndex;
	DataBase _db;
	map<string, int> _results;
	int _currentTurnAnswers;

	bool insertGameToDB();
	void initQuestionFromDB();
	void sendQuestionToAllUsers();
};
#pragma once

#include "sqlite3.h"
#include <string>
#include <iostream>
#include <vector>
#include <io.h>

using namespace std;

class Question;

class DataBase
{
public:
	DataBase();
	DataBase(DataBase& other); // Copy c'tor for Game's c'tor
	~DataBase();
	bool isUserExists(string username);
	bool addNewUser(string username, string password, string email);
	bool isUserAndPassMatch(string username, string password);
	vector<Question*> initQuestions(int questionsNo);
	vector<Question*> getBestScores();
	vector<string> getPersonalStatus();
	int insertNewGame();
	bool updateGameStatus(int status);
	bool addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime);

private:
	sqlite3* _db;
	string _filename;
	int _currGameId;

	static int callbackCount(void*, int, char**, char**);
	static int callbackQuestions(void*, int, char**, char**);
	static int callbackBestScores(void*, int, char**, char**);
	static int callbackPersonalStatus(void*, int, char**, char**);
};


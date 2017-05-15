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
	~DataBase();
	bool isUserExists(string);
	bool addNewUser(string, string, string);
	bool isUserAndPassMatch(string, string);
	vector<Question*> initQuestions(int);
	vector<Question*> getBestScores();
	vector<string> getPersonalStatus();
	int insertNewGame();
	bool updateGameStatus(int);
	bool addAnswerToPlayer(int, string, int, string, bool, int);
private:
	sqlite3* _db;
	string _filename;

	static int callbackCount(void*, int, char**, char**);
	static int callbackQuestions(void*, int, char**, char**);
	static int callbackBestScores(void*, int, char**, char**);
	static int callbackPersonalStatus(void*, int, char**, char**);
};


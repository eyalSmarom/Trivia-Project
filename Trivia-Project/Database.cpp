#include "DataBase.h"
#include "Question.h"
#include <sys/stat.h>
#include <algorithm>

#define DB_NAME "TriviaDb.sqlite"
#define DB_PROBLEM "Database Problem"
#define QUESTION_INSERT_PROBLEM "Question Insert Problem"
#define RETRIVING_INFROMATION_ERROR "Error Accured While Retriving Information"
#define INSERTING_INFORMATION_ERROR "Error Accured While Inserting Information"
#pragma warning(disable : 4996)

bool pairCompare(const pair<int, string>& first, const pair<int, string>& second);

DataBase::DataBase() : _filename(DB_NAME), _currGameId(0)
{
	try
	{
		struct stat buffer;
		bool file_exist = stat(_filename.c_str(), &buffer) == 0;
		int res = sqlite3_open(this->_filename.c_str(), &this->_db);
		if (res != SQLITE_OK)
		{
			this->_db = nullptr;
			throw exception("Failed to open DB");
		}
		if (!file_exist)
		{
			char* sqlStatement = "CREATE TABLE t_questions (question_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, question TEXT NOT NULL, correct_ans TEXT NOT NULL, ans2 TEXT NOT NULL, ans3 TEXT NOT NULL, ans4 TEXT NOT NULL);";
			char* errMessage = nullptr;
			res = sqlite3_exec(this->_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Database Problem");
			sqlStatement = "CREATE TABLE t_games(game_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, status INTEGER NOT NULL, start_time DATETIME NOT NULL, end_time DATETIME);";
			res = sqlite3_exec(this->_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Database Problem");
			sqlStatement = "CREATE TABLE t_users(username TEXT PRIMARY KEY NOT NULL, password TEXT NOT NULL, email TEXT);";
			res = sqlite3_exec(this->_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Database Problem");
			sqlStatement = "CREATE TABLE t_players_answers(game_id INTEGER NOT NULL, username TEXT NOT NULL, question_id INTEGER NOT NULL, player_answer TEXT NOT NULL, is_correct INTEGER NOT NULL, answer_time INTEGER NOT NULL, FOREIGN KEY(game_id) REFERENCES t_games(game_id), FOREIGN KEY(username) REFERENCES t_users(username), FOREIGN KEY(question_id) REFERENCES t_questions(question_id));";
			res = sqlite3_exec(this->_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Database Problem");

			// Inserting the questions
			sqlStatement = "INSERT INTO t_questions (question, correct_ans, ans2, ans3, ans4) VALUES ('When did Albert Einstein win a noble prize?', '1921', '1922', '1928', '1926');";
			res = sqlite3_exec(_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Question Insert Problem");

			sqlStatement = "INSERT INTO t_questions (question, correct_ans, ans2, ans3, ans4) VALUES ('When was Albert Einstein Born?', '14.3.1879', '4.6.1878', '3.9.1885', '3.10.1877');";
			res = sqlite3_exec(_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Question Insert Problem");

			sqlStatement = "INSERT INTO t_questions (question, correct_ans, ans2, ans3, ans4) VALUES ('Where was Albert Einstein born?', 'Ulm', 'Hamburg', 'Dresden', 'Berlin');";
			res = sqlite3_exec(_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Question Insert Problem");

			sqlStatement = "INSERT INTO t_questions (question, correct_ans, ans2, ans3, ans4) VALUES ('When did Albert Einstein formulate his special theory of relativity', '1905', '1903', '1900', '1904');";
			res = sqlite3_exec(_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Question Insert Problem");

			sqlStatement = "INSERT INTO t_questions (question, correct_ans, ans2, ans3, ans4) VALUES ('How Many Hearts does an Occtupus have?', '1', '2', '3', '4');";
			res = sqlite3_exec(_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Question Insert Problem");
		}
		char* sqlStatement = "Select * From t_games;";
		sqlite3_stmt* stmt;
		int counter = 0;
		res = sqlite3_prepare_v2(_db, sqlStatement, -1, &stmt, NULL);
		while (1)
		{
			int s;

			s = sqlite3_step(stmt);
			if (s == SQLITE_ROW)
				counter++;
			else
				break;
		}
		this->_currGameId = counter;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;

		/* To Do: Send exit message to all players and exit games because of the glitch
		Until that wouldn't be coded, exit the program for debugging purposes.        */

		exit(1);
	}
}

DataBase::DataBase(DataBase & other)
{
	_currGameId = other._currGameId;
	_filename = string(other._filename);
	_db = other._db;
}

DataBase::~DataBase()
{
	sqlite3_close(this->_db);
	this->_db = nullptr;
}

bool DataBase::isUserExists(string username)
{
	string sqlStatement = "SELECT username FROM t_users WHERE username = '" + username + "';";
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(this->_db, sqlStatement.c_str(), -1, &stmt, NULL) != SQLITE_OK)
		return false;//somthing went wrong
	while (1)
	{
		int s;

		s = sqlite3_step(stmt);//get first row
		if (s == SQLITE_ROW)
		{
			return true;//there is an existing row
		}
		else if (s == SQLITE_DONE)
		{
			break;
		}
		else
		{
			return false;//somthing went wrong
		}
	}
	return false;//didnt find
}

bool DataBase::addNewUser(string username, string password, string email)
{
	string sqlStatement = "INSERT INTO t_users(username, password, email) VALUES ('" + username + "', '" + password + "', '" + email + "');";
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;
	else
		return true;
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	string sqlStatement = "SELECT * FROM t_users WHERE username = '" + username + "' AND Password = '" + password + "';";
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(this->_db, sqlStatement.c_str(), strlen(sqlStatement.c_str()) + 1, &stmt, NULL) != SQLITE_OK)
		return false;//somthing went wrong
	while (1)
	{
		int s;

		s = sqlite3_step(stmt);//get first row
		if (s == SQLITE_ROW)
		{
			return true;//there is an existing row
		}
		else if (s == SQLITE_DONE)
		{
			break;
		}
		else
		{
			return false;//somthing went wrong
		}
	}
	return false;//didnt find
}

vector<Question*> DataBase::initQuestions(int questionsNo)
{
	vector<Question*> questionsVector;
	srand(time(NULL));
	try
	{
		int i = 1;
		string sqlStatement = "SELECT * FROM t_questions";
		sqlite3_stmt *stmt;
		if (sqlite3_prepare_v2(this->_db, sqlStatement.c_str(), strlen(sqlStatement.c_str()) + 1, &stmt, NULL) != SQLITE_OK)
			throw exception(RETRIVING_INFROMATION_ERROR);
		while (1)
		{
			int s;

			s = sqlite3_step(stmt);//get first row
			if (s == SQLITE_ROW)
			{
				string question = (char*)sqlite3_column_text(stmt, 1);
				string correctAns = (char*)sqlite3_column_text(stmt, 2);
				string ans2 = (char*)sqlite3_column_text(stmt, 3);
				string ans3 = (char*)sqlite3_column_text(stmt, 4);
				string ans4 = (char*)sqlite3_column_text(stmt, 5);
				Question* newQuestion = new Question(i, question, correctAns, ans2, ans3, ans4);//not sure if i sould free this memory later, also need to check if push_back shellow copy or not
				questionsVector.push_back(newQuestion);
				i++;
			}
			else if (s == SQLITE_DONE)
			{
				break;
			}
			else
			{
				sqlite3_finalize(stmt);
				throw exception(RETRIVING_INFROMATION_ERROR);
			}
		}
		int vSize = questionsVector.size();
		while (vSize > questionsNo)//randomize to get random questions from data base
		{
			int randIndex = rand() % vSize;
			questionsVector.erase(questionsVector.begin() + randIndex);
			vSize--;
		}
		sqlite3_finalize(stmt);
		return questionsVector;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return questionsVector;
	}
}

vector<pair<int, string>> DataBase::getBestScores()
{
	vector<pair<int, string>> bestScores;
	try
	{
		string sqlStatement = "SELECT username, count(is_correct) AS correctAns FROM t_players_answers GROUP BY username ORDER BY correctAns LIMIT 3;";//not sure if working - need to get the top 3 best players with the most correct answers 
		sqlite3_stmt *stmt;
		if (sqlite3_prepare_v2(this->_db, sqlStatement.c_str(), strlen(sqlStatement.c_str()) + 1, &stmt, NULL) != SQLITE_OK)
			throw exception(RETRIVING_INFROMATION_ERROR);//somthing went wrong
		while (1)
		{
			int s;

			s = sqlite3_step(stmt);//get first row
			if (s == SQLITE_ROW)
			{
				string username = (char*)sqlite3_column_text(stmt, 0);//not sure if its working
				string correctAnswers = (char*)sqlite3_column_text(stmt, 1);
				int correctAnswersCount = atoi(correctAnswers.c_str());
				pair<int, string> userScore(correctAnswersCount, username);//its in map because its easier to work with
				bestScores.push_back(userScore);
			}
			else if (s == SQLITE_DONE)
			{
				break;
			}
			else
			{
				sqlite3_finalize(stmt);
				throw exception(RETRIVING_INFROMATION_ERROR);
			}
		}
		sort(bestScores.begin(), bestScores.end(), pairCompare);
		sqlite3_finalize(stmt);
		return bestScores;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return bestScores;
	}
}

/// Function to compare between the pairs of the vector. for the sort function.
bool pairCompare(const pair<int, string>& first, const pair<int, string>& second)
{
	return first.first > second.first;
}

vector<string> DataBase::getPersonalStatus(string username)
{
	vector<string> personalStatus;
	try
	{
		string sqlStatement = "SELECT SUM(CASE WHEN is_correct IS NOT 0 THEN 1 ELSE 0 END) AS column1_count, MAX(game_id) AS column2_count, SUM(CASE WHEN question_id IS NOT NULL THEN 1 ELSE 0 END) AS column3_count, AVG(answer_time) AS column4_count FROM t_players_answers WHERE username = '" + username + "';";
		sqlite3_stmt *stmt;
		if (sqlite3_prepare_v2(this->_db, sqlStatement.c_str(), strlen(sqlStatement.c_str()) + 1, &stmt, NULL) != SQLITE_OK)
			throw exception(RETRIVING_INFROMATION_ERROR);
		while (1)
		{
			int s;

			s = sqlite3_step(stmt);//get first row
			if (s == SQLITE_ROW)
			{
				string correctAnswers = (char*)sqlite3_column_text(stmt, 0);
				int correctAnswersCount = atoi(correctAnswers.c_str());
				string gamesCount = (char*)sqlite3_column_text(stmt, 1);
				string questionsCount = (char*)sqlite3_column_text(stmt, 2);
				string avgTime = (char*)sqlite3_column_text(stmt, 3);//not sure if its working.. gets data from the row
				int avargeTime = atoi(avgTime.c_str());
				string wrongAnswers = to_string(atoi(questionsCount.c_str()) - correctAnswersCount);
				personalStatus.push_back(wrongAnswers);
				personalStatus.push_back(avgTime);//insert data to vector
				personalStatus.push_back(correctAnswers);
				personalStatus.push_back(gamesCount);
			}
			else if (s == SQLITE_DONE)
			{
				break;
			}
			else
			{
				sqlite3_finalize(stmt);
				throw exception(RETRIVING_INFROMATION_ERROR);
			}
		}
		sqlite3_finalize(stmt);
		return personalStatus;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return personalStatus;
	}
}

int DataBase::insertNewGame()
{
	time_t now = time(0);
	// convert now to string form
	char* dt = ctime(&now);
	string gameStatus = "0";//new game status equal zero
	string sqlStatement = "INSERT INTO t_games(status, start_time, end_time) VALUES ('" + gameStatus + "', '" + dt + "', 'NULL');";//not sure if it is working
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		return -1;//something went wrong a new game did not inserted
	}
	else
	{
		this->_currGameId++;//tracking/counting the amount of games in the database
		return this->_currGameId;
	}
}

bool DataBase::updateGameStatus(int id)
{
	time_t now = time(0);
	// convert now to string form
	char* dt = ctime(&now);
	string gameStatus = "1";//new game status equal zero
	string sqlStatement = "UPDATE t_games SET status = '" + gameStatus + "', end_time = '" + dt + "' WHERE game_id = '" + to_string(id) + "';";//not sure if it is working
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;//something went wrong
	else
		return true;
}

bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)
{
	int correctAns = isCorrect;
	try
	{
		string sqlStatement = "INSERT INTO t_players_answers(game_id, username, question_id, player_answer, is_correct, answer_time) VALUES('" + to_string(gameId) + "', '" + username + "', '" + to_string(questionId) + "', '" + answer + "', '" + to_string(correctAns) + "', '" + to_string(answerTime) + "');";//not sure if working
		char* errMessage = nullptr;
		int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
		if (res != SQLITE_OK)
			throw exception(INSERTING_INFORMATION_ERROR);
		else
			return true;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return false;
	}

}
#include "DataBase.h"
#include "Question.h"

#define DB_NAME "TriviaDB.sqlite"

DataBase::DataBase() : _filename(DB_NAME)
{
	try
	{
		int file_exist = _access(this->_filename.c_str(), 0);
		int res = sqlite3_open(this->_filename.c_str(), &this->_db);
		if (res != SQLITE_OK)
		{
			this->_db = nullptr;
			throw exception("Failed to open DB");
		}
		if (file_exist == 0)
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
			sqlStatement = "CREATE TABLE t_players_answers(game_id INTEGER NOT NULL, username TEXT NOT NULL, question_id INTEGER NOT NULL, player_answer TEXT NOT NULL, is_correct INTEGER NOT NULL, answer_time INTEGER NOT NULL, PRIMARY KEY(game_id, username, question_id), FOREIGN KEY(game_id) REFERENCES t_games(game_id), FOREIGN KEY(username) REFERENCES t_users(username), FOREIGN KEY(question_id) REFERENCES t_questions(question_id));";
			res = sqlite3_exec(this->_db, sqlStatement, nullptr, nullptr, &errMessage);
			if (res != SQLITE_OK)
				throw exception("Database Problem");
		}
	}
	catch (exception& e)
	{
		cout << e.what() << endl;

		/* To Do: Send exit message to all players and exit games because of the glitch 
		   Until that wouldn't be coded, exit the program for debugging purposes.        */

		exit(1);
	}
}

DataBase::~DataBase()
{
	sqlite3_close(this->_db);
	this->_db = nullptr;
}

bool DataBase::isUserExists(string username)
{
	string sqlStatement = "SELECT username FROM t_users WHERE username = " + username + ";";
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;
	return true;
}

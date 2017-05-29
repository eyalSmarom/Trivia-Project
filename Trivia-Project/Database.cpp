#include "DataBase.h"
#include "Question.h"

#define DB_NAME "TriviaDB.sqlite"

DataBase::DataBase() : _filename(DB_NAME), _currGameId(0)
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

DataBase::DataBase(DataBase & other)
{
}

DataBase::~DataBase()
{
	sqlite3_close(this->_db);
	this->_db = nullptr;
}

bool DataBase::isUserExists(string username)
{
	string sqlStatement = "SELECT username FROM t_users WHERE username = " + username + ";";
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

bool DataBase::addNewUser(string username, string password, string email)
{
	string sqlStatement = "INSERT INTO t_users(" + username + ", " + password + ", " + email + ");";
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;
	else
		return true;
}

bool DataBase::isUserAndPassMatch(string username, string password)
{
	string sqlStatement = "SELECT * FROM t_users WHERE username = " + username + "AND Password = " + password + ";";
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
	string sqlStatement = "SELECT * FROM t_questions";
	sqlite3_stmt *stmt;
	vector<Question*> questionsVector;
	if (sqlite3_prepare_v2(this->_db, sqlStatement.c_str(), strlen(sqlStatement.c_str()) + 1, &stmt, NULL) != SQLITE_OK)
		return questionsVector;//somthing went wrong
	while (1)
	{
		int s;

		s = sqlite3_step(stmt);//get first row
		if (s == SQLITE_ROW)
		{
			int id = (char)sqlite3_column_text(stmt, 0) - '0';//not sure if its working
			string question = (char*)sqlite3_column_text(stmt, 1);
			string correctAns = (char*)sqlite3_column_text(stmt, 2);
			string ans2 = (char*)sqlite3_column_text(stmt, 3);
			string ans3 = (char*)sqlite3_column_text(stmt, 4);
			string ans4 = (char*)sqlite3_column_text(stmt, 5);
			Question* newQuestion = new Question(id, question, correctAns, ans2, ans3, ans4);//not sure if i sould free this memory later, also need to check if push_back shellow copy or not
			questionsVector.push_back(newQuestion);
		}
		else if (s == SQLITE_DONE)
		{
			break;
		}
		else
		{
			return questionsVector;//somthing went wrong
		}
	}
	int vSize = questionsVector.size();
	srand(time(NULL));
	while (vSize > questionsNo)//randomize to get random questions from data base
	{
		int randIndex = rand() % vSize;
		questionsVector.erase(questionsVector.begin() + randIndex);
		vSize--;
	}
	return questionsVector;
}

vector<Question*> DataBase::getBestScores()
{
	return vector<Question*>();
}

int DataBase::insertNewGame()
{
	time_t now = time(0);
	// convert now to string form
	char* dt = ctime(&now);
	string gameStatus = "0";//new game status equal zero
	string sqlStatement = "INSERT INTO t_games(" + gameStatus + ", " + dt + ", " + "NULL" + ");";//not sure if it is working
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
	{
		return this->_currGameId;//something went wrong a new game did not inserted
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
	string sqlStatement = "UPDATE t_games SET status = " + gameStatus + ", end_time = " + dt + "WHERE game_id = " + to_string(id) + ";";//not sure if it is working
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;//something went wrong
	else
		return true;
}

bool DataBase::addAnswerToPlayer(int gameId, string username, int questionId, string answer, bool isCorrect, int answerTime)
{
	string sqlStatement = "INSERT INTO t_players_answers(" + to_string(gameId) + ", " + username + ", " + to_string(questionId) + ", " + answer + ", " + to_string(isCorrect) + ", " + to_string(answerTime) + ");";//not sure if working
	char* errMessage = nullptr;
	int res = sqlite3_exec(this->_db, sqlStatement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		return false;//something went wrong
	else
		return true;
}
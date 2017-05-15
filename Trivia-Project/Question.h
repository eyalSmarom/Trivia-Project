#pragma once

#include "Helper.h"
#include <time.h>

class Question
{
public:
	Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4);
	string getQuestion();
	string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();
	~Question();

private:
	string _question;
	string _answers[4];
	int _correctAnswerIndex;
	int _id;
};
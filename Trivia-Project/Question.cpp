#include "Question.h"

Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4)
{
	int correctAnswerIndex, index2 = -1, index3 = -1, index4 = -1;
	srand(time(NULL));

	_id = id;
	_question = string(question);

	//Correct answer index.
	correctAnswerIndex = rand() % 4;
	_answers[correctAnswerIndex] = string(correctAnswer);
	_correctAnswerIndex = correctAnswerIndex;

	//Second answer index
	while (index2 == correctAnswerIndex || index2 == -1) { index2 = rand() % 4; }
	_answers[index2] = string(answer2);

	//Third answer index
	while (index3 == correctAnswerIndex || index3 == index2 || index3 == -1) { index3 = rand() % 4; }
	_answers[index3] = string(answer3);

	//Fourth answer index
	while (index4 == correctAnswerIndex || index4 == index3 || index4 == index2 || index4 == -1) { index4 = rand() % 4; }
	_answers[index4] = string(answer4);
}

string Question::getQuestion()
{
	return string(_question);
}

string * Question::getAnswers()
{
	return _answers;
}

int Question::getCorrectAnswerIndex()
{
	return _correctAnswerIndex;
}

int Question::getId()
{
	return _id;
}

Question::~Question()
{
	delete _answers;
}

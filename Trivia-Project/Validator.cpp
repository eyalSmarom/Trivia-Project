#include "Validator.h"

bool Validator::isPasswordValid(string password)
{
	int digitCounter = 0, Upper = 0, Lower = 0;

	if (password.size() < 4)
		return false;

	for (int i = 0; i < password.size(); i++)
	{
		if (isdigit(password[i])) digitCounter++;
		else if (islower(password[i])) Lower++;
		else if (isupper(password[i])) Upper++;
		if (isspace(password[i])) return false;
	}

	if (digitCounter && Upper && Lower)
		return true;

	return false;
}

bool Validator::isUsernameValid(string username)
{
	if (username.size() == 0) return false;

	if (!isalpha(username[0])) return false;

	for (int i = 0; i < username.size(); i++)
		if (isspace(username[i])) return false;
}

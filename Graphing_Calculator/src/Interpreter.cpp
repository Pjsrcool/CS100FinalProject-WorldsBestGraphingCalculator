#include "../header/Interpreter.hpp"
//for testing only
#include <iostream>
using std::cout;

HandleVars* Interpreter::getParsedEquation(string eq)
{
	cout << "getParsed\t" << eq << "\n\n";		//<<<<<<<<----------------------------------------------------------
	eq = this->removeSpaces(eq);
	cout << "getParsed:\t" << eq << "\n";		//<<<<<<<<----------------------------------------------------------
	eq = this->findParenthesis(eq);
	NoVar* test = new NoVar(eq);		//<<<<-------------------------Needs Changed-------------------------------------
	return test;
}

string Interpreter::removeSpaces(string eq)
{
	for (int i = 0; i < eq.size(); i++)
	{
		if (eq.at(i) == ' ')
			eq.erase(i--, 1);
	}
	return eq;
}

/*
Finds the locations of outer parenthesis and passes them to handleParenthesis
 - If none returns the given string
*/
string Interpreter::findParenthesis(string eq)
{
	cout << "\n\t\t\t\tTop\t" << eq << "\n";		//<<<<<<<<----------------------------------------------------------
	vector<vector<int>> parenLocations;
	int openParen = 0;		//Used to track how many open parenthesis have been found	|	parenFound(+1 if open, -1 if closed)
	vector<int> paren;			//Tracks location of both parenthesis 0='(' 1=')'
	for (int i = 0; i < eq.size(); i++)
	{
		if (eq.at(i) == '(')
			if (openParen++ == 0)
				paren.push_back(i);
		if (eq.at(i) == ')')
			if (--openParen == 0)
			{
				paren.push_back(i);
				parenLocations.push_back(paren);
				cout << "\t\t\t\tMid\t" << eq.substr(paren[0],  1 + paren[1]) << "\n"; //<<<<<<<<----------------------------------------------------------
				paren.clear();
			}
	}
	cout << "\t\t\t\tEnd\t" << eq << "\n\n";		//<<<<<<<<----------------------------------------------------------
	if (parenLocations.empty())
		return eq;
	return handleParenthesis(eq, parenLocations);		//There were parenthesis inside the parenthesis
}

//adjusts all parenLocations after parenLocations[i][0] by change
vector<vector<int>> Interpreter::adjustLocation(vector<vector<int>> parenLocations, int change, int i)
{
	parenLocations[i][1] += change;
	for (++i; i < parenLocations.size(); i++)
	{
		parenLocations[i][0] += change;
		parenLocations[i][1] += change;
	}
	return parenLocations;
}

string Interpreter::handleParenthesis(string eq, vector<vector<int>> parenLocations)
{
	if (parenLocations.size() > 1)
	{
		int powPos = -1;
		for (int i = 0; i < parenLocations.size() - 1; i++)		//Handle possible paren with exponent------------------DOESN'T WORK WITH ()^-()-----------------
		{
			if (eq.at(parenLocations[i][1] + 1) == '^')		//a paren is in form ()^x
			{
				string exponent = "";
				int originalSize = parenLocations.size();
				if (parenLocations[i][1] == parenLocations[i + 1][0] - 2)		//eq has paren^paren
					exponent = this->findParenthesis(eq.substr(parenLocations[i + 1][0] + 1, parenLocations[i + 1][1]));
				else if (parenLocations[i][1] + 2 < eq.size())
					exponent = eq.at(parenLocations[i][1] + 2);
				eq.replace(parenLocations[i][0], 1 + parenLocations[i+1][1] - parenLocations[i][0], this->distributePow(this->findParenthesis(eq.substr(parenLocations[i][0] + 1, parenLocations[i][1])), exponent));
				parenLocations = this->adjustLocation(parenLocations, eq.size() - originalSize, i);
				parenLocations.erase(parenLocations.begin() + i--);
				cout << "handleParenthesis:\t" << eq << "\n\n";	//<<<<<<<<----------------------------------------------------------
			}
		}
	}
	if (parenLocations.size() > 1)		//handle possible paren multiplication
	{
		for (int i = 0; i < parenLocations.size() - 1; i++)
		{
			if (parenLocations[i][0] - 1 > 0)
				if (isdigit(eq.at(parenLocations[i][0] - 1)) || isalpha(eq.at(parenLocations[i][0] - 1)))
				{
					eq.insert(parenLocations[i][0]++ - 1, "*");
					this->adjustLocation(parenLocations, 1, i);
				}
			if (parenLocations[i][1] + 1 < eq.size())
				if (isdigit(eq.at(parenLocations[i][1] + 1)) || isalpha(eq.at(parenLocations[i][1] + 1)))
				{
					eq.insert(parenLocations[i][1] + 1, "*");
					this->adjustLocation(parenLocations, 1, i);
				}
			//there is at least one operation between the parenthesis
			int originalSize = eq.size();
			bool run = false;
			if (parenLocations[i][1] != parenLocations[i + 1][0] - 1 && parenLocations[i][1] >= parenLocations[i + 1][0] - 3)
			{
				if (eq.at(parenLocations[i][1] + 1) == '*')
				{
					if (eq.at(parenLocations[i][1] + 2) == '-')
					{
						eq.replace(parenLocations[i][1] + 2, 1 + parenLocations[i + 1][1], "+" + this->distributeNeg(this->findParenthesis(eq.substr(parenLocations[i + 1][0] + 1, parenLocations[i + 1][1]))));
						parenLocations = this->adjustLocation(parenLocations, eq.size() - originalSize, i);
						originalSize = parenLocations.size();
						cout << "handle-Mult:\t" << eq << "\n";		//<<<<<<<<----------------------------------------------------------
					}
					if (eq.at(parenLocations[i][1] + 2) == '+')
						run = true;
				}
			}
			else if (parenLocations[i][1] == parenLocations[i + 1][0] - 1)
				run = true;
			if (run)
			{
				eq.replace(parenLocations[i][0], 1 + parenLocations[i + 1][1] - parenLocations[i][0], this->distributeMult(this->findParenthesis(eq.substr(parenLocations[i][0] + 1, parenLocations[i][1] - 1 - parenLocations[i][0])), this->findParenthesis(eq.substr(parenLocations[i + 1][0] + 1, parenLocations[i + 1][1] - 1 - parenLocations[i + 1][0]))));
				parenLocations = this->adjustLocation(parenLocations, eq.size() - originalSize, i);
				parenLocations.erase(parenLocations.begin() + i--);
				cout << "handleMult:\t" << eq << "\n";		//<<<<<<<<----------------------------------------------------------
			}
		};
	}
	for (int i = 0; i < parenLocations.size(); i++)		//Handles distributing negatives
	{
		if (parenLocations[i][0] > 0 && eq.at(parenLocations[i][0] - 1) == '-')
		{
			eq.erase(parenLocations[i][0] - 1, 1);

			//Adjusts for deletion of leading '-'
			i--;
			parenLocations[i][0]--;

			int  originalSize = eq.size();
			eq.replace(parenLocations[i][0], 1 + parenLocations[i][1] - parenLocations[i][1], this->distributeNeg(this->findParenthesis(eq.substr(parenLocations[i][0], 1 + parenLocations[i][1] - parenLocations[i][0]))));
			parenLocations = this->adjustLocation(parenLocations, eq.size() - originalSize, i);
			parenLocations.erase(parenLocations.begin() + i--);
			cout << "handleNeg:\t" << eq << "\n\n";		//<<<<<<<<----------------------------------------------------------
		}
	}
	cout << "finishedParen:\t" << eq << "\n\n";		//<<<<<<<<----------------------------------------------------------
	return eq;
}


//-------------------Here Lies Possibly Unnessesary Functions-------------------



string Interpreter::distributeNeg(string eq)
{
	bool firstDigit = true;
	for (int i = 0; i < eq.size(); i++)
	{
		if (isdigit(eq.at(i)))
		{
			if (i == 0)
			{
				eq.insert(0, 1, '+');	//Second if() assumes a sign is given
				i++;
			}
			if (firstDigit == true)
			{
				firstDigit = false;
				if (eq.at(i - 1) == '+')	//flips sign to negative
				{
					eq.erase(--i, 1);
					eq.insert(i, 1, '-');
				}
				else if (eq.at(i - 1) == '-')	//flips sign to positive
				{
					eq.erase(--i, 1);
					eq.insert(i, 1, '+');
				}
			}
		}
		else
			firstDigit = true;
	}
	return eq;
}

string Interpreter::distributeMult(string parenLeft, string parenRight)
{
	vector<string> parenValsLeft = this->getVals(parenLeft), parenValsRight = this->getVals(parenRight);
	string result = "";
	for (int i = 0; i < parenValsLeft.size(); i++)
	{
		for (int j = 0; j < parenValsRight.size(); j++)
		{
			result += parenValsLeft[i] + "*" + parenValsRight[j] + "+";
			cout << "distributeMult:\t" << result << "\n";		//<<<<<<<<----------------------------------------------------------

		}
	}
	result.pop_back();
	return result;
}

string Interpreter::distributePow(string parenLeft, string parenRight)
{
	return parenLeft + parenRight;		//Placeholder
}

vector<string> Interpreter::getVals(string eq)
{
	vector<string> parenVals;
	bool digitStarted = false;
	int firstPos = 0;
	for (int i = 0; i < eq.size(); i++)
	{
		char val = eq.at(i);
		if (isdigit(val) || val == '.' || isalpha(val))
		{
			if (digitStarted == false)
			{
				digitStarted = true;
				firstPos = i;
				if (i != 0 && eq.at(i - 1) == '-')
					firstPos--;
			}
		}
		if (i != 0 && val == '+' || val == '-')
		{
			digitStarted = false;
			parenVals.push_back(eq.substr(firstPos, i - firstPos));
			cout << "getVals\tTop:\t" << eq.substr(firstPos, i - firstPos) << "\n";
		}
	}
	parenVals.push_back(eq.substr(firstPos, eq.size() - firstPos));
	cout << "getVals\tEnd:\t" << eq.substr(firstPos, eq.size() - firstPos) << "\n";
	return parenVals;
}
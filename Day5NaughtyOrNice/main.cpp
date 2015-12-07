#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <unordered_map>

using namespace std;

vector<string> GetLines()
{
	vector<string> out;
	
	FILE* file = fopen("input.txt", "r");
	if (file != nullptr)
	{
		char buffer[256];
		while (fgets(buffer, 256, file))
		{
			out.push_back(buffer);
		}
	}
	
	return out;
}

bool HasAtLeast3Vowels(const string& inString)
{
	static const string kVowels = "aeiou";
	
	int vowelCount = 0;
	
	for (char c : inString)
	{
		if (kVowels.find(c) != string::npos)
		{
			vowelCount++;
		}
	}
	
	return vowelCount >= 3;
}

bool HasDoubleLetters(const string& inString)
{
	for (size_t i = 0; i < inString.size() - 1; ++i)
	{
		if (inString[i] == inString[i+1])
		{
			return true;
		}
	}
	
	return false;
}

bool HasIllegalSubstrings(const string& inString)
{
	static const vector<string> kIllegalSubStrings = [] ()
	{
		vector<string> out;
		out.push_back("ab");
		out.push_back("cd");
		out.push_back("pq");
		out.push_back("xy");
		return out;
	} ();
	
	for (auto& subString : kIllegalSubStrings)
	{
		if (inString.find(subString) != string::npos)
		{
			return true;
		}
	}
	
	return false;
}

bool HasPairOfLettersAppearingAtLeastTwiceWithoutOverlapping(const string& inString)
{
	for (size_t i = 0; i < inString.size() - 1; ++i)
	{
		string letterPair = inString.substr(i, 2);
		if (inString.find(letterPair, i + 2) != string::npos)
		{
			return true;
		}
	}
	
	return false;
}

bool HasSameLetterEitherSideOfAnotherLetter(const string& inString)
{
	for (size_t i = 0; i < inString.size() - 2; ++i)
	{
		if (inString[i] == inString[i+2])
		{
			return true;
		}
	}
	
	return false;
}

void DoTest1(const vector<string>& inLines)
{
	int niceWords = 0;
	for (auto& line : inLines)
	{
		bool has3Vowels 			= HasAtLeast3Vowels(line);
		bool hasDoubleLetters 		= HasDoubleLetters(line);
		bool hasNoIllegalSubstrings = !HasIllegalSubstrings(line);
		
		if (has3Vowels && hasDoubleLetters && hasNoIllegalSubstrings)
		{
			niceWords++;
		}
	}
	
	cout << "Num nice words (test 1) = " << niceWords << endl;
}

void DoTest2(const vector<string>& inLines)
{
	int niceWords = 0;
	for (auto& line : inLines)
	{
		bool condition0 = HasPairOfLettersAppearingAtLeastTwiceWithoutOverlapping(line);
		bool condition1 = HasSameLetterEitherSideOfAnotherLetter(line);
		
		if (condition0 && condition1)
		{
			niceWords++;
		}
	}
	
	cout << "Num nice words (test 2) = " << niceWords << endl;
}

int main()
{
	vector<string> lines = GetLines();

	DoTest1(lines);
	DoTest2(lines);
	
	return 0;
}
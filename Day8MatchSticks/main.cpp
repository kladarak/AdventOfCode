#include <iostream>
#include <assert.h>

#include "File.h"

using namespace std;

int main()
{
	vector<string> lines = FileHelper::GetLinesInFile("input.txt");

	for (auto& line : lines)
	{
		if (line.back() == '\n')
		{
			line.pop_back();
		}
	}

	size_t numCodeCharacters = 0;
	for (auto& line : lines)
	{
		numCodeCharacters += line.size();
	}

	size_t numLiteralCharacters = 0;
	for (auto& line : lines)
	{
		for (size_t i = 1; i < line.size() - 1; ++i)
		{
			++numLiteralCharacters;

			char c = line[i];
			if (c == '\\')
			{
				char nextC = line[i+1];
				switch (nextC)
				{
					case '\\': i++; break;
					case '\"': i++; break;
					case 'x': i += 3; break;
					default: assert(false); break;
				}
			}
		}
	}

	size_t diffTest1 = numCodeCharacters - numLiteralCharacters;
	cout << "Difference Test 1 = " << diffTest1 << endl;

	size_t numCodeCharsWithEscapedChars = 0;
	for (auto& line : lines)
	{
		for (char c : line)
		{
			if (c == '\"' || c == '\\')
			{
				++numCodeCharsWithEscapedChars;
			}

			++numCodeCharsWithEscapedChars;
		}

		numCodeCharsWithEscapedChars += 2; // Start and end with new quotes.
	}

	size_t diffTest2 = numCodeCharsWithEscapedChars - numCodeCharacters;
	cout << "Difference Test 2 = " << diffTest2 << endl;

	return 0;
}
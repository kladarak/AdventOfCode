#include <iostream>
#include <assert.h>

#include "File.h"

using namespace std;

int main()
{
	vector<string> lines = FileHelper::GetLinesInFile("input.txt");

	for (auto& line : lines)
	{
		// remove \n from end of each line.
		line.pop_back();
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

	size_t difference = numCodeCharacters - numLiteralCharacters;
	cout << "Difference = " << difference << endl;
	return 0;
}
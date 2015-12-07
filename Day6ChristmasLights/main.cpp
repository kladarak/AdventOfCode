#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <assert.h>
#include <functional>
#include <algorithm>

#include "File.h"
#include "StringHelper.h"

using namespace std;

struct Vec2
{
	int x;
	int y;
};

//-----------------------------------------------------------

enum class EOperation
{
	TurnOn,
	TurnOff,
	Toggle
};

struct Instruction
{
	EOperation	mOperation;
	Vec2		mFrom;
	Vec2		mTo;
};

struct OperationMapping
{
	string		mString;
	EOperation	mOperation;
};

static const vector<OperationMapping> kOpMap = 
{
	{ "turn on",	EOperation::TurnOn },
	{ "turn off",	EOperation::TurnOff },
	{ "toggle",		EOperation::Toggle },
};

Vec2 ParseCoordinate(const string& inLine)
{
	vector<string> numbers = StringHelper::Delimit(inLine, ", ");
	assert(numbers.size() == 2);

	Vec2 out;
	out.x = stoi(numbers[0]);
	out.y = stoi(numbers[1]);
	return out;
}

Instruction ParseInstruction(const string& inLine)
{
	auto line = inLine;

	Instruction out;
	
	for (auto& op : kOpMap)
	{
		if (inLine.find(op.mString) != string::npos)
		{
			out.mOperation = op.mOperation;
			line = StringHelper::FindAndReplace(line, op.mString, " ");
			break;
		}
	}

	line = StringHelper::FindAndReplace(line, "through", "");
	vector<string> coords = StringHelper::Delimit(line, " ");
	assert(coords.size() == 2);

	out.mFrom	= ParseCoordinate(coords[0]);
	out.mTo		= ParseCoordinate(coords[1]);

	return out;
}

vector<Instruction> ParseLines(const vector<string>& inLines)
{
	vector<Instruction> out;

	for (auto& line : inLines)
	{
		out.push_back(ParseInstruction(line));
	}

	return out;
}

//------------------------------------------------------------------------------

bool DoOperation(EOperation inOperation, bool inOldValue)
{
	switch (inOperation)
	{
		case EOperation::TurnOff:	return false;
		case EOperation::TurnOn:	return true;
		case EOperation::Toggle:	return !inOldValue;
	}

	assert(false);
	return false;
}

void DoInstruction(vector<vector<bool>>& inGrid, const Instruction& inInstruction)
{
	for (int x = inInstruction.mFrom.x; x <= inInstruction.mTo.x; ++x)
	{
		for (int y = inInstruction.mFrom.y; y <= inInstruction.mTo.y; ++y)
		{
			inGrid[x][y] = DoOperation(inInstruction.mOperation, inGrid[x][y]);
		}
	}
}

void DoTest1(const vector<Instruction>& inInstructions)
{
	vector<bool> yPrototype(1000, false);
	vector<vector<bool>> grid(1000, yPrototype);

	for (auto& instruction : inInstructions)
	{
		DoInstruction(grid, instruction);
	}

	int lightsOn = 0;
	
	for (int x = 0; x < 1000; ++x)
	{
		for (int y = 0; y < 1000; ++y)
		{
			lightsOn += grid[x][y] ? 1 : 0;
		}
	}

	cout << "Num lights on = " << lightsOn << endl;
}

//----------------------------------------------------------------

int DoOperation(EOperation inOperation, int inOldValue)
{
	switch (inOperation)
	{
		case EOperation::TurnOff:	return max(inOldValue - 1, 0);
		case EOperation::TurnOn:	return inOldValue + 1;
		case EOperation::Toggle:	return inOldValue + 2;
	}

	assert(false);
	return 0;
}

void DoInstruction(vector<vector<int>>& inGrid, const Instruction& inInstruction)
{
	for (int x = inInstruction.mFrom.x; x <= inInstruction.mTo.x; ++x)
	{
		for (int y = inInstruction.mFrom.y; y <= inInstruction.mTo.y; ++y)
		{
			inGrid[x][y] = DoOperation(inInstruction.mOperation, inGrid[x][y]);
		}
	}
}

void DoTest2(const vector<Instruction>& inInstructions)
{
	vector<int> yPrototype(1000, 0);
	vector<vector<int>> grid(1000, yPrototype);

	for (auto& instruction : inInstructions)
	{
		DoInstruction(grid, instruction);
	}

	int totalBrightness = 0;
	
	for (int x = 0; x < 1000; ++x)
	{
		for (int y = 0; y < 1000; ++y)
		{
			totalBrightness += grid[x][y];
		}
	}

	cout << "Total brightness = " << totalBrightness << endl;
}

//------------------------------------------------------------

int main()
{
	vector<string> lines = FileHelper::GetLinesInFile("input.txt");
	vector<Instruction> instructions = ParseLines(lines);

	DoTest1(instructions);
	DoTest2(instructions);

	return 0;
}

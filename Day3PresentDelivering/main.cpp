#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

#include "File.h"

using namespace std;

//-----------------------------------------------------------------------------------

class Vec2D
{
public:
	int x;
	int y;

	Vec2D() { }
	Vec2D(int inX, int inY) : x(inX), y(inY) { };

	Vec2D& operator += (const Vec2D& inRHS)
	{
		x += inRHS.x;
		y += inRHS.y;
		return *this;
	}

	Vec2D& operator -= (const Vec2D& inRHS)
	{
		x -= inRHS.x;
		y -= inRHS.y;
		return *this;
	}
};
	
Vec2D operator+(const Vec2D& inLHS, const Vec2D& inRHS)
{
	Vec2D out = inLHS;
	out += inRHS;
	return out;
}
	
Vec2D operator-(const Vec2D& inLHS, const Vec2D& inRHS)
{
	Vec2D out = inLHS;
	out -= inRHS;
	return out;
}

Vec2D operator-(const Vec2D& inLHS)
{
	return Vec2D(-inLHS.x, -inLHS.y);
}

//-----------------------------------------------------------------------------------

struct Rect
{
	Vec2D BottomLeft;
	Vec2D TopRight;
	Rect() : BottomLeft(0, 0), TopRight(0, 0) { }

	void Extend(const Vec2D& inPos)
	{
		BottomLeft.x	= min(BottomLeft.x	, inPos.x);
		BottomLeft.y	= min(BottomLeft.y	, inPos.y);
		TopRight.x		= max(TopRight.x	, inPos.x);
		TopRight.y		= max(TopRight.y	, inPos.y);
	}
};

//-----------------------------------------------------------------------------------

class Grid2D
{
public:
	Grid2D(const Vec2D& inSize, const Vec2D& inOffset)
		: Offset(inOffset)
	{
		vector<int> yPrototype;
		yPrototype.resize(inSize.y, 0);
		Data.resize(inSize.x, yPrototype);
	}

	void Set(const Vec2D& inLocation, int inValue)
	{
		Vec2D vecLoc = inLocation + Offset;
		Data[vecLoc.x][vecLoc.y] = inValue;
	}

	int Get(const Vec2D& inLocation) const
	{
		Vec2D vecLoc = inLocation + Offset;
		return Data[vecLoc.x][vecLoc.y];
	}

	void Inc(const Vec2D& inLocation)
	{
		Set(inLocation, Get(inLocation) + 1);
	}

	template<typename TFunctor>
	void ForEach(const TFunctor& inFunctor) const
	{
		for (int x = 0; x < (int) Data.size(); ++x)
		{
			for (int y = 0; y < (int) Data[x].size(); ++y)
			{
				Vec2D location = Vec2D(x, y) - Offset;
				inFunctor(Get(location), location);
			}
		}
	}

private:
	vector<vector<int>> Data;
	Vec2D				Offset;
};

//-----------------------------------------------------------------------------------

vector<Vec2D> ParseInstructions(const string& inInstructions)
{
	struct CharToDirection
	{
		char	character;
		Vec2D	direction;
		CharToDirection(char c, int x, int y) : character(c), direction(x, y) { }
	};

	static const vector<CharToDirection> kInstructionInterpretations = [] ()
	{
		vector<CharToDirection> out;
		out.push_back( CharToDirection('>', 1, 0)	);
		out.push_back( CharToDirection('<', -1, 0)	);
		out.push_back( CharToDirection('^', 0, 1)	);
		out.push_back( CharToDirection('v', 0, -1)	);
		return out;
	} ();

	vector<Vec2D> out;

	for (char c : inInstructions)
	{
		for (auto& interp : kInstructionInterpretations)
		{
			if (interp.character == c)
			{
				out.push_back(interp.direction);
				break;
			}
		}
	}

	return out;
}

void GetTravelExtents(const vector<Vec2D>& inInstructions, Rect& inRect)
{	
	Vec2D pos(0, 0);

	for (auto& dir : inInstructions)
	{
		pos += dir;
		inRect.Extend(pos);
	}
}

Grid2D CreateGridFromExtents(const Rect& inExtents)
{
	Vec2D extentsSize = inExtents.TopRight - inExtents.BottomLeft;
	Vec2D gridSize = extentsSize + Vec2D(2, 2); // Size + 1 in each direction.
	Vec2D offset = -inExtents.BottomLeft;

	return Grid2D(gridSize, offset);
}

void ProcessInstructions(const vector<Vec2D>& inInstructions, Grid2D& ioGrid)
{
	Vec2D pos(0, 0);
	ioGrid.Inc(pos);

	for (auto& dir : inInstructions)
	{
		pos += dir;
		ioGrid.Inc(pos);
	}
}

int GetVisitedCount(const Grid2D& inGrid)
{
	int visited = 0;

	inGrid.ForEach([&visited] (int inValue, const Vec2D& inLocation)
	{
		if (inValue > 0) ++visited;
	});

	return visited;
}

int GetNumHousesVisitedBySantaAlone(const vector<Vec2D>& inInstructions)
{
	Rect extents;
	GetTravelExtents(inInstructions, extents);

	Grid2D grid = CreateGridFromExtents(extents);

	ProcessInstructions(inInstructions, grid);
	return GetVisitedCount(grid);
}

int GetNumHousesVisitedBySantaAndRobot(const vector<Vec2D>& inInstructions)
{
	vector<Vec2D> dirsForSanta;
	vector<Vec2D> dirsForRobot;

	for (size_t i = 0; i < inInstructions.size(); ++i)
	{
		auto& forWhom = (i % 2 == 0) ? dirsForSanta : dirsForRobot;
		forWhom.push_back(inInstructions[i]);
	}
	
	Rect extents;
	GetTravelExtents(dirsForSanta, extents);
	GetTravelExtents(dirsForRobot, extents);
	
	Grid2D grid = CreateGridFromExtents(extents);

	ProcessInstructions(dirsForSanta, grid);
	ProcessInstructions(dirsForRobot, grid);
	
	return GetVisitedCount(grid);
}

int main()
{
	string input = FileHelper::ReadFileToString("input.txt");
	vector<Vec2D> instructions = ParseInstructions(input);
	
	cout << "Num houses visited by Santa alone = "		<< GetNumHousesVisitedBySantaAlone(instructions)	<< endl;
	cout << "Num houses visited by Santa and robot = "	<< GetNumHousesVisitedBySantaAndRobot(instructions) << endl;

	return 0;
}

#include <iostream>
#include <assert.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

template<typename TType>
class DoubleBuffer
{
public:
	DoubleBuffer() : CurrentFrontIndex(0) {}
	TType&	Front()			{ return Buffer[FrontIndex()];		}
	TType&	Back()			{ return Buffer[BackIndex()];		}
	void	Swap()			{ CurrentFrontIndex = BackIndex();	}

private:
	int		FrontIndex() const	{ return CurrentFrontIndex;		}
	int		BackIndex() const	{ return CurrentFrontIndex ^ 1; }

	TType	Buffer[2];
	int		CurrentFrontIndex;
};

typedef vector<bool> GridRow;
typedef vector<GridRow> Grid;

Grid GetInitialLightGrid(const vector<string>& inLines)
{
	Grid grid;

	for (auto& line : inLines)
	{
		grid.push_back(GridRow());

		for (char c : line)
		{
			if (c == '#')
			{
				grid.back().push_back(true);
			}
			else if (c == '.')
			{
				grid.back().push_back(false);
			}
		}
	}

	return grid;
}

int CountOnNeighbours(const Grid& inGrid, const int inX, const int inY)
{
	const int x = inX - 1;
	const int y = inY - 1;

	int sum = 0;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int testX = x + i;
			int testY = y + j;

			if (testX == inX && testY == inY)
			{
				continue;
			}

			bool inRange = (testX >= 0)
						&& (testX < (int) inGrid.size())
						&& (testY >= 0)
						&& (testY < (int) inGrid[testX].size());

			if (inRange && inGrid[testX][testY])
			{
				++sum;
			}
		}
	}

	return sum;
}

void Iterate(DoubleBuffer<Grid>& inGridBuffer)
{
	auto const& front = inGridBuffer.Front();
	auto& back = inGridBuffer.Back();

	for (int x = 0; x < (int) front.size(); ++x)
	{
		for (int y = 0; y < (int) front[x].size(); ++y)
		{
			int neighboursOn = CountOnNeighbours(front, x, y);
			back[x][y] = front[x][y];

			if (front[x][y] && (neighboursOn != 2) && (neighboursOn != 3))
			{
				back[x][y] = false;
			}
			else if (!front[x][y] && (neighboursOn == 3))
			{
				back[x][y] = true;
			}
		}
	}
}

int NumLightsOn(const Grid& inGrid)
{
	int sum = 0;

	for (auto& row : inGrid)
	{
		for (bool cell : row)
		{
			if (cell)
			{
				sum++;
			}
		}
	}

	return sum;
}

void Print(const Grid& inGrid)
{
	for (auto& row : inGrid)
	{
		for (bool cell : row)
		{
			cout << (cell ? '#' : '.');
		}
		cout << endl;
	}
	cout << endl;
}

int DoTest1(const Grid& inGrid)
{
	DoubleBuffer<Grid> gridBuffer;
	gridBuffer.Back() = gridBuffer.Front() = inGrid;

	for (int i = 0; i < 100; ++i)
	{
		Iterate(gridBuffer);
		gridBuffer.Swap();
	}

	return NumLightsOn(gridBuffer.Front());
}

int DoTest2(const Grid& inGrid)
{
	DoubleBuffer<Grid> gridBuffer;
	gridBuffer.Back() = gridBuffer.Front() = inGrid;
	
	auto forceOnTheCorners = [] (Grid& inGrid)
	{
		inGrid.front().front() = true;
		inGrid.front().back() = true;
		inGrid.back().front() = true;
		inGrid.back().back() = true;
	};

	forceOnTheCorners(gridBuffer.Back());
	forceOnTheCorners(gridBuffer.Front());

	for (int i = 0; i < 100; ++i)
	{
		Iterate(gridBuffer);
		forceOnTheCorners(gridBuffer.Back());
		gridBuffer.Swap();
	}

	return NumLightsOn(gridBuffer.Front());
}

int main()
{
	vector<string>	lines	= FileHelper::GetLinesInFile("input.txt");
	Grid			grid	= GetInitialLightGrid(lines);
	
	int lightsOnWithNoFaultyLights = DoTest1(grid);
	int lightsOnWithFaultyLights = DoTest2(grid);

	cout << "Num lights on after 100 iterations (no faulty lights) = " << lightsOnWithNoFaultyLights << endl;
	cout << "Num lights on after 100 iterations (faulty lights) = " << lightsOnWithFaultyLights << endl;

	return 0;
}

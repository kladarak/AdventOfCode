#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include <functional>

#include "File.h"
#include "StringHelper.h"

using namespace std;

class LocationMap
{
public:

	void				SetDistance(const string& inFrom, const string& inTo, int inDistance);

	size_t				GetNumLocations() const							{ return mLocations.size(); }
	int					GetDistance(size_t inFrom, size_t inTo) const	{ return mDistances[inFrom][inTo]; }

	void				PrintMap() const;

private:
	size_t				FindOrAddLocation(const string& inLocation);

	vector<string>		mLocations;
	vector<vector<int>> mDistances;

};

size_t LocationMap::FindOrAddLocation(const string& inLocation)
{
	for (size_t i = 0; i < mLocations.size(); ++i)
	{
		if (inLocation == mLocations[i])
		{
			return i;
		}
	}

	mLocations.push_back(inLocation);
	mDistances.push_back(vector<int>());

	size_t size = mLocations.size();

	for (auto& row : mDistances)
	{
		row.resize(size);
	}

	return size - 1;
}

void LocationMap::SetDistance(const string& inFrom, const string& inTo, int inDistance)
{
	size_t from = FindOrAddLocation(inFrom);
	size_t to = FindOrAddLocation(inTo);

	mDistances[from][to] = inDistance;
	mDistances[to][from] = inDistance;
}

void LocationMap::PrintMap() const
{
	for (auto& row : mDistances)
	{
		for (int distance : row)
		{
			cout << distance << "\t";
		}

		cout << endl;
	}
}

//---------------------------------------------------------------------------------------

LocationMap	CreateMap(const vector<string>& inLines)
{
	LocationMap map;

	for (auto& line : inLines)
	{
		string			withoutTo		= StringHelper::FindAndReplace(line, " to ", " ");
		string			withoutEquals	= StringHelper::FindAndReplace(withoutTo, " = ", " ");
		vector<string>	delimited		= StringHelper::Delimit(withoutEquals, " ");
		assert(delimited.size() == 3);

		int distance = stoi(delimited[2]);
		map.SetDistance(delimited[0], delimited[1], distance);
	}

	return map;
}

//---------------------------------------------------------------------------------------

class PathFinder
{
public:
	PathFinder(const LocationMap& inMap) : mMap(inMap), mBestDistance(INT_MAX) { }

	int					FindShortestPath();
	int					FindLongestPath();

private:
	void				Start(const function<int (int)>& inTest);
	void				Recurse(const size_t inFrom, const size_t* inRemaining, const size_t inNumRemaining, const int inDistanceSoFar, const function<int (int)>& inTest);

	const LocationMap&	mMap;
	int					mBestDistance;
};

int	PathFinder::FindShortestPath()
{
	mBestDistance = INT_MAX;
	Start([this] (int inDistance) { return min(mBestDistance, inDistance); });
	return mBestDistance;
}

int	PathFinder::FindLongestPath()
{
	mBestDistance = 0;
	Start([this] (int inDistance) { return max(mBestDistance, inDistance); });
	return mBestDistance;
}

void PathFinder::Start(const function<int (int)>& inTest)
{
	size_t numRemaining = mMap.GetNumLocations() - 1;
	size_t* remaining = (size_t*) alloca(numRemaining * sizeof(size_t));

	for (size_t i = 0; i < mMap.GetNumLocations(); ++i)
	{
		size_t remIndex = 0;
		for (size_t n = 0; n < mMap.GetNumLocations(); ++n)
		{
			if (i != n)
			{
				remaining[remIndex] = n;
				++remIndex;
			}
		}

		Recurse(i, remaining, numRemaining, 0, inTest);
	}
}

void PathFinder::Recurse(const size_t inFrom, const size_t* inRemaining, const size_t inNumRemaining, const int inDistanceSoFar, const function<int (int)>& inTest)
{
	size_t numRemaining = inNumRemaining - 1;
	size_t* remaining = (numRemaining > 0) ? (size_t*) alloca(numRemaining * sizeof(size_t)) : nullptr;

	for (size_t i = 0; i < inNumRemaining; ++i)
	{
		size_t to = inRemaining[i];
		int distSoFar = inDistanceSoFar + mMap.GetDistance(inFrom, to);

		if (numRemaining > 0)
		{
			size_t remIndex = 0;
			for (size_t n = 0; n < inNumRemaining; ++n)
			{
				if (i != n)
				{
					remaining[remIndex] = inRemaining[n];
					++remIndex;
				}
			}

			Recurse(to, remaining, numRemaining, distSoFar, inTest);
		}
		else
		{
			mBestDistance = inTest(distSoFar);
		}
	}
}

//---------------------------------------------------------------------------------------

int main()
{
	vector<string>	lines	= FileHelper::GetLinesInFile("input.txt");
	LocationMap		locMap	= CreateMap(lines);

	locMap.PrintMap();

	int shortestDistance	= PathFinder(locMap).FindShortestPath();
	int longestDistance		= PathFinder(locMap).FindLongestPath();

	cout << "Shortest path = " << shortestDistance << endl;
	cout << "Longest path = " << longestDistance << endl;

	return 0;
}

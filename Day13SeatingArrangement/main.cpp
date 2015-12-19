#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm>
#include <functional>

#include "File.h"
#include "StringHelper.h"

using namespace std;

class HappinessMap
{
public:

	void				SetHappiness(const string& inFrom, const string& inTo, int inHappiness);

	size_t				GetNumPeople() const							{ return mPeople.size(); }
	int					GetHappiness(size_t inFrom, size_t inTo) const	{ return mHappiness[inFrom][inTo]; }
	const string&		GetPerson(size_t inIndex) const					{ return mPeople[inIndex]; }

	void				PrintMap() const;

private:
	size_t				FindOrAddPerson(const string& inPerson);

	vector<string>		mPeople;
	vector<vector<int>> mHappiness;

};

size_t HappinessMap::FindOrAddPerson(const string& inPerson)
{
	for (size_t i = 0; i < mPeople.size(); ++i)
	{
		if (inPerson == mPeople[i])
		{
			return i;
		}
	}

	mPeople.push_back(inPerson);
	mHappiness.push_back(vector<int>());

	size_t size = mPeople.size();

	for (auto& row : mHappiness)
	{
		row.resize(size);
	}

	return size - 1;
}

void HappinessMap::SetHappiness(const string& inFrom, const string& inTo, int inHappiness)
{
	size_t from = FindOrAddPerson(inFrom);
	size_t to = FindOrAddPerson(inTo);

	mHappiness[from][to] = inHappiness;
}

void HappinessMap::PrintMap() const
{
	for (auto& row : mHappiness)
	{
		for (int happiness : row)
		{
			cout << happiness << "\t";
		}

		cout << endl;
	}
}

//---------------------------------------------------------------------------------------

HappinessMap CreateMap(const vector<string>& inLines)
{
	HappinessMap map;

	for (auto& line : inLines)
	{
		string parsed = StringHelper::FindAndReplace(line, " would gain ", " ");
		parsed = StringHelper::FindAndReplace(parsed, " would lose ", " -");
		parsed = StringHelper::FindAndReplace(parsed, " happiness units by sitting next to ", " ");
		vector<string>	delimited = StringHelper::Delimit(parsed, " .\n");
		assert(delimited.size() == 3);

		int happiness = stoi(delimited[1]);
		map.SetHappiness(delimited[0], delimited[2], happiness);
	}

	return map;
}

//---------------------------------------------------------------------------------------

class OptimalSeatingFinder
{
public:
	OptimalSeatingFinder(const HappinessMap& inMap) : mMap(inMap), mBest(0) { }

	int					FindOptimalHappiness();

private:
	void				Start(const function<int (int)>& inTest);
	void				Recurse(const size_t inStart, const size_t inFrom, const size_t* inRemaining, const size_t inNumRemaining, const int inHappinessSoFar, const function<int (int)>& inTest);

	const HappinessMap&	mMap;
	int					mBest;
};

int	OptimalSeatingFinder::FindOptimalHappiness()
{
	mBest = INT_MIN;
	Start([this] (int inHappiness) { return max(mBest, inHappiness); });
	return mBest;
}

void OptimalSeatingFinder::Start(const function<int (int)>& inTest)
{
	size_t numRemaining = mMap.GetNumPeople() - 1;
	size_t* remaining = (size_t*) alloca(numRemaining * sizeof(size_t));

	for (size_t i = 0; i < mMap.GetNumPeople(); ++i)
	{
		size_t remIndex = 0;
		for (size_t n = 0; n < mMap.GetNumPeople(); ++n)
		{
			if (i != n)
			{
				remaining[remIndex] = n;
				++remIndex;
			}
		}

		Recurse(i, i, remaining, numRemaining, 0, inTest);
	}
}

void OptimalSeatingFinder::Recurse(const size_t inStart, const size_t inFrom, const size_t* inRemaining, const size_t inNumRemaining, const int inHappinessSoFar, const function<int (int)>& inTest)
{
	size_t numRemaining = inNumRemaining - 1;
	size_t* remaining = (numRemaining > 0) ? (size_t*) alloca(numRemaining * sizeof(size_t)) : nullptr;

	for (size_t i = 0; i < inNumRemaining; ++i)
	{
		size_t to = inRemaining[i];
		int happinessSoFar = inHappinessSoFar;
		happinessSoFar += mMap.GetHappiness(inFrom, to);
		happinessSoFar += mMap.GetHappiness(to, inFrom);

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

			Recurse(inStart, to, remaining, numRemaining, happinessSoFar, inTest);
		}
		else
		{
			happinessSoFar += mMap.GetHappiness(to, inStart);
			happinessSoFar += mMap.GetHappiness(inStart, to);
			mBest = inTest(happinessSoFar);
		}
	}
}

//---------------------------------------------------------------------------------------

int main()
{
	vector<string>	lines	= FileHelper::GetLinesInFile("input.txt");
	HappinessMap	map		= CreateMap(lines);

	map.PrintMap();

	int optimalHappiness1 = OptimalSeatingFinder(map).FindOptimalHappiness();
	cout << "Optimal Happiness 1 = " << optimalHappiness1 << endl;

	static const string kMe = "Me";
	for (size_t i = 0; i < map.GetNumPeople(); ++i)
	{
		string them = map.GetPerson(i);
		map.SetHappiness(kMe, them, 0);
	}

	for (size_t i = 0; i < map.GetNumPeople(); ++i)
	{
		string them = map.GetPerson(i);
		map.SetHappiness(them, kMe, 0);
	}

	map.PrintMap();
	
	int optimalHappiness2 = OptimalSeatingFinder(map).FindOptimalHappiness();
	cout << "Optimal Happiness 2 = " << optimalHappiness2 << endl;

	return 0;
}

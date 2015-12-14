#include <iostream>
#include <assert.h>
#include <algorithm>

#include "File.h"
#include "StringHelper.h"

using namespace std;

struct ReindeerStats
{
	int Speed;
	int SpeedTime;
	int RestTime;
};

class Reindeer
{
public:
	Reindeer(const ReindeerStats& inStats) : mStats(inStats), mTimeIntoCycle(0), mDistanceTravelled(0), mScore(0) { }

	void			Tick();
	int				GetDistanceTravelled() const { return mDistanceTravelled; }

	void			IncScore()			{ mScore++; }
	int				GetScore() const	{ return mScore; }

private:
	ReindeerStats	mStats;
	int				mTimeIntoCycle;
	int				mDistanceTravelled;
	int				mScore;
};

void Reindeer::Tick()
{
	if (mTimeIntoCycle < mStats.SpeedTime)
	{
		mDistanceTravelled += mStats.Speed;
	}

	++mTimeIntoCycle;
	
	const int cycleTime = mStats.SpeedTime + mStats.RestTime;
	if (mTimeIntoCycle == cycleTime)
	{
		mTimeIntoCycle = 0;
	}
}

vector<ReindeerStats> ParseLines(const vector<string>& inLines)
{
	vector<ReindeerStats> out;

	for (auto& line : inLines)
	{
		// Example:
		// Dancer can fly 37 km/s for 1 seconds, but then must rest for 36 seconds.\n
		// Reduced to:
		// Dancer 37 1 36
		string parsed = StringHelper::FindAndReplace(line, " can fly ", " ");
		parsed = StringHelper::FindAndReplace(parsed, " km/s for ", " ");
		parsed = StringHelper::FindAndReplace(parsed, " seconds, but then must rest for ", " ");
		parsed = StringHelper::FindAndReplace(parsed, " seconds.", " ");
		vector<string> pieces = StringHelper::Delimit(parsed, " \n");
		assert(pieces.size() == 4);

		ReindeerStats stats;
		stats.Speed = stoi(pieces[1]);
		stats.SpeedTime = stoi(pieces[2]);
		stats.RestTime = stoi(pieces[3]);
		out.push_back(stats);
	}

	return out;
}

int FurthestDistanceAfterTime(const vector<ReindeerStats>& inStats, const int inTotalRaceTime)
{
	int furthestDistance = 0;

	for (auto& stat : inStats)
	{
		int cycleTime = stat.SpeedTime + stat.RestTime;
		int totalCycles = inTotalRaceTime / cycleTime;
		int remainderTime = inTotalRaceTime % cycleTime;

		int distPerCycle = stat.SpeedTime * stat.Speed;
		int distance = totalCycles * distPerCycle;
		distance += (remainderTime < stat.SpeedTime) ? remainderTime * stat.Speed : distPerCycle;

		furthestDistance = max(furthestDistance, distance);
	}

	return furthestDistance;
}

int HighestScoreAfterTime(const vector<ReindeerStats>& inStats, const int inTotalRaceTime)
{
	vector<Reindeer> reindeer;

	for (auto& stat : inStats)
	{
		reindeer.emplace_back(stat);
	}

	int time = 0;
	while (time < inTotalRaceTime)
	{
		int furthestTravelled = 0;

		for (auto& deer : reindeer)
		{
			deer.Tick();
			furthestTravelled = max(furthestTravelled, deer.GetDistanceTravelled());
		}
		
		for (auto& deer : reindeer)
		{
			if (deer.GetDistanceTravelled() == furthestTravelled)
			{
				deer.IncScore();
			}
		}

		++time;
	}
	
	int highestScore = 0;
	for (auto& deer : reindeer)
	{
		highestScore = max(deer.GetScore(), highestScore);
	}

	return highestScore;
}

int main()
{
	vector<string> lines = FileHelper::GetLinesInFile("input.txt");
	vector<ReindeerStats> stats = ParseLines(lines);

	static const int kRaceTime = 2503;

	int furthestDistance	= FurthestDistanceAfterTime(stats, kRaceTime);
	int highestScore		= HighestScoreAfterTime(stats, kRaceTime);

	cout << "Furthest distance travelled = " << furthestDistance << endl;
	cout << "Highest Score = " << highestScore << endl;

	return 0;
}

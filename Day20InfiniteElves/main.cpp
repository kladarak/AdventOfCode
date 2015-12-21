#include <iostream>
#include <assert.h>
#include <unordered_set>
#include <vector>
#include <algorithm>

using namespace std;

long long TriNum(const long long inNum)
{
	return ((inNum * inNum) + inNum) / 2;
}

template<typename TFilter>
int FindHouseNum(const int inTarget, const int inNumPresentsPerHouse, const TFilter& inFilter)
{
	int testNum = 0;
	
	while (true)
	{
		testNum++;

		const int root = (int) sqrt((double) testNum);
		int highFactorSum = 0;
		int runningSum = 0;

		for (int i = 1; i <= root; ++i)
		{
			if (testNum % i == 0)
			{
				const int loFactor = i;
				const int hiFactor = testNum / i;

				const long long triNum = TriNum(hiFactor) * inNumPresentsPerHouse;
				const int triNumTest = inTarget - (highFactorSum * inNumPresentsPerHouse);
				assert(triNum > 0);

				if (triNum < triNumTest)
				{
					break;
				}

				highFactorSum += inFilter(hiFactor, testNum) ? hiFactor : 0;
				runningSum += inFilter(hiFactor, testNum) ? hiFactor : 0;
				runningSum += inFilter(loFactor, testNum) && (hiFactor != loFactor) ? loFactor : 0;
			}
		}

		if ((runningSum * inNumPresentsPerHouse) >= inTarget)
		{
			return testNum;
		}
	}
}

int main()
{
	static const int kTarget = 36000000;

	int houseNumTest1 = FindHouseNum(kTarget, 10, [] (int, int) { return true; });
	int houseNumTest2 = FindHouseNum(kTarget, 11, [] (int inFactor, int inHouseNum)
	{
		return (inFactor * 50) >= inHouseNum;
	});

	cout << "House num test 1: " << houseNumTest1 << endl;
	cout << "House num test 2: " << houseNumTest2 << endl;
	
	return 0;
}

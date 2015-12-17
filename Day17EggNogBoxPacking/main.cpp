#include <iostream>
#include <assert.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

vector<int> ParseContainers(const string& inInputString)
{
	vector<int> out;

	vector<string> words = StringHelper::Delimit(inInputString, "\n");
	for (auto& word : words)
	{
		out.push_back(stoi(word));
	}

	return out;
}

bool IsBitSet(unsigned int inVariation, size_t inBit)
{
	return (inVariation & (1u << inBit)) != 0;
}

int main()
{
	string inputString = FileHelper::ReadFileToString("input.txt");
	vector<int> containers = ParseContainers(inputString);

	static const int kTargetCapacity = 150;

	unsigned int variation = 0;
	int count = 0;
	vector<int> combinationsPerContainerCount;
	combinationsPerContainerCount.resize(containers.size());

	while (!IsBitSet(variation, containers.size()))
	{
		int capacity = 0;
		int containersUsedCount = 0;

		for (size_t i = 0; i < containers.size(); ++i)
		{
			if (IsBitSet(variation, i))
			{
				capacity += containers[i];
				++containersUsedCount;
			}
		}

		if (capacity == kTargetCapacity)
		{
			++count;
			combinationsPerContainerCount[containersUsedCount]++;
		}

		++variation;
	};

	cout << "Combination count = " << count << endl;

	for (int count : combinationsPerContainerCount)
	{
		if (count != 0)
		{
			cout << "Fewest number of containers combo count = " << count << endl;
			break;
		}
	}

	return 0;
}

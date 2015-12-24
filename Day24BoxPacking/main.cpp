#include <iostream>
#include <assert.h>
#include <algorithm>
#include <stdint.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

vector<int> ParseBoxes(const string& inList)
{
	vector<int> boxes;
	
	vector<string> boxesStr = StringHelper::Delimit(inList, " \n");
	for (auto& box : boxesStr)
	{
		boxes.push_back(stoi(box));
	}

	return boxes;
}

int Sum(const vector<int>& inBoxes)
{
	int sum = 0;
	for (int box : inBoxes)
	{
		sum += box;
	}
	return sum;
}

uint64_t CalculateQuantumEntanglement(const vector<int>& inBoxes)
{
	uint64_t qe = 1;
	for (int value : inBoxes)
	{
		qe *= value;
	}
	return qe;
}

struct BoxCombo
{
	vector<int> Boxes;
	int			Count	= 0;
	uint64_t	QE		= 0;
};

void BuildCombo(const vector<int>& inBoxes, const int inTargetWeight, const int inWeightSoFar, const size_t inListIndex, const BoxCombo inCombo, BoxCombo& ioBest)
{
	for (size_t i = inListIndex; i < inBoxes.size(); ++i)
	{
		int weight = inBoxes[i] + inWeightSoFar;
		if (weight > inTargetWeight)
		{
			continue;
		}

		BoxCombo combo = inCombo;
		combo.Boxes.push_back(inBoxes[i]);
		
		if (weight == inTargetWeight)
		{
			combo.Count = combo.Boxes.size();
			combo.QE = CalculateQuantumEntanglement(combo.Boxes);

			if (combo.QE < ioBest.QE)
			{
				ioBest = combo;
			}
		}
		else
		{
			if (combo.Boxes.size() < ioBest.Count)
			{
				BuildCombo(inBoxes, inTargetWeight, weight, i + 1, combo, ioBest);
			}
		}
	}
}

BoxCombo GetBestCombo(const vector<int>& inBoxes, int inTargetWeight)
{
	BoxCombo best;
	best.Count = INT_MAX;
	best.QE = ULLONG_MAX;

	BuildCombo(inBoxes, inTargetWeight, 0, 0, BoxCombo(), best);

	return best;
}

int main()
{
	vector<int> boxes = ParseBoxes(FileHelper::ReadFileToString("input.txt"));

	sort(boxes.begin(), boxes.end(), [] (int lhs, int rhs)
	{
		return lhs > rhs;
	});

	int totalWeight = Sum(boxes);
	int thirdWeight = totalWeight / 3;
	int quarterWeight = totalWeight / 4;
	
	BoxCombo best1 = GetBestCombo(boxes, thirdWeight);
	BoxCombo best2 = GetBestCombo(boxes, quarterWeight);

	cout << "QE of ideal combo 1 = " << best1.QE << endl;
	cout << "QE of ideal combo 2 = " << best2.QE << endl;

	return 0;
}

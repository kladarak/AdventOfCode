#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

#include "File.h"
#include "StringHelper.h"

using namespace std;

struct Vec3 
{
	int d[3];

	int MaxDim() const { return max(d[0], max(d[1], d[2])); }
	int Volume() const { return d[0] * d[1] * d[2]; }
	int SumAll() const { return d[0] + d[1] + d[2]; }
};

vector<Vec3> ParseLinesToVec3(const vector<string>& inLines)
{
	vector<Vec3> dims;

	for (auto& line : inLines)
	{
		vector<string> pieces = StringHelper::Delimit(line, "x\n");

		Vec3 dim;

		for (size_t i = 0; i < pieces.size(); ++i)
		{
			dim.d[i] = stoi(pieces[i]);
		}

		dims.push_back(dim);
	}

	return dims;
}

int CalculateRequiredGiftWrapArea(const Vec3& inDim)
{
	int smallestFaceArea = inDim.Volume() / inDim.MaxDim();

	return	(inDim.d[0] * inDim.d[1] * 2)
		+	(inDim.d[1] * inDim.d[2] * 2)
		+	(inDim.d[0] * inDim.d[2] * 2)
		+	smallestFaceArea;
}

int CalculateRibbonLength(const Vec3& inDim)
{
	int smallestFacePerimeter = (inDim.SumAll() - inDim.MaxDim()) * 2;
	int volume = inDim.Volume();

	return volume + smallestFacePerimeter;
}

int main()
{
	vector<string>	lines		= FileHelper::GetLinesInFile("input.txt");
	vector<Vec3>	dimensions	= ParseLinesToVec3(lines);

	int requiredArea = 0;
	int ribbonLength = 0;

	for (auto& dim : dimensions)
	{
		requiredArea += CalculateRequiredGiftWrapArea(dim);
		ribbonLength += CalculateRibbonLength(dim);
	}

	cout << "Total required gift wrap area: " << requiredArea << endl;
	cout << "Total length of ribbon required: " << ribbonLength << endl;

	return 0;
}


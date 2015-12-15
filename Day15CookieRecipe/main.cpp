#include <iostream>
#include <algorithm>
#include <assert.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

enum EProperty
{
	Capacity,
	Durability,
	Flavour,
	Texture,
	CountWithoutCalories,

	Calories = CountWithoutCalories,

	Count,
};

struct Ingredient
{
	int Properties[EProperty::Count];
};

Ingredient ParseIngredient(const string& inLine)
{
	auto parsed = inLine;
	parsed = StringHelper::FindAndReplace(parsed, "capacity",	"");
	parsed = StringHelper::FindAndReplace(parsed, "durability", "");
	parsed = StringHelper::FindAndReplace(parsed, "flavor",		"");
	parsed = StringHelper::FindAndReplace(parsed, "texture",	"");
	parsed = StringHelper::FindAndReplace(parsed, "calories",	"");

	vector<string> pieces = StringHelper::Delimit(parsed, ": ,\n");
	assert(pieces.size() == 6);

	Ingredient out;
	out.Properties[EProperty::Capacity]		= stoi(pieces[1]);
	out.Properties[EProperty::Durability]	= stoi(pieces[2]);
	out.Properties[EProperty::Flavour]		= stoi(pieces[3]);
	out.Properties[EProperty::Texture]		= stoi(pieces[4]);
	out.Properties[EProperty::Calories]		= stoi(pieces[5]);

	return out;
}

vector<Ingredient> ParseIngredients(const vector<string>& inLines)
{
	vector<Ingredient> out;
	for (auto& line : inLines)
	{
		out.push_back(ParseIngredient(line));
	}
	return out;
}

class Mixture
{
public:
	Mixture(size_t inIngredientCount, int inTotalAmount);

	bool		Increment();
	int			GetAmount(size_t inIngredientIndex) const { return Amounts[inIngredientIndex]; }
	void		Print() const;

private:
	bool		TransferOne(size_t inIndex);

	vector<int> Amounts;
};

Mixture::Mixture(size_t inIngredientCount, int inTotalAmount)
{
	assert(inIngredientCount > 0);
	Amounts.resize(inIngredientCount, 0);
	Amounts.front() = inTotalAmount;
}

bool Mixture::Increment()
{
	return TransferOne(0);
}

bool Mixture::TransferOne(size_t inIndex)
{
	if ((inIndex + 1) == Amounts.size())
	{
		return false; // We've hit the last value - cannot carry over any more.
	}

	if (Amounts[inIndex] != 0)
	{
		Amounts[inIndex]--;
		Amounts[inIndex + 1]++;
	}
	else if (TransferOne(inIndex + 1))
	{
		Amounts[inIndex] = Amounts[inIndex + 1];
		Amounts[inIndex + 1] = 0;
	}
	else
	{
		return false;
	}

	return true;
}

void Mixture::Print() const
{
	for (auto amount : Amounts)
	{
		cout << amount << " ";
	}

	cout << endl;
}

int CalculateScore(const Mixture& inMixture, const vector<Ingredient>& inIngredients)
{
	int totalScore = 1;

	for (size_t p = 0; p < EProperty::CountWithoutCalories; ++p)
	{
		int propertyScore = 0;
		for (size_t i = 0; i < inIngredients.size(); ++i)
		{
			propertyScore += inIngredients[i].Properties[p] * inMixture.GetAmount(i);
		}
			
		if (propertyScore > 0)
		{
			totalScore *= propertyScore;
		}
		else
		{
			// Abort, no score for this recipe.
			return 0;
		}
	}

	return totalScore;
}

int CalculateCalories(const Mixture& inMixture, const vector<Ingredient>& inIngredients)
{
	int calories = 0;
	for (size_t i = 0; i < inIngredients.size(); ++i)
	{
		calories += inIngredients[i].Properties[EProperty::Calories] * inMixture.GetAmount(i);
	}
	return calories;
}

int CalculateBestScoreIgnoringCalories(const vector<Ingredient>& inIngredients, const int inTeaspoons)
{
	Mixture mixture(inIngredients.size(), inTeaspoons);

	int bestScore = 0;
	do
	{
		bestScore = max(bestScore, CalculateScore(mixture, inIngredients));
	} while (mixture.Increment());

	return bestScore;
}

int CalculateBestScoreWithCaloryCount(const vector<Ingredient>& inIngredients, const int inTeaspoons, const int inCalories)
{
	Mixture mixture(inIngredients.size(), inTeaspoons);

	int bestScore = 0;
	do
	{
		if (CalculateCalories(mixture, inIngredients) == inCalories)
		{
			bestScore = max(bestScore, CalculateScore(mixture, inIngredients));
		}
	} while (mixture.Increment());

	return bestScore;
}

int main()
{
	vector<string>		lines		= FileHelper::GetLinesInFile("input.txt");
	vector<Ingredient>	ingredients = ParseIngredients(lines);

	const int kTeaspoons	= 100;
	const int kCalories		= 500;

	int bestScoreIgnoringCalories	= CalculateBestScoreIgnoringCalories(ingredients, kTeaspoons);
	int bestScoreWithCalories		= CalculateBestScoreWithCaloryCount(ingredients, kTeaspoons, kCalories);

	cout << "Best score ignoring calorie count = " << bestScoreIgnoringCalories << endl;
	cout << "Best score with 500 calories = " << bestScoreWithCalories << endl;

	return 0;
}

#include <iostream>
#include <assert.h>
#include <functional>

#include "File.h"
#include "StringHelper.h"

using namespace std;

enum ECompound
{
	Children,
	Cats,
	Samoyeds,
	Pomeranians,
	Akitas,
	Vizslas,
	Goldfish,
	Trees,
	Cars,
	Perfumes,

	Count,
};

struct CompoundNameMapping
{
	string		Name;
	ECompound	Compound;
};

static const CompoundNameMapping kCompoundNameMap[] =
{
	{ "children",		Children	},
	{ "cats",			Cats		},
	{ "samoyeds",		Samoyeds	},
	{ "pomeranians",	Pomeranians	},
	{ "akitas",			Akitas		},
	{ "vizslas",		Vizslas		},
	{ "goldfish",		Goldfish	},
	{ "trees",			Trees		},
	{ "cars",			Cars		},
	{ "perfumes",		Perfumes	},
};
static const int kCompoundNameMapCount = sizeof(kCompoundNameMap)/sizeof(CompoundNameMapping);

struct AuntSue
{
	static const int kUnknownCount = -1;

	int SueNumber;
	int Compounds[ECompound::Count];

	AuntSue(int inNumber)
		: SueNumber(inNumber)
	{
		for (int i = 0; i < ECompound::Count; ++i)
		{
			Compounds[i] = kUnknownCount;
		}
	}
};

AuntSue ParseAuntSue(const string& inLine)
{
	AuntSue sue(-1);

	vector<string> compoundPieces = StringHelper::Delimit(inLine, ",\n");
	if (compoundPieces[0].find("Sue") != string::npos)
	{
		vector<string> words = StringHelper::Delimit(compoundPieces[0], " :");
		sue.SueNumber = stoi(words[1]);
	}

	for (auto& compoundPiece : compoundPieces)
	{
		for (int i = 0; i < kCompoundNameMapCount; ++i)
		{
			auto& nameMapping = kCompoundNameMap[i];
			if (compoundPiece.find(nameMapping.Name) != string::npos)
			{
				vector<string> words = StringHelper::Delimit(compoundPiece, " :");
				sue.Compounds[nameMapping.Compound] = stoi(words.back());
				break;
			}
		}
	}

	return sue;
}

vector<AuntSue> ParseAllAuntSues(const vector<string>& inAuntSueDescriptions)
{
	vector<AuntSue> sues;

	for (auto& line : inAuntSueDescriptions)
	{
		sues.push_back(ParseAuntSue(line));
	}

	return sues;
}

typedef function<bool(const AuntSue&, const AuntSue&, ECompound)> AuntSueComparator;

AuntSue FindAuntSue(const vector<AuntSue>& inAllSues, const AuntSue& inSueToFind, const vector<AuntSueComparator>& inComparators)
{
	vector<AuntSue> remainingSues = inAllSues;

	for (int i = 0; i < ECompound::Count; ++i)
	{
		vector<AuntSue> filteredSues;

		for (auto& sue : remainingSues)
		{
			if (inComparators[i](sue, inSueToFind, (ECompound) i))
			{
				filteredSues.push_back(sue);
			}
		}

		remainingSues = filteredSues;
	}

	assert(remainingSues.size() == 1);
	return remainingSues[0];
}

static const vector<AuntSueComparator> kTest1Comparators = [] ()
{
	vector<AuntSueComparator> out;
	
	for (int i = 0; i < ECompound::Count; ++i)
	{
		out.push_back( [] (const AuntSue& inToTest, const AuntSue& inToFind, ECompound inCompound)
		{
			return (inToTest.Compounds[inCompound] == inToFind.Compounds[inCompound])
				|| (inToTest.Compounds[inCompound] == AuntSue::kUnknownCount);
		} );
	}

	return out;
} ();

static const vector<AuntSueComparator> kTest2Comparators = [] ()
{
	vector<AuntSueComparator> out;
	
	for (int i = 0; i < ECompound::Count; ++i)
	{
		switch (i)
		{
			case ECompound::Cats:
			case ECompound::Trees:
				out.push_back( [] (const AuntSue& inToTest, const AuntSue& inToFind, ECompound inCompound)
				{
					return (inToTest.Compounds[inCompound] > inToFind.Compounds[inCompound])
						|| (inToTest.Compounds[inCompound] == AuntSue::kUnknownCount);
				} );
				break;
				
			case ECompound::Pomeranians:
			case ECompound::Goldfish:
				out.push_back( [] (const AuntSue& inToTest, const AuntSue& inToFind, ECompound inCompound)
				{
					return (inToTest.Compounds[inCompound] < inToFind.Compounds[inCompound])
						|| (inToTest.Compounds[inCompound] == AuntSue::kUnknownCount);
				} );
				break;

			default:
				out.push_back( [] (const AuntSue& inToTest, const AuntSue& inToFind, ECompound inCompound)
				{
					return (inToTest.Compounds[inCompound] == inToFind.Compounds[inCompound])
						|| (inToTest.Compounds[inCompound] == AuntSue::kUnknownCount);
				} );
				break;
		}
	}

	return out;
} ();

int main()
{
	vector<string> lines = FileHelper::GetLinesInFile("input.txt");
	vector<AuntSue> sues = ParseAllAuntSues(lines);

	string tickerTapeDescription	= FileHelper::ReadFileToString("tickerTape.txt");
	AuntSue sueToFind				= ParseAuntSue(tickerTapeDescription);

	AuntSue sueFound1 = FindAuntSue(sues, sueToFind, kTest1Comparators);
	AuntSue sueFound2 = FindAuntSue(sues, sueToFind, kTest2Comparators);
	
	cout << "Sue who got the gift (test 1): " << sueFound1.SueNumber << endl;
	cout << "Sue who got the gift (test 2): " << sueFound2.SueNumber << endl;

	return 0;
}

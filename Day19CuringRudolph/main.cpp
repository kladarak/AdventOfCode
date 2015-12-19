#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
#include <assert.h>
#include <unordered_set>
#include <queue>

#include "File.h"
#include "StringHelper.h"

using namespace std;

struct Transition
{
	string	mFrom;
	string	mTo;
	int		mDelta;
};

struct Molecule
{
	string	mMolecule;
	int		mElementCount;
	int		mTransitionCount;

	bool operator==(const Molecule& inRHS) const
	{
		return mMolecule == inRHS.mMolecule
			&& mElementCount == inRHS.mElementCount
			&& mTransitionCount == inRHS.mTransitionCount;
	}
};

template<>
struct hash<Molecule>
{
	size_t operator()(const Molecule& inM)
	{
		return hash<string>()(inM.mMolecule);
	}
};

int ElementCount(const string& inStr)
{
	int sum = 0;
	for (char c : inStr)
	{
		if (c >= 'A' && c <= 'Z')
		{
			sum++;
		}
	}
	return sum;
}

vector<Transition> ParseTransitions(const vector<string>& inLines)
{
	vector<Transition> out;

	for (auto line : inLines)
	{
		if (line.find("=>") != string::npos)
		{
			line = StringHelper::FindAndReplace(line, " => ", " ");
			vector<string> pieces = StringHelper::Delimit(line, " \n");
			assert(pieces.size() == 2);

			Transition t;
			t.mFrom		= pieces[0];
			t.mTo		= pieces[1];
			t.mDelta	= ElementCount(pieces[1]) - 1;
			out.push_back(t);
		}
	}

	return out;
}

Molecule ParseTargetMolecule(const vector<string>& inLines)
{
	Molecule out;
	out.mMolecule			= inLines.back();
	out.mElementCount		= ElementCount(out.mMolecule);
	out.mTransitionCount	= 0;
	return out;
}

void Test1(const Molecule& inBaseMolecule, const vector<Transition>& inTransitions)
{
	unordered_set<Molecule> newMolecules;

	for (auto& transition : inTransitions)
	{
		size_t pos = inBaseMolecule.mMolecule.find(transition.mFrom);
		while (pos != string::npos)
		{
			Molecule m = inBaseMolecule;
			m.mMolecule.replace(pos, transition.mFrom.size(), transition.mTo);
			m.mTransitionCount++;
			m.mElementCount += transition.mDelta;
			newMolecules.insert(m);

			pos = inBaseMolecule.mMolecule.find(transition.mFrom, pos + 1);
		}
	}
	
	cout << "Unique new molecules = " << newMolecules.size() << endl;
}

void Test2(const Molecule& inTarget, const vector<Transition>& inTransitions)
{
	struct Comparison
	{
		bool operator()(const Molecule& inLHS, const Molecule& inRHS) const
		{
			return (inLHS.mElementCount > inRHS.mElementCount)
				|| ((inLHS.mElementCount == inRHS.mElementCount)
					&& (inLHS.mTransitionCount > inRHS.mTransitionCount));
		}
	};

	priority_queue<Molecule, vector<Molecule>, Comparison> molecules;
	molecules.push(inTarget);

	int steps = 0;
	while (steps == 0)
	{
		Molecule top = molecules.top();
		molecules.pop();

		unordered_set<Molecule> newMolecules;
		for (auto& transition : inTransitions)
		{
			size_t pos = top.mMolecule.find(transition.mTo);
			while (pos != string::npos)
			{
				Molecule m = top;
				m.mMolecule.replace(pos, transition.mTo.size(), transition.mFrom);
				m.mTransitionCount++;
				m.mElementCount -= transition.mDelta;
				newMolecules.insert(m);

				pos = top.mMolecule.find(transition.mTo, pos + 1);
			}
		}

		for (auto& m : newMolecules)
		{
			if (m.mMolecule == "e")
			{
				steps = m.mTransitionCount;
				break;
			}
			else
			{
				molecules.push(m);
			}
		}
	}

	cout << "Num steps from e to target molecule = " << steps << endl;
}

int main()
{
	const vector<string>		lines		= FileHelper::GetLinesInFile("input.txt");
	const vector<Transition>	transitions = ParseTransitions(lines);
	const Molecule				target		= ParseTargetMolecule(lines);

	Test1(target, transitions);
	Test2(target, transitions);

	return 0;
}


#include <iostream>
#include <functional>
#include <unordered_map>
#include <assert.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

typedef unsigned short uint16;
class Wire;
typedef unordered_map<string, Wire> WireMap;
typedef function<uint16 (const WireMap&)> EvaluateFunc;

class Wire
{
public:
	Wire()								: mEvaluateFunc([] (const WireMap&) { assert(false); return 0; }),	mResultCache(0), mEvaluated(false) { }
	Wire(const EvaluateFunc& inFunc)	: mEvaluateFunc(inFunc),											mResultCache(0), mEvaluated(false) { }

	void	SetEvaluateFunction(const EvaluateFunc& inFunc) { mEvaluateFunc = inFunc; }
	void	Reset()											{ mEvaluated = false; }

	uint16	Evaluate(const WireMap& inWires) const
	{
		if (!mEvaluated)
		{
			mResultCache = mEvaluateFunc(inWires);
			mEvaluated = true;
		}

		return mResultCache;
	}

private:
	EvaluateFunc	mEvaluateFunc;
	mutable uint16	mResultCache;
	mutable bool	mEvaluated;
};

const Wire& FindWire(const WireMap& inWires, const string& inKey)
{
	auto iter = inWires.find(inKey);
	assert(iter != inWires.end());
	return iter->second;
}

uint16 EvaluateWire(const WireMap& inWires, const string& inKey)
{
	return FindWire(inWires, inKey).Evaluate(inWires);
}

//--------------------------------------------------------------------------------------------------------------------

EvaluateFunc CreateAND(const string& inLHS, const string& inRHS)
{
	return [inLHS, inRHS] (const WireMap& inWires) { return EvaluateWire(inWires, inLHS) & EvaluateWire(inWires, inRHS); };
}

EvaluateFunc CreateAND(uint16 inLHS, const string& inRHS)
{
	return [inLHS, inRHS] (const WireMap& inWires) { return inLHS & EvaluateWire(inWires, inRHS); };
}

EvaluateFunc CreateOR(const string& inLHS, const string& inRHS)
{
	return [inLHS, inRHS] (const WireMap& inWires) { return EvaluateWire(inWires, inLHS) | EvaluateWire(inWires, inRHS); };
}

EvaluateFunc CreateOR(uint16 inLHS, const string& inRHS)
{
	return [inLHS, inRHS] (const WireMap& inWires) { return inLHS | EvaluateWire(inWires, inRHS); };
}

EvaluateFunc CreateNOT(const string& inLHS)
{
	return [inLHS] (const WireMap& inWires) { return ~EvaluateWire(inWires, inLHS); };
}

EvaluateFunc CreateLEFTSHIFT(const string& inLHS, int inAmount)
{
	return [inLHS, inAmount] (const WireMap& inWires) { return EvaluateWire(inWires, inLHS) << inAmount; };
}

EvaluateFunc CreateRIGHTSHIFT(const string& inLHS, int inAmount)
{
	return [inLHS, inAmount] (const WireMap& inWires) { return EvaluateWire(inWires, inLHS) >> inAmount; };
}

EvaluateFunc CreateNOOP(const string& inLHS)
{
	return [inLHS] (const WireMap& inWires) { return EvaluateWire(inWires, inLHS); };
}

EvaluateFunc CreateNOOP(uint16 inValue)
{
	return [inValue] (const WireMap&) { return inValue; };
}

//--------------------------------------------------------------------------------------------------------------------

namespace OperatorName
{
	static const string AND		= "AND";
	static const string OR		= "OR";
	static const string NOT		= "NOT";
	static const string LSHIFT	= "LSHIFT";
	static const string RSHIFT	= "RSHIFT";
}

static const string WHITESPACE = " \t\n\r";

bool IsNumeric(const string& inString)
{
	for (auto c : inString)
	{
		if (c < '0' || c > '9')
		{
			return false;
		}
	}

	return true;
}

EvaluateFunc ParseAND(const string& inString)
{
	string keysString	= StringHelper::FindAndReplace(inString, OperatorName::AND, "");
	vector<string> keys = StringHelper::Delimit(keysString, WHITESPACE);
	assert(keys.size() == 2);
	return IsNumeric(keys[0]) ? CreateAND((uint16) stoi(keys[0]), keys[1]) : CreateAND(keys[0], keys[1]);
}

EvaluateFunc ParseOR(const string& inString)
{
	string keysString	= StringHelper::FindAndReplace(inString, OperatorName::OR, "");
	vector<string> keys = StringHelper::Delimit(keysString, WHITESPACE);
	assert(keys.size() == 2);
	return IsNumeric(keys[0]) ? CreateOR((uint16) stoi(keys[0]), keys[1]) : CreateOR(keys[0], keys[1]);
}

EvaluateFunc ParseNOT(const string& inString)
{
	string keysString	= StringHelper::FindAndReplace(inString, OperatorName::NOT, "");
	vector<string> keys = StringHelper::Delimit(keysString, WHITESPACE);
	assert(keys.size() == 1);
	return CreateNOT(keys[0]);
}

EvaluateFunc ParseLSHIFT(const string& inString)
{
	string keysString	= StringHelper::FindAndReplace(inString, OperatorName::LSHIFT, "");
	vector<string> keys = StringHelper::Delimit(keysString, WHITESPACE);
	assert(keys.size() == 2);
	int shiftAmount = stoi(keys[1]);
	return CreateLEFTSHIFT(keys[0], shiftAmount);
}

EvaluateFunc ParseRSHIFT(const string& inString)
{
	string keysString	= StringHelper::FindAndReplace(inString, OperatorName::RSHIFT, "");
	vector<string> keys = StringHelper::Delimit(keysString, WHITESPACE);
	assert(keys.size() == 2);
	int shiftAmount = stoi(keys[1]);
	return CreateRIGHTSHIFT(keys[0], shiftAmount);
}

EvaluateFunc ParseNOOP(const string& inString)
{
	vector<string> keys = StringHelper::Delimit(inString, WHITESPACE);
	assert(keys.size() == 1);
	return IsNumeric(keys[0]) ? CreateNOOP((uint16) stoi(keys[0])) : CreateNOOP(keys[0]);
}

//--------------------------------------------------------------------------------------------------------------------

EvaluateFunc ParseOperation(const string& inString)
{
	auto isOperation = [&inString] (const string& inOperationName)
	{
		return inString.find(inOperationName) != string::npos;
	};

	if (isOperation(OperatorName::AND))		return ParseAND(inString);
	if (isOperation(OperatorName::OR))		return ParseOR(inString);
	if (isOperation(OperatorName::NOT))		return ParseNOT(inString);
	if (isOperation(OperatorName::LSHIFT))	return ParseLSHIFT(inString);
	if (isOperation(OperatorName::RSHIFT))	return ParseRSHIFT(inString);
	
	return ParseNOOP(inString);
}

//--------------------------------------------------------------------------------------------------------------------

void ParseWire(const string& inString, WireMap& ioWires)
{
	static const string kEquals = " -> ";
	size_t equalsIndex = inString.find(kEquals);
	assert(equalsIndex != string::npos);

	string wireKey = inString.substr(equalsIndex + kEquals.size());
	vector<string> wireKeyPieces = StringHelper::Delimit(wireKey, WHITESPACE);
	assert(wireKeyPieces.size() == 1);
	wireKey = wireKeyPieces[0];

	string operation = inString.substr(0, equalsIndex);

	ioWires[wireKey] = Wire(ParseOperation(operation));
}

WireMap CreateWires(const vector<string>& inLines)
{
	WireMap wires;

	for (auto& line : inLines)
	{
		ParseWire(line, wires);
	}

	return wires;
}

//--------------------------------------------------------------------------------------------------------------------

int main()
{
	vector<string>	lines = FileHelper::GetLinesInFile("input.txt");
	WireMap			wires = CreateWires(lines);

	uint16 signalAt_a = EvaluateWire(wires, "a");
	cout << "Signal at wire a = " << signalAt_a << endl;

	for (auto& wire : wires)
	{
		wire.second.Reset();
	}

	wires["b"] = CreateNOOP(signalAt_a);

	uint16 newSignalAt_a = EvaluateWire(wires, "a");
	cout << "Signal at wire a after setting b to previous value of a = " << newSignalAt_a << endl;

	return 0;
}

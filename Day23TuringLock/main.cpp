#include <iostream>
#include <assert.h>
#include <functional>
#include <stdint.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

namespace InstructionSet
{
	void Half(uint64_t& r, uint64_t& c)					{ r /= 2; c++; }
	void Triple(uint64_t& r, uint64_t& c)				{ r *= 3; c++; }
	void Inc(uint64_t& r, uint64_t& c)					{ r++; c++; }
	void Jmp(uint64_t& c, uint64_t o)					{ c += o; }
	void JmpIfEven(uint64_t r, uint64_t& c, uint64_t o)	{ c += ((r % 2) == 0) ? o : 1; }
	void JmpIfOne(uint64_t r, uint64_t& c, uint64_t o)	{ c += (r == 1) ? o : 1; }
}

struct Registers
{
	union
	{
		uint64_t r[2];
		struct
		{
			uint64_t a;
			uint64_t b;
		};
	};

	uint64_t c;
};

typedef function<void (Registers&)> Instruction;

int ParseRegister(const string& inArg)
{
	if (inArg == "a") return 0;
	if (inArg == "b") return 1;
	assert(false);
	return 0;
}

Instruction ParseHalf(const string& inArg)
{
	int reg = ParseRegister(inArg);
	return [reg] (Registers& inR) { InstructionSet::Half(inR.r[reg], inR.c); };
}

Instruction ParseTriple(const string& inArg)
{
	int reg = ParseRegister(inArg);
	return [reg] (Registers& inR) { InstructionSet::Triple(inR.r[reg], inR.c); };
}

Instruction ParseInc(const string& inArg)
{
	int reg = ParseRegister(inArg);
	return [reg] (Registers& inR) { InstructionSet::Inc(inR.r[reg], inR.c); };
}

Instruction ParseJmp(const string& inArg)
{
	int offset = stoi(inArg);
	return [offset] (Registers& inR) { InstructionSet::Jmp(inR.c, offset); };
}

Instruction ParseJmpIfEven(const string& inArg0, const string& inArg1)
{
	int reg = ParseRegister(inArg0);
	int offset = stoi(inArg1);
	return [reg, offset] (Registers& inR) { InstructionSet::JmpIfEven(inR.r[reg], inR.c, offset); };
}

Instruction ParseJmpIfOne(const string& inArg0, const string& inArg1)
{
	int reg = ParseRegister(inArg0);
	int offset = stoi(inArg1);
	return [reg, offset] (Registers& inR) { InstructionSet::JmpIfOne(inR.r[reg], inR.c, offset); };
}

vector<Instruction> Compile(const vector<string>& inLines)
{
	vector<Instruction> out;

	for (auto& line : inLines)
	{
		auto pieces = StringHelper::Delimit(line, " ,\n");
		auto inst = pieces[0];

			 if (inst == "hlf") out.push_back( ParseHalf(pieces[1]) );
		else if (inst == "tpl") out.push_back( ParseTriple(pieces[1]) );
		else if (inst == "inc") out.push_back( ParseInc(pieces[1]) );
		else if (inst == "jmp") out.push_back( ParseJmp(pieces[1]) );
		else if (inst == "jie") out.push_back( ParseJmpIfEven(pieces[1], pieces[2]) );
		else if (inst == "jio") out.push_back( ParseJmpIfOne(pieces[1], pieces[2]) );
		else assert(false);
	}

	return out;
}

uint64_t Execute(const vector<Instruction> inProgram, Registers inRegisters)
{
	Registers& registers = inRegisters;

	while (registers.c >= 0 && registers.c < (uint64_t) inProgram.size())
	{
		inProgram[(int) registers.c](registers);
	}

	return registers.b;
}

int main()
{
	vector<Instruction> program = Compile(FileHelper::GetLinesInFile("input.txt"));
	Registers registers;

	registers.a = 0;
	registers.b = 0;
	registers.c = 0;
	uint64_t result1 = Execute(program, registers);

	registers.a = 1;
	registers.b = 0;
	registers.c = 0;
	uint64_t result2 = Execute(program, registers);

	cout << "Register b test 1 = " << result1 << endl;
	cout << "Register b test 2 = " << result2 << endl;

	return 0;
}

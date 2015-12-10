#include <iostream>
#include <string>

using namespace std;

string PlayGame(const string& inInput)
{
	char lastC = 0;
	size_t count = 0;
	string out;

	auto commit = [&] 
	{
		if (count > 0)
		{
			out += '0' + count;
			out += lastC;
		}
	};

	for (char c : inInput)
	{
		if (c != lastC)
		{
			commit();
			lastC = c;
			count = 0;
		}

		++count;
	}
	
	commit();

	return out;
}

string PlayGame(const string& inStart, int inIterationCount)
{
	string output = inStart;
	
	for (int i = 0; i < inIterationCount; ++i)
	{
		output = PlayGame(output);
	}

	return output;
}

int main()
{
	static const string kInput = "1321131112";

	string test1Output = PlayGame(kInput, 40);
	string test2Output = PlayGame(kInput, 50);


	cout << "Length of final string test 1 = " << test1Output.size() << endl;
	cout << "Length of final string test 2 = " << test2Output.size() << endl;

	return 0;
}
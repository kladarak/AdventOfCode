#include <iostream>

#include "File.h"

using namespace std;

int main()
{
	string input = FileHelper::ReadFileToString("input.txt");
	
	string buffer;
	int sum = 0;

	auto commit = [&] ()
	{
		if (buffer.size() > 0)
		{
			sum += stoi(buffer);
			buffer.clear();
		}
	};

	for (size_t i = 0; i < input.size(); ++i)
	{
		if (input[i] >= '0' && input[i] <= '9')
		{
			if (buffer.size() == 0)
			{
				if (input[i-1] == '-')
				{
					buffer += '-';
				}
			}

			buffer += input[i];
		}
		else
		{
			commit();
		}
	}

	commit();

	cout << "Number count = " << sum << endl;

	return 0;
}
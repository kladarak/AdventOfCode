#include <iostream>
#include <assert.h>
#include <stdint.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

uint64_t FindNext(uint64_t inCurrent)
{
	return (inCurrent * 252533) % 33554393;
}

int main()
{
	cout << "Hello World." << endl;

	static const uint64_t kStart = 20151125;
	static const uint64_t kColumn = 3019;
	static const uint64_t kRow = 3010;

	int row = 1;
	int col = 1;
	uint64_t code = kStart;

	while (!(row == kRow && col == kColumn))
	{
		code = FindNext(code);

		col++;
		row--;
		if (row == 0)
		{
			row = col;
			col = 1;
		}
	}

	cout << "New code = " << code << endl;

	while (true);

	return 0;
}

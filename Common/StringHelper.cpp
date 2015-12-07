#include "StringHelper.h"

using namespace std;

namespace StringHelper
{
	vector<string> Delimit(const string& inString, const string& inDelimiters)
	{
		vector<string> result;

		string buffer;

		auto commitBuffer = [&buffer, &result] ()
		{
			if (buffer.size() > 0)
			{
				result.push_back(buffer);
				buffer.clear();
			}
		};

		for (char c : inString)
		{
			if (inDelimiters.find(c) == string::npos)
			{
				buffer += c;
			}
			else
			{
				commitBuffer();
			}
		}
		
		commitBuffer();

		return result;
	}

	string FindAndReplace(const string& inString, const string& inFind, const string& inReplace)
	{
		string out = inString;

		size_t pos = inString.find(inFind);
		if (pos != string::npos)
		{
			out.replace(pos, inFind.size(), inReplace);
		}

		return out;
	}
}
#include "File.h"

using namespace std;

namespace FileHelper
{
	vector<string> GetLinesInFile(const string& inFilename)
	{
		vector<string> lines;

		FILE* pFile = nullptr;
		fopen_s(&pFile, inFilename.c_str(), "r");

		if (pFile != nullptr)
		{
			char buffer[1024];

			while (fgets(buffer, 1024, pFile) != nullptr)
			{
				string line = buffer;
				lines.push_back(line);
			}

			fclose(pFile);
		}

		return lines;
	}
	
	string ReadFileToString(const string& inFilename)
	{
		string out;
		
		FILE* pFile;
		fopen_s(&pFile, inFilename.c_str(), "r");
		
		if (pFile)
		{
			fseek(pFile , 0 , SEEK_END);
			size_t bytes = ftell(pFile);
			rewind(pFile);
			
			if (bytes > 0)
			{
				out.resize(bytes);
			
				/*size_t bytesRead =*/ fread(&out[0], 1, bytes, pFile);
			}
			
			fclose(pFile);
		}
		
		return out;
	}
	
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
}
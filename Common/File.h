#pragma once

#include <string>
#include <vector>

namespace FileHelper
{
	std::vector<std::string> 	GetLinesInFile(const std::string& inFilename);
	std::string 				ReadFileToString(const std::string& inFilename);
	
	std::vector<std::string> 	Delimit(const std::string& inString, const std::string& inDelimiters);
}

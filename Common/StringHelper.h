#pragma once

#include <string>
#include <vector>

namespace StringHelper
{
	std::vector<std::string>	Delimit(const std::string& inString, const std::string& inDelimiters);
	std::string					FindAndReplace(const std::string& inString, const std::string& inFind, const std::string& inReplace);
}

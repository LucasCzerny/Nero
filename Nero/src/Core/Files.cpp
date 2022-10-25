#include "Files.h"

#include "Assert.h"

namespace Nero { namespace Util
{
	std::string ReadFile(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		std::ifstream inputFile(filePath);

		if (!inputFile.is_open())
			Assert::Unreachable("Could not open file " + filePath.string());

		std::string output;
		std::string line;

		while (std::getline(inputFile, line))
			output += line + '\n';

		return output;
	}
} }
#include "Assert.h"

#include "Log.h"

namespace Nero
{
	void Assert::That(bool condition)
	{
		if (!condition)
		{
			Log::Error("Assertion failed.");
			__debugbreak();
		}
	}

	void Assert::That(bool condition, const std::string& output)
	{
		if (!condition)
		{
			Log::Error("Assertion failed: " + output);
			__debugbreak();
		}
	}

	void Assert::Unreachable(const std::string& output)
	{
		Log::Error("Assertion failed: This line was supposed to be unreachable. " + output);
		__debugbreak();
	}
}
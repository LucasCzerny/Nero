#include "Log.h"

namespace Nero
{
	LogLevel Log::s_LogLevel;

	void Log::SetLogLevel(const LogLevel& logLevel)
	{
		s_LogLevel = logLevel;
	}
}
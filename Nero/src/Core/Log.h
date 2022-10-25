#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	enum class LogLevel
	{
		INFO, WARNING, ERROR
	};

	class Log
	{
	private:
		static LogLevel s_LogLevel;

	public:
		static void SetLogLevel(const LogLevel& logLevel);

		template <typename T>
		static void Info(const T& message)
		{
			if (s_LogLevel > LogLevel::INFO)
				return;

			GenericLog("INFO", message);
		}

		template <typename T>
		static void Warning(const T& message)
		{
			if (s_LogLevel > LogLevel::WARNING)
				return;

			GenericLog("WARNING", message);
		}

		template <typename T>
		static void Error(const T& message)
		{
			if (s_LogLevel > LogLevel::ERROR)
				return;

			GenericLog("ERROR", message);
		}

	private:
		template <typename T>
		static void GenericLog(const char* logLevel, T message)
		{
			std::cout << "[" << logLevel << "] " << std::to_string(message) << "\n";
		}

		template <>
		static void GenericLog(const char* logLevel, std::string message)
		{
			std::cout << "[" << logLevel << "] " << message << "\n";
		}

		template <>
		static void GenericLog(const char* logLevel, const std::string& message)
		{
			std::cout << "[" << logLevel << "] " << message << "\n";
		}

		template <>
		static void GenericLog(const char* logLevel, const char* message)
		{
			std::cout << "[" << logLevel << "] " << message << "\n";
		}
	};
}

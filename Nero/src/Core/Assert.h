#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	class Assert
	{
	public:
		static void That(bool condition);
		static void That(bool condition, const std::string& output);
		
		static void Unreachable(const std::string& output);
	};
}
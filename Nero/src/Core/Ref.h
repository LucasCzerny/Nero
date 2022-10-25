#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename ... Args>
	Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	Scope<T> CreateScope(const T& arg)
	{
		return std::make_unique<T>(arg);
	}
}

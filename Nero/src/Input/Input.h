#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Keycodes.h"

namespace Nero
{
	class Input
	{
	public:
		static bool GetKeyDown(const Key& key);
		static bool GetKeyPressed(const Key& key);

		static bool GetKeyReleased(const Key& key);

		static bool GetMouseButtonPressed(const MouseButton& mouseButton);

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
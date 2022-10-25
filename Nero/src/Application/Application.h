#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	class Application
	{
	private:
		static GLFWwindow* m_GlfwWindow;

		static int32_t m_Width, m_Height;

		static std::function<void(int, int)> m_SizeCallback;

	public:
		static void Create(const char* name = "Nero Application", uint32_t width = 1280, uint32_t height = 720, bool vsync = true);
		static void Destroy();

		static void OnUpdate();

		static void OnResize(std::function<void(int, int)> function);
		static void ExternalViewportResize(int32_t width, int32_t height);

		static bool IsOpen();
		static glm::vec2 GetWindowSize();

		static GLFWwindow* GetGlfwWindow() { return m_GlfwWindow; }
	};
}



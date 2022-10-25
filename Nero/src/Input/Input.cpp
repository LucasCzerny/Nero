#include "Input.h"

#include "Application/Application.h"

namespace Nero
{
	bool Input::GetKeyDown(const Key& key)
	{
		FUNCTION_TIMER;

		static GLFWwindow* Application = Application::GetGlfwWindow();

		int state = glfwGetKey(Application, (int)key);
		return state == GLFW_PRESS;
	}

	bool Input::GetKeyPressed(const Key& key)
	{
		FUNCTION_TIMER;

		static GLFWwindow* Application = Application::GetGlfwWindow();

		int state = glfwGetKey(Application, (int)key);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::GetKeyReleased(const Key& key)
	{
		FUNCTION_TIMER;

		static GLFWwindow* Application = Application::GetGlfwWindow();

		int state = glfwGetKey(Application, (int)key);
		return state == GLFW_RELEASE;
	}

	bool Input::GetMouseButtonPressed(const MouseButton& mouseButton)
	{
		FUNCTION_TIMER;

		static GLFWwindow* Application = Application::GetGlfwWindow();

		int state = glfwGetMouseButton(Application, (int)mouseButton);
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		FUNCTION_TIMER;

		static GLFWwindow* Application = Application::GetGlfwWindow();

		double xPos, yPos;
		glfwGetCursorPos(Application, &xPos, &yPos);

		return glm::vec2((float)xPos, (float)yPos);
	}

	float Input::GetMouseX()
	{
		FUNCTION_TIMER;

		glm::vec2 mousePosition = GetMousePosition();

		return mousePosition.x;
	}

	float Input::GetMouseY()
	{
		FUNCTION_TIMER;

		glm::vec2 mousePosition = GetMousePosition();

		return mousePosition.y;
	}
}
#include "Time.h"

#include "GLFW/glfw3.h"

namespace Nero
{
	float Time::DeltaTime = 0.0f;
	float Time::m_LastTime = 0.0f;

	void Time::OnUpdate()
	{
		float currentTime = glfwGetTime();
		DeltaTime = currentTime - m_LastTime;
		m_LastTime = currentTime;
	}
}
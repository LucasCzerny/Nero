#pragma once

namespace Nero
{
	class Time
	{
	public:
		static float DeltaTime;

	private:
		static float m_LastTime;

	public:
		static void OnUpdate();
	};
}
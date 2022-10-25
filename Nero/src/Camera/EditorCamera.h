#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Camera.h"

namespace Nero
{
	class EditorCamera : public Camera
	{
	private:
		glm::vec2 m_OldMousePosition = glm::vec2(0.0f);

		const float m_LongitudinalSpeed = 2.75f;
		const float m_LateralSpeed = 4.0f;
		const float m_VerticalSpeed = 3.0f;

		const float m_GoFastMultiplier = 3.0f;

		const float m_PitchSpeed = 0.1f;
		const float m_YawSpeed = 0.15f;

		static const float m_VerticalFov;
		static const float m_NearPlane;
		static const float m_FarPlane;

	public:
		static Ref<EditorCamera> Create(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f));

		EditorCamera(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f));

		void OnUpdate(float deltaTime) override;

		operator Camera();
	};
}
#include "EditorCamera.h"

#include "Input/Input.h"

namespace Nero
{
	const float EditorCamera::m_VerticalFov = glm::radians(60.0f);
	const float EditorCamera::m_NearPlane = 0.1f;
	const float EditorCamera::m_FarPlane = 100.0f;

	Ref<EditorCamera> EditorCamera::Create(const glm::vec3& position /* = glm::vec3(0.0f) */, const glm::vec3& rotation /* = glm::vec3(0.0f) */)
	{
		FUNCTION_TIMER;

		return CreateRef<EditorCamera>(position, rotation);
	}

	EditorCamera::EditorCamera(const glm::vec3& position /* = glm::vec3(0.0f) */, const glm::vec3& rotation /* = glm::vec3(0.0f) */)
		: Camera(position, rotation, m_VerticalFov, m_NearPlane, m_FarPlane)
	{
		FUNCTION_TIMER;
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		FUNCTION_TIMER;

		Camera::OnUpdate(deltaTime);

		float speedMultiplier = Input::GetKeyDown(Key::LEFT_SHIFT) ? m_GoFastMultiplier : 1.0f;

		// Fowards/backwards

		if (Input::GetKeyDown(Key::W))
			MoveForwards(m_LongitudinalSpeed * speedMultiplier * deltaTime);
		else if (Input::GetKeyDown(Key::S))
			MoveBackwards(m_LongitudinalSpeed * speedMultiplier * deltaTime);

		// Left/right

		if (Input::GetKeyDown(Key::D))
			MoveRight(m_LateralSpeed * speedMultiplier * deltaTime);
		else if (Input::GetKeyDown(Key::A))
			MoveLeft(m_LateralSpeed * speedMultiplier * deltaTime);

		// Up/down

		if (Input::GetKeyDown(Key::SPACE))
			MoveY(m_VerticalSpeed * speedMultiplier * deltaTime);
		else if (Input::GetKeyDown(Key::LEFT_CONTROL))
			MoveY(-m_VerticalSpeed * speedMultiplier * deltaTime);

		glm::vec2 currentMousePosition = Input::GetMousePosition();

		if (Input::GetMouseButtonPressed(MouseButton::LEFT))
		{
			glm::vec2 mouseDelta = currentMousePosition - m_OldMousePosition;

			RotateX(-mouseDelta.y * m_PitchSpeed * deltaTime);
			RotateY(-mouseDelta.x * m_YawSpeed * deltaTime);
		}

		m_OldMousePosition = currentMousePosition;
	}

	EditorCamera::operator Camera()
	{
		FUNCTION_TIMER;

		return (Camera&)*this;
	}
}
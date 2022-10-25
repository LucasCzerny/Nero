#include "Camera.h"

namespace Nero
{
	Ref<Camera> Camera::Create(const glm::vec3& position, const glm::vec3& rotation, float fov, float nearPlane, float farPlane)
	{
		FUNCTION_TIMER;

		return CreateRef<Camera>(position, rotation, fov, nearPlane, farPlane);
	}

	Camera::Camera(const glm::vec3& position, const glm::vec3& rotation, float fov, float nearPlane, float farPlane)
		: m_Position(position), m_Rotation(rotation), m_VerticalFov(fov), m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		FUNCTION_TIMER;

		RecalculateProjectionMatrix();
		RecalculateViewMatrix();
		RecalculateViewProjectionMatrix();
	}

	void Camera::OnUpdate(float deltaTime)
	{
		FUNCTION_TIMER;

		if (m_HasMoved)
		{
			RecalculateViewMatrix();
			RecalculateViewProjectionMatrix();
		}

		m_HasMoved = false;
	}


	glm::vec3 Camera::GetForwardDirection() const
	{
		FUNCTION_TIMER;

		return glm::rotate(glm::quat(m_Rotation), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Camera::GetRightDirection() const
	{
		FUNCTION_TIMER;

		return glm::rotate(glm::quat(m_Rotation), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::GetUpDirection() const
	{
		FUNCTION_TIMER;

		return glm::rotate(glm::quat(m_Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Camera::SetViewportSize(uint32_t width, uint32_t height)
	{
		FUNCTION_TIMER;

		m_ViewportWidth = width, m_ViewportHeight = height;

		RecalculateProjectionMatrix();
		RecalculateViewProjectionMatrix();
	}

	void Camera::SetPosition(const glm::vec3& position)
	{
		FUNCTION_TIMER;

		m_Position = position;

		m_HasMoved = true;
	}

	void Camera::SetRotation(const glm::vec3& rotation)
	{
		FUNCTION_TIMER;

		m_Rotation = rotation;

		m_HasMoved = true;
	}

	void Camera::SetFov(float fov)
	{
		FUNCTION_TIMER;

		m_VerticalFov = fov;

		RecalculateProjectionMatrix();
		RecalculateViewProjectionMatrix();
	}

	void Camera::MoveForwards(float amount)
	{
		FUNCTION_TIMER;

		m_Position += GetForwardDirection() * amount;

		m_HasMoved = true;
	}

	void Camera::MoveBackwards(float amount)
	{
		FUNCTION_TIMER;

		m_Position -= GetForwardDirection() * amount;

		m_HasMoved = true;
	}

	void Camera::MoveRight(float amount)
	{
		FUNCTION_TIMER;

		m_Position += GetRightDirection() * amount;

		m_HasMoved = true;
	}

	void Camera::MoveLeft(float amount)
	{
		FUNCTION_TIMER;

		m_Position -= GetRightDirection() * amount;

		m_HasMoved = true;
	}

	void Camera::MoveUp(float amount)
	{
		FUNCTION_TIMER;

		m_Position += GetUpDirection() * amount;

		m_HasMoved = true;
	}

	void Camera::MoveDown(float amount)
	{
		FUNCTION_TIMER;

		m_Position -= GetUpDirection() * amount;

		m_HasMoved = true;
	}

	void Camera::MoveX(float amount)
	{
		FUNCTION_TIMER;

		m_Position.x += amount;

		m_HasMoved = true;
	}

	void Camera::MoveY(float amount)
	{
		FUNCTION_TIMER;

		m_Position.y += amount;

		m_HasMoved = true;
	}

	void Camera::MoveZ(float amount)
	{
		FUNCTION_TIMER;

		m_Position.z += amount;

		m_HasMoved = true;
	}

	void Camera::RotateX(float amount)
	{
		FUNCTION_TIMER;

		m_Rotation.x += amount;
		
		m_HasMoved = true;
	}

	void Camera::RotateY(float amount)
	{
		FUNCTION_TIMER;

		m_Rotation.y += amount;

		m_HasMoved = true;
	}

	void Camera::RotateZ(float amount)
	{
		FUNCTION_TIMER;

		m_Rotation.z += amount;

		m_HasMoved = true;
	}

	void Camera::RecalculateProjectionMatrix()
	{
		FUNCTION_TIMER;

		m_ProjectionMatrix = glm::perspective(m_VerticalFov, (float)m_ViewportWidth / (float)m_ViewportHeight, m_NearPlane, m_FarPlane);
	}

	void Camera::RecalculateViewMatrix()
	{
		FUNCTION_TIMER;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)
			* glm::toMat4(glm::quat(m_Rotation));

		m_ViewMatrix = glm::inverse(transform);
	}

	void Camera::RecalculateViewProjectionMatrix()
	{
		FUNCTION_TIMER;

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
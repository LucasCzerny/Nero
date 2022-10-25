#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	class Camera
	{
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position;
		glm::vec3 m_Rotation;

		glm::vec3 m_ForwardDirection;
		glm::vec3 m_RightDirection;
		glm::vec3 m_UpDirection;

		uint32_t m_ViewportWidth;
		uint32_t m_ViewportHeight;

		float m_VerticalFov;
		float m_NearPlane;
		float m_FarPlane;

		bool m_HasMoved = false;

	public:
		static Ref<Camera> Create(const glm::vec3& position, const glm::vec3& rotation, float fov, float nearPlane, float farPlane);

		Camera(const glm::vec3& position, const glm::vec3& rotation, float fov, float nearPlane, float farPlane);

		virtual void OnUpdate(float deltaTime);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() { return m_ViewProjectionMatrix; }

		float GetFov() const { return m_VerticalFov; };
		float GetNearPlane() const { return m_NearPlane; };
		float GetFarPlane() const { return m_FarPlane; };

		glm::vec3 GetViewPosition() const { return m_Position; }

		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetUpDirection() const;

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetPosition(const glm::vec3& position);
		void SetRotation(const glm::vec3& rotation);

		void SetFov(float fov);

		void MoveForwards(float amount);
		void MoveBackwards(float amount);

		void MoveRight(float amount);
		void MoveLeft(float amount);

		void MoveUp(float amount);
		void MoveDown(float amount);

		void MoveX(float amount);
		void MoveY(float amount);
		void MoveZ(float amount);

		void RotateX(float amount);
		void RotateY(float amount);
		void RotateZ(float amount);

	private:
		void RecalculateViewMatrix();
		void RecalculateProjectionMatrix();
		void RecalculateViewProjectionMatrix();
	};
}
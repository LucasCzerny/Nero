#pragma once

#include "Dependencies.h"
#include "Core.h"

namespace Nero
{
	class DirectionalLight
	{
	private:
		glm::vec3 m_Direction;
		glm::vec3 m_Diffuse;

		float m_NearPlane;
		float m_FarPlane;

		glm::mat4 m_LightSpaceTransform;

	public:
		static Ref<DirectionalLight> Create(const glm::vec3& direction, const glm::vec3& diffuse, float nearPlane = 1.0f, float farPlane = 7.5f);

		DirectionalLight() = default;
		DirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse, float nearPlane = 1.0f, float farPlane = 7.5f);

		glm::vec3 GetDirection() const { return m_Direction; }
		glm::vec3 GetDiffuse() const { return m_Diffuse; }

		float GetNearPlane() const { return m_NearPlane; }
		float GetFarPlane() const { return m_FarPlane; }

		float* GetDirectionPointer() const { return (float*)&m_Direction.x; }
		float* GetDiffusePointer() const { return (float*)&m_Diffuse.r; }

		float* GetNearPlanePointer() const { return (float*)&m_NearPlane; }
		float* GetFarPlanePointer() const { return (float*)&m_FarPlane; }

		glm::mat4 GetLightSpaceTransform() const { return m_LightSpaceTransform; }

		void SetDirection(const glm::vec3& direction);
		void SetDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }

		void CalculateLightSpaceTransform(const glm::vec3& center = glm::vec3(0.0f, 0.0f, 0.0f));
	};
}

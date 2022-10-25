#pragma once

#include "Dependencies.h"
#include "Core.h"

namespace Nero
{
	class PointLight
	{
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Diffuse;

		std::array<glm::mat4, 6> m_LightSpaceTransforms;

		float m_NearPlane;
		float m_FarPlane;

	public:
		static Ref<PointLight> Create(const glm::vec3& position, const glm::vec3& diffuse, float nearPlane = 0.1f, float farPlane = 25.0f);

		PointLight() = default;
		PointLight(const glm::vec3& position, const glm::vec3& diffuse, float nearPlane = 0.1f, float farPlane = 25.0f);

		glm::vec3 GetPosition() const { return m_Position; }
		glm::vec3 GetDiffuse() const { return m_Diffuse; }

		float GetNearPlane() const { return m_NearPlane; }
		float GetFarPlane() const { return m_FarPlane; }

		float* GetPositionPointer() const { return (float*)&m_Position.x; }
		float* GetDiffusePointer() const { return (float*)&m_Diffuse.r; }

		float* GetNearPlanePointer() const { return (float*)&m_NearPlane; }
		float* GetFarPlanePointer() const { return (float*)&m_FarPlane; }

		std::array<glm::mat4, 6> GetLightSpaceTransforms() { return m_LightSpaceTransforms; }

		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetDiffuse(const glm::vec3& diffuse) { m_Diffuse = diffuse; }

		void CalculateLightSpaceTransforms();
	};
}

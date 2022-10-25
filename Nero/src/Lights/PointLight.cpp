#include "PointLight.h"

namespace Nero
{
	Ref<PointLight> PointLight::Create(const glm::vec3& position, const glm::vec3& diffuse, float nearPlane /* = 0.1f */, float farPlane /* = 25.0f */)
	{
		return CreateRef<PointLight>(position, diffuse);
	}

	PointLight::PointLight(const glm::vec3& position, const glm::vec3& diffuse, float nearPlane /* = 0.1f */, float farPlane /* = 25.0f */)
		: m_Position(position), m_Diffuse(diffuse), m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		CalculateLightSpaceTransforms();
	}

	void PointLight::CalculateLightSpaceTransforms()
	{
		static glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, m_NearPlane, m_FarPlane);

		m_LightSpaceTransforms[0] = shadowProj * glm::lookAt(m_Position, m_Position + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
		m_LightSpaceTransforms[1] = shadowProj * glm::lookAt(m_Position, m_Position + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f));
		m_LightSpaceTransforms[2] = shadowProj * glm::lookAt(m_Position, m_Position + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f));
		m_LightSpaceTransforms[3] = shadowProj * glm::lookAt(m_Position, m_Position + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f));
		m_LightSpaceTransforms[4] = shadowProj * glm::lookAt(m_Position, m_Position + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
		m_LightSpaceTransforms[5] = shadowProj * glm::lookAt(m_Position, m_Position + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f));
	}
}
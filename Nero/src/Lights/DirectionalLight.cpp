#include "DirectionalLight.h"

namespace Nero
{
	Ref<DirectionalLight> DirectionalLight::Create(const glm::vec3& direction, const glm::vec3& diffuse, float nearPlane /* = 1.0f */, float farPlane /* = 7.5f */)
	{
		return CreateRef<DirectionalLight>(direction, diffuse, nearPlane, farPlane);
	}

	DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse, float nearPlane /* = 1.0f */, float farPlane /* = 7.5f */)
		: m_Direction(direction), m_Diffuse(diffuse), m_NearPlane(nearPlane), m_FarPlane(farPlane)
	{
		CalculateLightSpaceTransform();
	}

	void DirectionalLight::SetDirection(const glm::vec3& direction)
	{
		m_Direction = direction;
		CalculateLightSpaceTransform();
	}

	/*void DirectionalLight::MoveWith(const glm::vec3& position)
	{
		glm::vec3 positionHorizontal(position.x, 0.0f, position.y);
		CalculateLightSpaceTransform( positionHorizontal);
	}*/

	void DirectionalLight::CalculateLightSpaceTransform(const glm::vec3& center)
	{
		static glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, m_NearPlane, m_FarPlane);
		glm::mat4 lightView = glm::lookAt(center + m_Direction, center, glm::vec3(0.0f, 1.0f, 0.0f));

		m_LightSpaceTransform = lightProjection * lightView;
	}
}
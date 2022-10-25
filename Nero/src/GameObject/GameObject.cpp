#include "GameObject.h"

namespace Nero
{
	Ref<GameObject> GameObject::Create(const Ref<Model> model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
	{
		FUNCTION_TIMER;

		return CreateRef<GameObject>(model, position, scale, rotation);
	}

	GameObject::GameObject(const Ref<Model> model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
		: m_Model(model), m_Position(position), m_Scale(scale), m_Rotation(rotation)
	{
		FUNCTION_TIMER;

		CalculateModelMatrix();
	}

	glm::mat4 GameObject::GetModelMatrix()
	{
		FUNCTION_TIMER;

		if (m_HasMoved)
			CalculateModelMatrix();

		return m_ModelMatrix;
	}

	void GameObject::CalculateModelMatrix()
	{
		FUNCTION_TIMER;

		m_ModelMatrix = glm::translate(glm::mat4(1.0f), m_Position)
			* glm::toMat4(glm::quat(m_Rotation))
			* glm::scale(glm::mat4(1.0f), m_Scale);
	}

	void GameObject::SetPosition(const glm::vec3& position)
	{
		FUNCTION_TIMER;

		m_Position = position;

		m_HasMoved = true;
	}

	void GameObject::SetRotation(const glm::vec3& rotation)
	{
		FUNCTION_TIMER;

		m_Rotation = rotation;

		m_HasMoved = true;
	}

	void GameObject::MoveForwards(float amount)
	{
		FUNCTION_TIMER;

		m_Position += GetForwardDirection() * amount;

		m_HasMoved = true;
	}

	void GameObject::MoveBackwards(float amount)
	{
		FUNCTION_TIMER;

		m_Position -= GetForwardDirection() * amount;

		m_HasMoved = true;
	}

	void GameObject::MoveRight(float amount)
	{
		FUNCTION_TIMER;

		m_Position += GetRightDirection() * amount;

		m_HasMoved = true;
	}

	void GameObject::MoveLeft(float amount)
	{
		FUNCTION_TIMER;

		m_Position -= GetRightDirection() * amount;

		m_HasMoved = true;
	}

	void GameObject::MoveUp(float amount)
	{
		FUNCTION_TIMER;

		m_Position += GetUpDirection() * amount;

		m_HasMoved = true;
	}

	void GameObject::MoveDown(float amount)
	{
		FUNCTION_TIMER;

		m_Position -= GetUpDirection() * amount;

		m_HasMoved = true;
	}

	void GameObject::MoveX(float amount)
	{
		FUNCTION_TIMER;

		m_Position.x += amount;

		m_HasMoved = true;
	}

	void GameObject::MoveY(float amount)
	{
		FUNCTION_TIMER;

		m_Position.y += amount;

		m_HasMoved = true;
	}

	void GameObject::MoveZ(float amount)
	{
		FUNCTION_TIMER;

		m_Position.z += amount;

		m_HasMoved = true;
	}

	void GameObject::RotateX(float amount)
	{
		FUNCTION_TIMER;

		m_Rotation.x += amount;

		m_HasMoved = true;
	}

	void GameObject::RotateY(float amount)
	{
		FUNCTION_TIMER;

		m_Rotation.y += amount;

		m_HasMoved = true;
	}

	void GameObject::RotateZ(float amount)
	{
		FUNCTION_TIMER;

		m_Rotation.z += amount;

		m_HasMoved = true;
	}

	glm::vec3 GameObject::GetForwardDirection() const
	{
		FUNCTION_TIMER;

			return glm::rotate(glm::quat(m_Rotation), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 GameObject::GetRightDirection() const
	{
		FUNCTION_TIMER;

			return glm::rotate(glm::quat(m_Rotation), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 GameObject::GetUpDirection() const
	{
		FUNCTION_TIMER;

			return glm::rotate(glm::quat(m_Rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}
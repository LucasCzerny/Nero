#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Model/Model.h"

namespace Nero
{
	class GameObject
	{
	private:
		std::string m_Name;

		const Ref<Model> m_Model;

		glm::vec3 m_Position;
		glm::vec3 m_Scale;
		glm::vec3 m_Rotation;

		glm::mat4 m_ModelMatrix;

		bool m_HasMoved = false;

	public:
		static Ref<GameObject> Create(const Ref<Model> model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);

		GameObject(const Ref<Model> model, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);

		void HasMoved(bool moved = true) { m_HasMoved = moved; }

		void SetPosition(const glm::vec3& position);
		void SetRotation(const glm::vec3& rotation);

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

		const Ref<Model> GetModel() const { return m_Model; }
		const std::vector<Mesh>& GetMeshes() const { return m_Model->GetMeshes(); }

		glm::mat4 GetModelMatrix();
		void CalculateModelMatrix();

		glm::vec3 GetPosition() const { return m_Position; }
		glm::vec3 GetScale() const { return m_Scale; }
		glm::vec3 GetRotation() const { return m_Rotation; }

		float* GetPositionPointer() { return (float*)& m_Position.x; }
		float* GetScalePointer() { return (float*)&m_Scale.x; }
		float* GetRotationPointer() { return (float*)&m_Rotation.x; }

		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetUpDirection() const;

		std::string GetName() { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }
	};
}

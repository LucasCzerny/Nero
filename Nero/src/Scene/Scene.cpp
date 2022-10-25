#include "Scene.h"

#include "SceneSerializer.h"

namespace Nero
{
	Ref<Scene> Scene::Create()
	{
		FUNCTION_TIMER;

		return CreateRef<Scene>();
	}

	Ref<Scene> Scene::Create(const fs::path& path)
	{
		FUNCTION_TIMER;

		return DeserializeScene(path);
	}

	Ref<Model> Scene::GetModelByName(const std::string name) const
	{
		FUNCTION_TIMER;

		for (auto it = Models.begin(); it != Models.end(); it++)
		{
			if ((*it)->GetName() == name)
				return *it;
		}

		Assert::Unreachable("Could not find model with name" + name);
		return nullptr;
	}

	Ref<GameObject> Scene::GetGameObjectByName(const std::string name) const
	{
		FUNCTION_TIMER;

		for (auto it = GameObjects.begin(); it != GameObjects.end(); it++)
		{
			if ((*it)->GetName() == name)
				return *it;
		}

		Assert::Unreachable("Could not find game object with name" + name);
		return nullptr;
	}

	Ref<DirectionalLight> Scene::GetDirectionalLight(int32_t index) const
	{
		FUNCTION_TIMER;

		return DirectionalLights[index];
	}

	Ref<PointLight> Scene::GetPointLight(int32_t index) const
	{
		FUNCTION_TIMER;

		return PointLights[index];
	}
}
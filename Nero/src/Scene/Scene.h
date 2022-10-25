#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Model/Model.h"
#include "GameObject/GameObject.h"

#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"

#include "Skybox/Skybox.h"
		
namespace Nero
{
	class Scene
	{
	public:
		std::vector<Ref<Model>> Models;
		std::vector<Ref<GameObject>> GameObjects;

		std::vector<Ref<DirectionalLight>> DirectionalLights;
		std::vector<Ref<PointLight>> PointLights;

		Ref<Skybox> Skybox;

	public:
		static Ref<Scene> Create();
		static Ref<Scene> Create(const fs::path& path);

		Scene() = default;

		Ref<Model> GetModelByName(const std::string name) const;
		Ref<GameObject> GetGameObjectByName(const std::string name) const;

		Ref<DirectionalLight> GetDirectionalLight(int32_t index) const;
		Ref<PointLight> GetPointLight(int32_t index) const;
	};
}
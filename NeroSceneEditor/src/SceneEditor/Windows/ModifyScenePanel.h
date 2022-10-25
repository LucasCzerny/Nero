#pragma once

#include "EditorDependencies.h"

using namespace Nero;

namespace ImCustom
{
	void DrawModels(const Ref<Scene> currentScene);
	void DrawGameObjects(const Ref<Scene> currentScene);
	void DrawDirectionalLights(const Ref<Scene> currentScene);
	void DrawPointLights(const Ref<Scene> currentScene);
	void DrawSkybox(const Ref<Scene> currentScene);

	void DrawModifyScenePanel(const Ref<Scene> currentScene);
}
#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Scene.h"

namespace Nero
{
	void SerializeScene(const Ref<Scene> scene, const fs::path& outputPath);
	Ref<Scene> DeserializeScene(const fs::path& filePath);
}
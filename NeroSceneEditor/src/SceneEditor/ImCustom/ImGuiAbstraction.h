#pragma once

#include "EditorDependencies.h"

namespace ImCustom
{
	void CreateContext(GLFWwindow* window);
	void DestroyContext();

	void NewFrame();
	void RenderFrame(int32_t windowWidth, int32_t windowHeight);
}
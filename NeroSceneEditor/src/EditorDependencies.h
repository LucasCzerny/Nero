#pragma once

#include <Nero.h>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <cstdint>
#include <string>

#include <filesystem>

/*
 * RENAME STUFF
 */

namespace fs = std::filesystem;
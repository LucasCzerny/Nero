#pragma once

#include <iostream>
#include <cstdint>
#include <string>
#include <fstream>

#include <array>
#include <vector>
#include <unordered_map>

#include <functional>
#include <algorithm>

#include <filesystem>

#include <chrono>

#include <thread>

#include <cmath>
#include <random>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <yaml-cpp/yaml.h>

/*
 * RENAME STUFF
 */

namespace Nero
{
	namespace fs = std::filesystem;
}
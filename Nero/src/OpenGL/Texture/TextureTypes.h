#pragma once

#include "Dependencies.h"
#include "Core.h"

namespace Nero
{
    enum class TextureType
    {
        UNKNOWN = 0,
        ALBEDO = aiTextureType_BASE_COLOR,
        METALLIC_ROUGHNESS = aiTextureType_METALNESS,
        METALLIC_ROUGHNESS_2 = aiTextureType_DIFFUSE_ROUGHNESS,
        NORMAL = aiTextureType_NORMALS,
        DISPLACEMENT = aiTextureType_DISPLACEMENT
    };

    // Depth float? for pointlight shadow map, maybe direcitonal light shadow map idk
}
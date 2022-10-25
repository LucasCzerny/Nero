#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "OpenGL/BufferLayout/BufferLayout.h"

namespace Nero
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoords;
		glm::vec3 Tangent;

		static const BufferLayout& GetLayout()
		{
			static BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position"  },
				{ ShaderDataType::Float3, "a_Normal"    },
				{ ShaderDataType::Float2, "a_TexCoords" },
				{ ShaderDataType::Float3, "a_Tangent" }
			};

			return layout;
		}
	};
}
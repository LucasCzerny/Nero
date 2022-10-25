#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};
}

namespace Nero { namespace Util
{
	uint32_t ShaderDataTypeToOpenGlType(const ShaderDataType& type);

	uint32_t GetShaderDataTypeSize(const ShaderDataType& type);

	uint32_t GetCount(const ShaderDataType& type);
} }

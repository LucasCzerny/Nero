#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	enum class TextureFormat
	{
		None = 0,

		// Grayscale
		GRAYSCALE,

		// Color
		RG,
		RGB8, RGB16F, R11F_G11F_B10F,
		RGBA8, R32I, RGBA16F, RGBA32F,

		// Depth / Stencil
		DEPTH24STENCIL8,

		// Default Depth Format
		DEPTH = DEPTH24STENCIL8
	};

	struct GLTextureSpecification
	{
		GLenum InternalFormat, AccessFormat, Type;
	};
}

namespace Nero { namespace Util
{
	bool IsDepthBuffer(const TextureFormat& format);
	GLTextureSpecification GetGLTextureSpecification(TextureFormat format);
} }
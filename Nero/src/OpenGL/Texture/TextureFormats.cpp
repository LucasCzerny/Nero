#include "TextureFormats.h"

namespace Nero { namespace Util
{
	bool IsDepthBuffer(const TextureFormat& format)
	{
		FUNCTION_TIMER;

		return format == TextureFormat::DEPTH24STENCIL8;
	}

	GLTextureSpecification GetGLTextureSpecification(TextureFormat format)
	{
		FUNCTION_TIMER;

		switch (format)
		{
		case TextureFormat::GRAYSCALE:
			return { GL_RED, GL_RED, GL_UNSIGNED_BYTE };
		case TextureFormat::RG:
			return { GL_RGB16F, GL_RG, GL_FLOAT };
		case TextureFormat::RGB8:
			return { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE };
		case TextureFormat::RGB16F:
			return { GL_RGB16F, GL_RGB, GL_FLOAT };
		case TextureFormat::R11F_G11F_B10F:
			return { GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT };
		case TextureFormat::RGBA8:
			return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
		case TextureFormat::R32I:
			return { GL_R32I, GL_RED_INTEGER, GL_UNSIGNED_BYTE };
		case TextureFormat::RGBA16F:
			return { GL_RGBA16F, GL_RGBA, GL_FLOAT };
		case TextureFormat::RGBA32F:
			return { GL_RGBA32F, GL_RGBA, GL_FLOAT };
		case TextureFormat::DEPTH24STENCIL8:
			return { GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE };

		default:
		{
			Assert::Unreachable("Invalid Framebuffer Format");
			return { NULL, NULL, NULL };
		}
		}
	}
} }
#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "OpenGL/Texture/TextureFormats.h"

namespace Nero
{
	struct FramebufferAttachmentSpecification
	{
		TextureFormat Format = TextureFormat::None;

		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(TextureFormat format)
			: Format(format) {}
	};
	
	using FramebufferAttachments = std::vector<FramebufferAttachmentSpecification>;

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		// uint32_t Samples = 1;

		FramebufferAttachments Attachments;

		// Rendering to screen ... true
		bool SwapChainTarget = false;
	};
}
#pragma once

#include "Dependencies.h"
#include "Core.h"

namespace Nero
{
	class CubemapRenderFramebuffer
	{
	private:
		uint32_t m_FramebufferId;
		uint32_t m_RenderbufferId;

	public:
		static Ref<CubemapRenderFramebuffer> Create();

		CubemapRenderFramebuffer();

		void SetSpecification(int32_t width, int32_t height, GLenum internalFormat);
		void SetTarget(int32_t attachmentId, uint32_t rendererId, uint32_t mip = 0);

		void Bind();
		void Unbind();
	};
}
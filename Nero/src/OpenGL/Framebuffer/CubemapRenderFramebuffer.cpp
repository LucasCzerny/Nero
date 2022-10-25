#include "CubemapRenderFramebuffer.h"

namespace Nero
{
	Ref<CubemapRenderFramebuffer> CubemapRenderFramebuffer::Create()
	{
		FUNCTION_TIMER;
	
		return CreateRef<CubemapRenderFramebuffer>();
	}

	CubemapRenderFramebuffer::CubemapRenderFramebuffer()
	{
		FUNCTION_TIMER;
		
		glGenFramebuffers(1, &m_FramebufferId);
		glGenRenderbuffers(1, &m_RenderbufferId);

		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CubemapRenderFramebuffer::SetSpecification(int32_t width, int32_t height, GLenum internalFormat)
	{
		FUNCTION_TIMER;
		
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

		glViewport(0, 0, width, height);
	}

	void CubemapRenderFramebuffer::SetTarget(int32_t attachmentId, uint32_t rendererId, uint32_t mip /* = 0 */)
	{
		FUNCTION_TIMER;
		
		glFramebufferTexture(GL_FRAMEBUFFER, attachmentId, rendererId, mip);

		// Assert::That(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "CubemapRenderFramebuffer is incomplete!");
	}

	void CubemapRenderFramebuffer::Bind()
	{
		FUNCTION_TIMER;
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
	}

	void CubemapRenderFramebuffer::Unbind()
	{
		FUNCTION_TIMER;
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}
#include "RenderFramebuffer.h"

namespace Nero
{
	Ref<RenderFramebuffer> RenderFramebuffer::Create()
	{
		FUNCTION_TIMER;

		return CreateRef<RenderFramebuffer>();
	}

	RenderFramebuffer::RenderFramebuffer()
	{
		FUNCTION_TIMER;

		glGenFramebuffers(1, &m_FramebufferId);
		glGenRenderbuffers(1, &m_RenderbufferId);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferId);
	}

	void RenderFramebuffer::SetSpecification(int32_t width, int32_t height, GLenum internalFormat)
	{
		FUNCTION_TIMER;

		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);

		glViewport(0, 0, width, height);
	}

	void RenderFramebuffer::SetTarget(int32_t attachmentId, GLenum textureTarget, uint32_t rendererId, uint32_t mip /* = 0 */)
	{
		FUNCTION_TIMER;

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentId, textureTarget, rendererId, mip);
	}

	void RenderFramebuffer::Bind()
	{
		FUNCTION_TIMER;

		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
	}

	void RenderFramebuffer::Unbind()
	{
		FUNCTION_TIMER;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}
#include "Framebuffer.h"

namespace Nero
{
	Ref<Framebuffer> Framebuffer::Create()
	{
		FUNCTION_TIMER;

		return CreateRef<Framebuffer>();
	}

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		FUNCTION_TIMER;

		return CreateRef<Framebuffer>(specification);
	}

	Framebuffer::Framebuffer(const FramebufferSpecification& specification)
		: m_Specification(specification)
	{
		FUNCTION_TIMER;

		SetSpecification(specification);
	}

	void Framebuffer::SetSpecification(const FramebufferSpecification& specification)
	{
		FUNCTION_TIMER;

		for (FramebufferAttachmentSpecification attachment : m_Specification.Attachments)
		{
			if (Util::IsDepthBuffer(attachment.Format))
				m_DepthAttachment = attachment;
			else
				m_ColorAttachments.emplace_back(attachment);
		}

		Recreate();
	}

	void Framebuffer::Recreate()
	{
		FUNCTION_TIMER;

		if (m_RendererId)
		{
			DeleteAll();
		}

		glCreateFramebuffers(1, &m_RendererId);

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);

		uint32_t size = m_ColorAttachments.size();

		if (size != 0)
		{
			m_ColorAttachmentIDs.resize(size);
			CreateTextures(m_ColorAttachmentIDs.data(), size);

			for (int i = 0; i < size; i++)
			{
				FramebufferAttachmentSpecification attachment = m_ColorAttachments[i];
				uint32_t id = m_ColorAttachmentIDs[i];

				AttachTexture(id, attachment, GL_COLOR_ATTACHMENT0 + i);
			}

			GLenum buffers[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			glDrawBuffers(size, buffers);
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}

		if (m_DepthAttachment.Format != TextureFormat::None)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentID);

			AttachTexture(m_DepthAttachmentID, m_DepthAttachment, GL_DEPTH_ATTACHMENT);
		}

		Assert::That(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::DeleteAll()
	{
		FUNCTION_TIMER;

		glDeleteFramebuffers(1, &m_RendererId);

		glDeleteTextures(m_ColorAttachmentIDs.size(), m_ColorAttachmentIDs.data());
		glDeleteTextures(1, &m_DepthAttachmentID);
	}

	void Framebuffer::CreateTextures(uint32_t* outID, uint32_t count)
	{
		FUNCTION_TIMER;

		glCreateTextures(GL_TEXTURE_2D, count, outID);
	}

	void Framebuffer::AttachTexture(uint32_t id, const FramebufferAttachmentSpecification& attachment, int attachmentSlot)
	{
		FUNCTION_TIMER;

		glBindTexture(GL_TEXTURE_2D, id);

		GLTextureSpecification format = Util::GetGLTextureSpecification(attachment.Format);

		if (attachmentSlot == GL_DEPTH_ATTACHMENT)
		{
			glTexStorage2D(GL_TEXTURE_2D, 1, format.InternalFormat, m_Specification.Width, m_Specification.Height);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format.InternalFormat, m_Specification.Width, m_Specification.Height,
				0, format.AccessFormat, format.Type, nullptr);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentSlot, GL_TEXTURE_2D, id, 0);
	}

	void Framebuffer::Bind() const
	{
		FUNCTION_TIMER;

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Framebuffer::Unbind() const
	{
		FUNCTION_TIMER;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// glReadBuffer(GL_COLOR_ATTACHMENT0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		FUNCTION_TIMER;

		m_Specification.Width = width;
		m_Specification.Height = height;

		Recreate();
	}

	uint32_t Framebuffer::GetColorAttachmentRendererID(uint32_t index /* = 0 */) const
	{
		FUNCTION_TIMER;

		Assert::That(index < m_ColorAttachmentIDs.size(), "Index out of range");

		return m_ColorAttachmentIDs[index];
	}
}
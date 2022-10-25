#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "FramebufferSpecification.h"

#include "OpenGL/Texture/Texture2D.h"

namespace Nero
{
	class Framebuffer
	{
	private:
		uint32_t m_RendererId;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferAttachmentSpecification> m_ColorAttachments;
		FramebufferAttachmentSpecification m_DepthAttachment;

		std::vector<uint32_t> m_ColorAttachmentIDs;
		uint32_t m_DepthAttachmentID;

	public:
		static Ref<Framebuffer> Create();
		static Ref<Framebuffer> Create(const FramebufferSpecification& specification);

		Framebuffer() = default;
		Framebuffer(const FramebufferSpecification& specification);

		void SetSpecification(const FramebufferSpecification& specification);

		void Bind() const;
		void Unbind() const;

		void Resize(uint32_t width, uint32_t height);

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const;
		uint32_t GetDepthAttachmentRendererID() const { return m_DepthAttachmentID; }

	private:
		void Recreate();
		void DeleteAll();

		static void CreateTextures(uint32_t* outID, uint32_t count);
		void AttachTexture(uint32_t id, const FramebufferAttachmentSpecification& attachment, int attachmentSlot);

	};
}
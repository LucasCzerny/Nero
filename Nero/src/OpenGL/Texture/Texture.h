#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	class Texture
	{
	protected:
		uint32_t m_RendererId;
		uint32_t m_Width, m_Height;

	public:
		virtual void Bind(uint32_t slot = 0) const = 0;
		static void Bind(uint32_t rendererId, uint32_t slot);

		uint32_t GetRendererID() const { return m_RendererId; }

		virtual void GenerateMipMap() const = 0;

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		glm::vec2 GetSize() const { return glm::vec2(m_Width, m_Height); }

	protected:
		Texture()
			: m_Width(0), m_Height(0) {}

		Texture(uint32_t rendererId)
			: m_RendererId(rendererId) {}

		Texture(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}
	};
}
#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Texture.h"
#include "TextureFormats.h"
#include "TextureTypes.h"

namespace Nero
{
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> BlueTexture;

	private:
		fs::path m_Path;
		TextureType m_TypeHint = TextureType::UNKNOWN;

	public:
		static Ref<Texture2D> Create(const fs::path& filePath);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat format, void* data);
		static Ref<Texture2D> Create(const aiTexture* texture, const fs::path& path = "");

		Texture2D(const fs::path& filePath);
		Texture2D(uint32_t width, uint32_t height, TextureFormat format);
		Texture2D(uint32_t width, uint32_t height, TextureFormat format, void* data);
		Texture2D(const aiTexture* texture, const fs::path& path = "");

		~Texture2D();

		void Bind(uint32_t slot = 0) const override;
		static void Bind(uint32_t rendererId, uint32_t slot);

		void SetData(void* data);
		void GenerateMipMap() const override;

		TextureType GetTextureTypeHint() const { return m_TypeHint; }
		fs::path GetPath() const { return m_Path; }

		void SetTextureTypeHint(const TextureType& typeHint) { m_TypeHint = typeHint; }
	};
}
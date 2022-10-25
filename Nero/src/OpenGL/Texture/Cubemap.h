#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Texture.h"
#include "TextureFormats.h"

namespace Nero
{
	class Cubemap : public Texture
	{
	private:
		std::string m_Path;

	public:
		static Ref<Cubemap> Create(uint32_t rendererId);
		static Ref<Cubemap> Create(int32_t width, int32_t height, bool floatTexture = false);
		static Ref<Cubemap> Create(int32_t width, int32_t height, TextureFormat format);
		static Ref<Cubemap> Create(const std::array<fs::path, 6>& paths, bool floatTexture = false);

		Cubemap(uint32_t rendererId);
		Cubemap(int32_t width, int32_t height, bool floatTexture = false);
		Cubemap(int32_t width, int32_t height, TextureFormat format);
		Cubemap(const std::array<fs::path, 6>& paths, bool floatTexture = false);

		~Cubemap();

		void Bind(uint32_t slot = 0) const override;
		static void Bind(uint32_t rendererId, uint32_t slot);

		void GenerateMipMap() const override;
	};
}



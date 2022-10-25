#include "Cubemap.h"

#include "TextureFormats.h"

namespace Nero
{
	Ref<Cubemap> Cubemap::Create(uint32_t rendererId)
	{
		FUNCTION_TIMER;

		return CreateRef<Cubemap>(rendererId);
	}

	Ref<Cubemap> Cubemap::Create(int32_t width, int32_t height, bool floatTexture /* = false */)
	{
		FUNCTION_TIMER;

		return CreateRef<Cubemap>(width, height, floatTexture);
	}
	
	Ref<Cubemap> Cubemap::Create(int32_t width, int32_t height, TextureFormat format)
	{
		FUNCTION_TIMER;

		return CreateRef<Cubemap>(width, height, format);
	}

	Ref<Cubemap> Cubemap::Create(const std::array<fs::path, 6>& paths, bool floatTexture /* = false */)
	{
		FUNCTION_TIMER;

		return CreateRef<Cubemap>(paths, floatTexture);
	}

	Cubemap::Cubemap(uint32_t rendererId)
		: Texture(rendererId)
	{
	}

	Cubemap::Cubemap(int32_t width, int32_t height, bool floatTexture /* = false */)
		: Texture(width, height)
	{
		FUNCTION_TIMER;

		glGenTextures(1, &m_RendererId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);

		for (uint32_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Cubemap::Cubemap(int32_t width, int32_t height, TextureFormat format)
	{
		FUNCTION_TIMER;

		glGenTextures(1, &m_RendererId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);

		GLTextureSpecification spec = Util::GetGLTextureSpecification(format);

		for (uint32_t i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, spec.InternalFormat, width, height, 0, spec.AccessFormat, spec.Type, nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Cubemap::Cubemap(const std::array<fs::path, 6>& paths, bool floatTexture /* = false */)
	{
		FUNCTION_TIMER;

		glGenTextures(1, &m_RendererId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);

		stbi_set_flip_vertically_on_load(0);
		int width, height, channels;

		fs::path assetDirectory = Util::GetAssetDirectory();

		for (int i = 0; i < 6; i++)
		{
			const fs::path& filePath = paths[i];
			fs::path fullPath = assetDirectory / filePath;
			std::string pathString = fullPath.string();

			stbi_uc* data = stbi_load(pathString.c_str(), &width, &height, &channels, NULL);

			Assert::That(data != nullptr, "Failed to load image from path " + pathString);

			m_Width = width, m_Height = height;

			TextureFormat format;

			switch (channels)
			{
			case 1:
				format = TextureFormat::GRAYSCALE;
				break;

			case 3:
				format = floatTexture ? TextureFormat::RGB16F : TextureFormat::RGB8;
				break;

			case 4:
				format = floatTexture ? TextureFormat::RGBA16F : TextureFormat::RGBA8;
				break;

			default:
				Assert::Unreachable("Texture format not supported!");
			}

			GLTextureSpecification spec = Util::GetGLTextureSpecification(format);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, spec.InternalFormat,
				width, height, 0, spec.AccessFormat, spec.Type, data);

			if (data)
				stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Cubemap::~Cubemap()
	{
		FUNCTION_TIMER;

		glDeleteTextures(1, &m_RendererId);
	}

	void Cubemap::Bind(uint32_t slot /* = 0 */) const
	{
		FUNCTION_TIMER;

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);
	}

	void Cubemap::GenerateMipMap() const
	{
		FUNCTION_TIMER;

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererId);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	void Cubemap::Bind(uint32_t rendererId, uint32_t slot)
	{
		FUNCTION_TIMER;;

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, rendererId);
	}
}
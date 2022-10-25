#include "Texture2D.h"

namespace Nero
{
	Ref<Texture2D> Texture2D::BlueTexture;

	Ref<Texture2D> Texture2D::Create(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		return CreateRef<Texture2D>(filePath);
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFormat format)
	{
		FUNCTION_TIMER;

		return CreateRef<Texture2D>(width, height, format);
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, TextureFormat format, void* data)
	{
		FUNCTION_TIMER;

		return CreateRef<Texture2D>(width, height, format, data);
	}

	Ref<Texture2D> Texture2D::Create(const aiTexture* texture, const fs::path& path /* = "" */)
	{
		FUNCTION_TIMER;

		return CreateRef<Texture2D>(texture, path);
	}

	Texture2D::Texture2D(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		fs::path fullPath = Util::GetAssetDirectory() / filePath;

		m_Path = fullPath;

		std::string pathString = fullPath.string();

		glGenTextures(1, &m_RendererId);

		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		stbi_uc* data = stbi_load(pathString.c_str(), &width, &height, &channels, NULL);

		Assert::That(data != nullptr, "Failed to load image from path " + pathString);

		m_Width = width;
		m_Height = height;

		TextureFormat format;

		switch (channels)
		{
		case 1:
			format = TextureFormat::GRAYSCALE;
			break;

		case 3:
			format = TextureFormat::RGB8;
			break;

		case 4:
			format = TextureFormat::RGBA8;
			break;

		default:
			Assert::Unreachable("Texture format not supported!");
		}
		
		GLTextureSpecification spec = Util::GetGLTextureSpecification(format);

		glBindTexture(GL_TEXTURE_2D, m_RendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, spec.InternalFormat, m_Width, m_Height, 0, spec.AccessFormat, spec.Type, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
		glGenerateMipmap(GL_TEXTURE_2D);

		if (data)
		{
			stbi_image_free(data);
		}
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, TextureFormat format)
		: Texture(width, height), m_Path("")
	{
		FUNCTION_TIMER;

		GLTextureSpecification spec = Util::GetGLTextureSpecification(format);

		glGenTextures(1, &m_RendererId);

		glBindTexture(GL_TEXTURE_2D, m_RendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, spec.InternalFormat, m_Width, m_Height, 0, spec.AccessFormat, spec.Type, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture2D::Texture2D(uint32_t width, uint32_t height, TextureFormat format, void* data)
		: Texture(width, height), m_Path("")
	{
		FUNCTION_TIMER;

		GLTextureSpecification spec = Util::GetGLTextureSpecification(format);

		glGenTextures(1, &m_RendererId);

		glBindTexture(GL_TEXTURE_2D, m_RendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, spec.InternalFormat, m_Width, m_Height, 0, spec.AccessFormat, spec.Type, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
		glGenerateMipmap(GL_TEXTURE_2D);

		SetData(data);
	}

	Texture2D::Texture2D(const aiTexture* texture, const fs::path& path /* = "" */)
	{
		FUNCTION_TIMER;

		unsigned char* image_data = nullptr;
		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		if (texture->mHeight == 0)
		{
			image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &channels, 0);
		}
		else
		{
			image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &channels, 0);
		}

		m_Width = width;
		m_Height = height;
		m_Path = path;

		TextureFormat format;

		switch (channels)
		{
		case 1:
			format = TextureFormat::GRAYSCALE;
			break;

		case 3:
			format = TextureFormat::RGB8;
			break;

		case 4:
			format = TextureFormat::RGBA8;
			break;

		default:
			Assert::Unreachable("Texture format not supported!");
		}

		GLTextureSpecification spec = Util::GetGLTextureSpecification(format);


		glGenTextures(1, &m_RendererId);

		glBindTexture(GL_TEXTURE_2D, m_RendererId);
		glTexImage2D(GL_TEXTURE_2D, 0, spec.InternalFormat, width, height, 0, spec.AccessFormat, spec.Type, image_data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererId);
	}

	void Texture2D::Bind(uint32_t slot /* = 0 */) const
	{
		FUNCTION_TIMER;

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererId);
	}

	void Texture2D::Bind(uint32_t rendererId, uint32_t slot)
	{
		FUNCTION_TIMER;

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, rendererId);
	}

	void Texture2D::SetData(void* data)
	{
		FUNCTION_TIMER;

		glTextureSubImage2D(m_RendererId, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	void Texture2D::GenerateMipMap() const
	{
		Assert::Unreachable("Not implemented yet lol");
	}
}
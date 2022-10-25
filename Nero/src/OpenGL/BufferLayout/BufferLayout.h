#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "OpenGL/Shader/ShaderDataType.h"

namespace Nero
{
	struct BufferElement
	{
	public:
		ShaderDataType Type;
		std::string Name;
		uint32_t Size;
		uint32_t Offset;
		uint32_t Count;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Type(type), Name(name), Size(Util::GetShaderDataTypeSize(type)), Count(Util::GetCount(type)), Offset(0), Normalized(normalized) {}
	};

	struct BufferLayout
	{
	private:
		uint32_t m_Stride = 0;
		std::vector<BufferElement> m_Elements;

	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements);

		inline uint32_t GetStride() const { return m_Stride; }
		inline std::vector<BufferElement> GetElements() const { return m_Elements; }
	};
}
#include "IndexBuffer.h"

namespace Nero
{
	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* data, uint32_t size)
	{
		FUNCTION_TIMER;

		return CreateRef<IndexBuffer>(data, size);
	}

	IndexBuffer::IndexBuffer(uint32_t* data, uint32_t size)
	{
		FUNCTION_TIMER;

		m_Count = size / sizeof(uint32_t);

		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer()
	{
		FUNCTION_TIMER;

		glDeleteBuffers(1, &m_RendererId);
	}

	void IndexBuffer::Bind() const
	{
		FUNCTION_TIMER;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
	}

	void IndexBuffer::Unbind() const
	{
		FUNCTION_TIMER;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
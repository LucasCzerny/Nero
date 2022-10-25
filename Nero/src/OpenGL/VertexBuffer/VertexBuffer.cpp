#include "VertexBuffer.h"

namespace Nero
{
	Ref<VertexBuffer> VertexBuffer::Create(const float* data, uint32_t size)
	{
		FUNCTION_TIMER;

		return CreateRef<VertexBuffer>(data, size);
	}

	VertexBuffer::VertexBuffer(const float* data, uint32_t size)
	{
		FUNCTION_TIMER;

		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	VertexBuffer::~VertexBuffer()
	{
		FUNCTION_TIMER;

		glDeleteBuffers(1, &m_RendererId);
	}

	void VertexBuffer::Bind() const
	{
		FUNCTION_TIMER;

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}

	void VertexBuffer::Unbind() const
	{
		FUNCTION_TIMER;

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
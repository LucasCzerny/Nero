#include "VertexArray.h"

#include "OpenGL/Shader/ShaderDataType.h"

namespace Nero
{
	Ref<VertexArray> VertexArray::Create()
	{
		FUNCTION_TIMER;

		return CreateRef<VertexArray>();
	}

	VertexArray::VertexArray()
	{
		FUNCTION_TIMER;

		glCreateVertexArrays(1, &m_RendererId);
		glBindVertexArray(m_RendererId);
	}

	VertexArray::~VertexArray()
	{
		FUNCTION_TIMER;

		glDeleteVertexArrays(1, &m_RendererId);
	}

	void VertexArray::Bind() const
	{
		FUNCTION_TIMER;

		glBindVertexArray(m_RendererId);
	}

	void VertexArray::Unbind() const
	{
		FUNCTION_TIMER;

		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer, const BufferLayout& layout)
	{
		FUNCTION_TIMER;

		Assert::That(layout.GetElements().size() != 0, "Layout is not set");

		Bind();
		vertexBuffer->Bind();

		std::vector<BufferElement> elements = layout.GetElements();
		for (uint32_t i = 0; i < elements.size(); i++)
		{
			BufferElement element = elements[i];

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(
				i,
				element.Count,
				Util::ShaderDataTypeToOpenGlType(element.Type),
				element.Normalized,
				layout.GetStride(),
				(const void*)element.Offset
			);
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer> indexBuffer)
	{
		FUNCTION_TIMER;

		glBindVertexArray(m_RendererId);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}
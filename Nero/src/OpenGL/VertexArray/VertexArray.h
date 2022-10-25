#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "OpenGL/VertexBuffer/VertexBuffer.h"
#include "OpenGL/IndexBuffer/IndexBuffer.h"
#include "OpenGL/BufferLayout/BufferLayout.h"

namespace Nero
{
	class VertexArray
	{
	private:
		uint32_t m_RendererId;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;

	public:
		static Ref<VertexArray> Create();

		VertexArray();
		~VertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer, const BufferLayout& layout);
		void SetIndexBuffer(const Ref<IndexBuffer> indexBuffer);

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
		Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }
	};
}
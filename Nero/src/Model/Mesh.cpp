#include "Mesh.h"

namespace Nero
{
	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Ref<Texture2D> albedo, const Ref<Texture2D> normal, const Ref<Texture2D> metallicRoughness)
		: m_Vertices(vertices), m_Indices(indices), m_Albedo(albedo), m_Normal(normal), m_MetallicRoughness(metallicRoughness)
	{
		FUNCTION_TIMER;

		m_VertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create((float*)&m_Vertices[0], m_Vertices.size() * sizeof(Vertex));

		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create((uint32_t*)&m_Indices[0], m_Indices.size() * sizeof(uint32_t));

		m_VertexArray->AddVertexBuffer(vertexBuffer, Vertex::GetLayout());
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}

	void Mesh::BindVertexArray() const
	{
		FUNCTION_TIMER;

		m_VertexArray->Bind();
	}
}
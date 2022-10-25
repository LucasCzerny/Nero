#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Renderer/Vertex.h"
#include "OpenGL/VertexArray/VertexArray.h"
#include "OpenGL/Texture/Texture2D.h"

namespace Nero
{
	class Mesh
	{
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		Ref<Texture2D> m_Albedo;
		Ref<Texture2D> m_Normal;
		Ref<Texture2D> m_MetallicRoughness;

		Ref<VertexArray> m_VertexArray;

	public:
		Mesh() = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, const Ref<Texture2D> albedo, const Ref<Texture2D> normal, const Ref<Texture2D> metallicRoughness);

		void BindVertexArray() const;

		const Ref<Texture2D> GetAlbedoMap() const { return m_Albedo; }
		const Ref<Texture2D> GetNormalMap() const { return m_Normal; }
		const Ref<Texture2D> GetMetallicRoughnessMap() const { return m_MetallicRoughness; }
		
		int32_t GetIndexCount() const { return m_Indices.size(); }
	};
}
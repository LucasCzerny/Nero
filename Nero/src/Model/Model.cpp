#include "Model.h"

namespace Nero
{
	Ref<Model> Model::Create(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		return CreateRef<Model>(filePath);
	}

	Model::Model(const fs::path& filePath)
		: m_FilePath(filePath)
	{
		FUNCTION_TIMER;

		fs::path fullPath = Util::GetAssetDirectory() / filePath;

		fs::path directory = fullPath;
		directory.remove_filename();
		m_Directory = directory;

		LoadModel(fullPath);
	}

	void Model::LoadModel(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		std::string pathString = filePath.string();

		Assimp::Importer importer;

		{
			SCOPE_TIMER("Assimp model import");

			m_Scene = importer.ReadFile(pathString, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices);
		}

		if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
		{
			const char* errorString = importer.GetErrorString();
			Assert::Unreachable("Assimp Error: " + std::string(errorString));
		}

		aiNode* rootNode = m_Scene->mRootNode;
		
		ProcessNode(rootNode, m_Scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* m_Scene)
	{
		FUNCTION_TIMER;
		
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* assimpMesh = m_Scene->mMeshes[node->mMeshes[i]];

			Mesh neroMesh = ProcessMesh(assimpMesh, m_Scene);
			m_Meshes.push_back(neroMesh);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], m_Scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* m_Scene)
	{
		FUNCTION_TIMER;

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Ref<Texture2D>> textures;

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			glm::vec3 position;
			position.x = mesh->mVertices[i].x;
			position.y = mesh->mVertices[i].y;
			position.z = mesh->mVertices[i].z;

			vertex.Position = position;

			glm::vec3 normal;
			normal.x = mesh->mNormals[i].x;
			normal.y = mesh->mNormals[i].y;
			normal.z = mesh->mNormals[i].z;

			vertex.Normal = normal;

			glm::vec2 textureCoords;

			if (mesh->mTextureCoords[0])
			{
				textureCoords.x = mesh->mTextureCoords[0][i].x;
				textureCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				textureCoords = glm::vec2(0.0f, 1.0f);
			}

			vertex.TextureCoords = textureCoords;

			glm::vec3 tangent;
			tangent.x = mesh->mTangents[i].x;
			tangent.y = mesh->mTangents[i].y;
			tangent.z = mesh->mTangents[i].z;

			vertex.Tangent = tangent;

			vertices.push_back(vertex);
		}

		// INDICES

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];

			for (int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		// MATERIAL

		Ref<Texture2D> albedo, normal, metallicRoughness;

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = m_Scene->mMaterials[mesh->mMaterialIndex];

			albedo = LoadMaterialTexture(material, aiTextureType_BASE_COLOR);
			normal = LoadMaterialTexture(material, aiTextureType_NORMALS);
			metallicRoughness = LoadMaterialTexture(material, aiTextureType_METALNESS);
		}

		return Mesh(vertices, indices, albedo, normal, metallicRoughness);
	}

	Ref<Texture2D> Model::LoadMaterialTexture(aiMaterial* material, aiTextureType type)
	{
		FUNCTION_TIMER;

		if (!material->GetTextureCount(type))
			return Texture2D::BlueTexture;

		aiString path;
		material->GetTexture(type, 0, &path);

		fs::path filePath(path.C_Str());

		bool hasEmbeddedTexture = filePath.extension() == "";

		const aiTexture* embeddedTexture = nullptr;
		if (hasEmbeddedTexture)
			embeddedTexture = m_Scene->GetEmbeddedTexture(path.C_Str());

		fs::path fullPath = m_Directory / filePath;

		bool load = true;

		for (auto it = m_TextureCache.begin(); it != m_TextureCache.end(); it++)
		{
			fs::path cachedPath = (*it)->GetPath();

			if (cachedPath == fullPath)
			{
				return *it;
			}
		}

		Ref<Texture2D> texture;

		if (hasEmbeddedTexture)
			texture = Texture2D::Create(embeddedTexture, filePath);
		else
			texture = Texture2D::Create(fullPath);

		texture->SetTextureTypeHint((TextureType)type);

		m_TextureCache.push_back(texture);

		return texture;
	}

	glm::mat4 Model::AiMatrix4x4ToGlm(const aiMatrix4x4* from)
	{
		glm::mat4 to;


		to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
		to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
		to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
		to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

		return to;
	}
}
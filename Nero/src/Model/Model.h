#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Mesh.h"

namespace Nero
{
	class Model
	{
	private:
		std::string m_Name;

		std::vector<Mesh> m_Meshes;
		std::vector<Ref<Texture2D>> m_TextureCache;

		fs::path m_Directory;
		fs::path m_FilePath;

		const aiScene* m_Scene;

	public:
		static Ref<Model> Create(const fs::path& filePath);

		Model(const fs::path& filePath);

		const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }

		fs::path GetPath() const { return m_FilePath; }

		std::string GetName() { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

	private:
		void LoadModel(const fs::path& filePath);

		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		Ref<Texture2D> LoadMaterialTexture(aiMaterial* material, aiTextureType type);

		glm::mat4 AiMatrix4x4ToGlm(const aiMatrix4x4* from);
	};
}
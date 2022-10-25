#include "SceneSerializer.h"

// Custom encode & decode functions for glm
namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Nero
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	void SerializeScene(const Ref<Scene> scene, const fs::path& outputPath)
	{
		YAML::Emitter out;

		out << YAML::BeginMap; // Scene


		out << YAML::Key << "Skybox" << YAML::Value << YAML::BeginMap; // Skybox

		out << YAML::Key << "Directory" << YAML::Value << scene->Skybox->GetDirectory().string();
		out << YAML::Key << "Extension" << YAML::Value << scene->Skybox->GetFileExtension();

		out << YAML::EndMap; // Skybox


		out << YAML::Key << "DirectionalLights" << YAML::Value << YAML::BeginSeq; // DirectionalLights

		std::vector<Ref<DirectionalLight>> directionalLights = scene->DirectionalLights;
		for (auto it = directionalLights.begin(); it != directionalLights.end(); it++)
		{
			const Ref<DirectionalLight> light = *it;

			out << YAML::BeginMap; // DirectionalLight

			out << YAML::Key << "Direction" << YAML::Value << light->GetDirection();
			out << YAML::Key << "Diffuse" << YAML::Value << light->GetDiffuse();
			out << YAML::Key << "NearPlane" << YAML::Value << light->GetNearPlane();
			out << YAML::Key << "FarPlane" << YAML::Value << light->GetFarPlane();

			out << YAML::EndMap; // DirectionalLight
		}

		out << YAML::EndSeq; // DirectionalLights


		out << YAML::Key << "PointLights" << YAML::Value << YAML::BeginSeq; // PointLights

		std::vector<Ref<PointLight>> pointLights = scene->PointLights;
		for (auto it = pointLights.begin(); it != pointLights.end(); it++)
		{
			const Ref<PointLight> light = *it;

			out << YAML::BeginMap; // PointLight

			out << YAML::Key << "Position" << YAML::Value << light->GetPosition();
			out << YAML::Key << "Diffuse" << YAML::Value << light->GetDiffuse();
			out << YAML::Key << "NearPlane" << YAML::Value << light->GetNearPlane();
			out << YAML::Key << "FarPlane" << YAML::Value << light->GetFarPlane();

			out << YAML::EndMap; // PointLight
		}

		out << YAML::EndSeq; // PointLights


		out << YAML::Key << "Models" << YAML::Value << YAML::BeginSeq; // Models

		for (auto it = scene->Models.begin(); it != scene->Models.end(); it++)
		{
			const Ref<Model> model = *it;

			out << YAML::BeginMap; // Model

			out << YAML::Key << "Name" << YAML::Value << model->GetName();
			std::string pathString = model->GetPath().string();
			out << YAML::Key << "Path" << YAML::Value << pathString;

			out << YAML::EndMap; // Model
		}

		out << YAML::EndSeq; // Models


		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq; // GameObjects

		for (auto it = scene->GameObjects.begin(); it != scene->GameObjects.end(); it++)
		{
			const Ref<GameObject> gameObject = *it;

			out << YAML::BeginMap; // GameObject

			out << YAML::Key << "Name" << YAML::Value << gameObject->GetName();
			out << YAML::Key << "ModelName" << YAML::Value << gameObject->GetModel()->GetName();

			out << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap; // Transform

			out << YAML::Key << "Position" << YAML::Value << gameObject->GetPosition();
			out << YAML::Key << "Scale" << YAML::Value << gameObject->GetScale();
			out << YAML::Key << "Rotation" << YAML::Value << gameObject->GetRotation();

			out << YAML::Key << YAML::EndMap; // Transform

			out << YAML::EndMap; // GameObject
		}

		out << YAML::EndSeq; // GameObjects


		out << YAML::EndMap; // Scene

		std::ofstream outputFile(outputPath);
		outputFile << out.c_str();
	}

	Ref<Scene> DeserializeScene(const fs::path& filePath)
	{
		std::ifstream inputFile(filePath);

		if (inputFile.peek() == std::ifstream::traits_type::eof())
		{
			inputFile.close();
			Assert::Unreachable("Could not open" + filePath.string());
		}

		std::stringstream fileName;
		fileName << inputFile.rdbuf();

		inputFile.close();

		YAML::Node data = YAML::Load(fileName.str());
		Ref<Scene> newScene = Scene::Create();


		YAML::Node skyboxNode = data["Skybox"];
		fs::path directory = skyboxNode["Directory"].as<std::string>();
		std::string extension = skyboxNode["Extension"].as<std::string>();
		newScene->Skybox = Skybox::Create(directory, extension);


		YAML::Node directionalLightNodes = data["DirectionalLights"];
		for (YAML::Node directionalLightNode : directionalLightNodes)
		{
			glm::vec3 direction = directionalLightNode["Direction"].as<glm::vec3>();
			glm::vec3 diffuse = directionalLightNode["Diffuse"].as<glm::vec3>();

			float nearPlane = directionalLightNode["NearPlane"].as<float>();
			float farPlane = directionalLightNode["FarPlane"].as<float>();
			
			Ref<DirectionalLight> light = DirectionalLight::Create(direction, diffuse, nearPlane, farPlane);
			newScene->DirectionalLights.push_back(light);
		}


		YAML::Node pointLightNodes = data["PointLights"];
		for (YAML::Node pointLightNode : pointLightNodes)
		{
			glm::vec3 position = pointLightNode["Position"].as<glm::vec3>();
			glm::vec3 diffuse = pointLightNode["Diffuse"].as<glm::vec3>();

			float nearPlane = pointLightNode["NearPlane"].as<float>();
			float farPlane = pointLightNode["FarPlane"].as<float>();

			Ref<PointLight> light = PointLight::Create(position, diffuse, nearPlane, farPlane);
			newScene->PointLights.push_back(light);
		}


		YAML::Node modelNodes = data["Models"];
		for (YAML::Node modelNode : modelNodes)
		{
			std::string name = modelNode["Name"].as<std::string>();
			std::string pathString = modelNode["Path"].as<std::string>();
			fs::path path = fs::path(pathString);

			Ref<Model> model = Model::Create(path);
			model->SetName(name);

			newScene->Models.push_back(model);
		}


		YAML::Node gameObjectNodes = data["GameObjects"];
		for (YAML::Node gameObjectNode : gameObjectNodes)
		{
			std::string name = gameObjectNode["Name"].as<std::string>();
			std::string modelName = gameObjectNode["ModelName"].as<std::string>();

			glm::vec3 position, scale, rotation;

			YAML::Node transformComponentNode = gameObjectNode["Transform"];
			if (transformComponentNode)
			{
				position = transformComponentNode["Position"].as<glm::vec3>();
				scale = transformComponentNode["Scale"].as<glm::vec3>();
				rotation = transformComponentNode["Rotation"].as<glm::vec3>();
			}

			Ref<Model> model = newScene->GetModelByName(modelName);

			Ref<GameObject> gameObject = GameObject::Create(model, position, scale, rotation);
			gameObject->SetName(name);

			newScene->GameObjects.push_back(gameObject);
		}


		return newScene;
	}
}
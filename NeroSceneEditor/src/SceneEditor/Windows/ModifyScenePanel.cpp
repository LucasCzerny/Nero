#include "ModifyScenePanel.h"

#include "SceneEditor/ImCustom/FileDialog.h"

namespace ImCustom
{
	void DrawModifyScenePanel(const Ref<Scene> currentScene)
	{
		ImGui::Begin("Game Objects");

		if (ImGui::CollapsingHeader("Models", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("+##model"))
			{
				fs::path filePath = FileDialog::OpenFileDialog(FILE_DIALOG_FILTER("Model file (.gltf, .glb)", "*.gltf;*.glb"), Util::GetAssetDirectory());

				if (!filePath.empty())
				{
					Ref<Model> model = Model::Create(filePath);
					std::string name = filePath.stem().string();
					model->SetName(name);

					currentScene->Models.push_back(model);
				}
			}

			if (!currentScene->Models.empty())
				DrawModels(currentScene);
		}

		if (ImGui::CollapsingHeader("Game Objects", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (!currentScene->GameObjects.empty())
				DrawGameObjects(currentScene);

			ImGui::Button("Drag model here");

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelRef");

				if (payload != nullptr)
				{
					Ref<Model> model = *(Ref<Model>*)payload->Data;
					Ref<GameObject> gameObject = GameObject::Create(model, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });

					gameObject->SetName(model->GetName());

					currentScene->GameObjects.push_back(gameObject);
				}

				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::CollapsingHeader("Directional Lights", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("+##dl"))
				currentScene->DirectionalLights.push_back(DirectionalLight::Create({ 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));

			if (!currentScene->DirectionalLights.empty())
				DrawDirectionalLights(currentScene);
		}

		if (ImGui::CollapsingHeader("Point Lights", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("+##pl"))
				currentScene->PointLights.push_back(PointLight::Create({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }));

			if (!currentScene->PointLights.empty())
				DrawPointLights(currentScene);
		}

		if (ImGui::CollapsingHeader("Skybox", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::Button("Select File"))
			{
				fs::path filePath = FileDialog::OpenFileDialog(FILE_DIALOG_FILTER("Image file (.png, .jpg)", "*.png;*.jpg"), Util::GetAssetDirectory());

				if (!filePath.empty())
				{
					fs::path directory = filePath.parent_path();
					std::string extension = filePath.extension().string();

					Ref<Skybox> newSkybox = Skybox::Create(directory, extension);
					currentScene->Skybox = newSkybox;
				}
			}

			if (currentScene->Skybox != nullptr)
				DrawSkybox(currentScene);
		}

		ImGui::End();
	}

	void DrawModels(const Ref<Scene> currentScene)
	{
		for (int i = 0; i < currentScene->Models.size(); i++)
		{
			Ref<Model> model = currentScene->Models[i];

			std::string label = model->GetName();

			ImGui::Button(label.c_str());

			static char tagBuffer[MAX_PATH];
			memset(tagBuffer, '\0', MAX_PATH);
			strcpy_s(tagBuffer, MAX_PATH, label.c_str());

			if (ImGui::IsItemHovered())
			{
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("ModelRef", &model, sizeof(Ref<Model>), ImGuiCond_Once);

					ImGui::EndDragDropSource();
				}
			}

			std::stringstream inputLabel;
			inputLabel << "##model";
			inputLabel << i;

			ImGui::Text("Rename: ");
			ImGui::SameLine();

			if (ImGui::InputText(inputLabel.str().c_str(), tagBuffer, MAX_PATH))
			{
				model->SetName(tagBuffer);
			}
		}
	}

	void DrawGameObjects(const Ref<Scene> currentScene)
	{
		for (int i = 0; i < currentScene->GameObjects.size(); i++)
		{
			Ref<GameObject> gameObject = currentScene->GameObjects[i];

			std::string label = gameObject->GetName();

			ImGui::Button(label.c_str());

			static char tagBuffer[MAX_PATH];
			memset(tagBuffer, '\0', MAX_PATH);
			strcpy_s(tagBuffer, MAX_PATH, label.c_str());

			std::stringstream inputLabel;
			inputLabel << "##gameObject";
			inputLabel << i;

			ImGui::Text("Rename: ");
			ImGui::SameLine();

			if (ImGui::InputText(inputLabel.str().c_str(), tagBuffer, MAX_PATH))
			{
				gameObject->SetName(tagBuffer);
			}

			std::stringstream positionLabel;
			positionLabel << "Position##";
			positionLabel << i;
			bool positionChanged = ImGui::DragFloat3(positionLabel.str().c_str(), gameObject->GetPositionPointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			std::stringstream scaleLabel;
			scaleLabel << "Scale##";
			scaleLabel << i;
			bool scaleChanged = ImGui::DragFloat3(scaleLabel.str().c_str(), gameObject->GetScalePointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			std::stringstream rotationLabel;
			rotationLabel << "Rotation##";
			rotationLabel << i;
			bool rotationChanged = ImGui::DragFloat3(rotationLabel.str().c_str(), gameObject->GetRotationPointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			if (positionChanged || scaleChanged || rotationChanged)
				gameObject->HasMoved();
		}
	}

	void DrawDirectionalLights(const Ref<Scene> currentScene)
	{
		const std::vector<Ref<DirectionalLight>>& directionalLights = currentScene->DirectionalLights;

		for (int i = 0; i < directionalLights.size(); i++)
		{
			std::string headerLabel = "DirectionalLight #" + std::to_string(i);

			ImGui::Text(headerLabel.c_str());

			const Ref<DirectionalLight> directionalLight = directionalLights[i];

			std::stringstream directionLabel;
			directionLabel << "LightDirection##";
			directionLabel << i;
			bool directionChanged = ImGui::DragFloat3(directionLabel.str().c_str(), directionalLight->GetDirectionPointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			std::stringstream diffuseLabel;
			diffuseLabel << "DirectionalDiffuse##";
			diffuseLabel << i;
			ImGui::DragFloat3(diffuseLabel.str().c_str(), directionalLight->GetDiffusePointer(), 0.1f, 0.0f, 1.0f, "%.01f");

			std::stringstream nearPlaneLabel;
			nearPlaneLabel << "DirectionalNear##";
			nearPlaneLabel << i;
			bool nearPlaneChanged = ImGui::DragFloat(nearPlaneLabel.str().c_str(), directionalLight->GetNearPlanePointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			std::stringstream farPlaneLabel;
			farPlaneLabel << "DirectionalFar##";
			farPlaneLabel << i;
			bool farPlaneChanged = ImGui::DragFloat(farPlaneLabel.str().c_str(), directionalLight->GetFarPlanePointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			if (directionChanged || nearPlaneChanged || farPlaneChanged)
				directionalLight->CalculateLightSpaceTransform();
		}
	}

	void DrawPointLights(const Ref<Scene> currentScene)
	{
		const std::vector<Ref<PointLight>>& pointLights = currentScene->PointLights;

		for (int i = 0; i < pointLights.size(); i++)
		{
			std::string headerLabel = "PointLight #" + std::to_string(i);

			ImGui::Text(headerLabel.c_str());

			const Ref<PointLight> pointLight = pointLights[i];

			std::stringstream positionLabel;
			positionLabel << "LightPosition##";
			positionLabel << i;
			bool positionChanged = ImGui::DragFloat3(positionLabel.str().c_str(), pointLight->GetPositionPointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			std::stringstream diffuseLabel;
			diffuseLabel << "PointDiffuse##";
			diffuseLabel << i;
			ImGui::DragFloat3(diffuseLabel.str().c_str(), pointLight->GetDiffusePointer(), 0.1f, 0.0f, 1.0f, "%.01f");

			std::stringstream nearPlaneLabel;
			nearPlaneLabel << "PointNear##";
			nearPlaneLabel << i;
			bool nearPlaneChanged = ImGui::DragFloat(nearPlaneLabel.str().c_str(), pointLight->GetNearPlanePointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			std::stringstream farPlaneLabel;
			farPlaneLabel << "PointFar##";
			farPlaneLabel << i;
			bool farPlaneChanged = ImGui::DragFloat(farPlaneLabel.str().c_str(), pointLight->GetFarPlanePointer(), 0.1f, 0.0f, 0.0f, "%.2f");

			if (positionChanged || nearPlaneChanged || farPlaneChanged)
				pointLight->CalculateLightSpaceTransforms();
		}
	}

	void DrawSkybox(const Ref<Scene> currentScene)
	{
	}
}


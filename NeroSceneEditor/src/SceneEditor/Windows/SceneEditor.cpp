#include "SceneEditor.h"

#include "SceneEditor/ImCustom/FileDialog.h"
#include "ModifyScenePanel.h"

#include "Model/Model.h"
#include "Gameobject/Gameobject.h"

#include "Scene/SceneSerializer.h"

using namespace Nero;

namespace ImCustom
{
	static Ref<Scene> currentScene = Scene::Create();

	const Ref<Scene> DrawSceneEditor(int32_t frameId)
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		static bool open = true;

		if (!ImGui::Begin("Scene Editor", &open, windowFlags))
		{
			ImGui::End();
			return currentScene;
		}

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Scene"))
			{
				ImGui::MenuItem("New Scene");
				if (ImGui::MenuItem("Load Scene"))
				{
					fs::path filePath = FileDialog::OpenFileDialog(FILE_DIALOG_FILTER("Scene file (.nero_scene)", "*.nero_scene"), Util::GetAssetDirectory());

					if (!filePath.empty())
						currentScene = DeserializeScene(filePath);
				}

				if (ImGui::MenuItem("Save Scene As"))
				{
					fs::path filePath = FileDialog::SaveFileDialog(FILE_DIALOG_FILTER("Scene file (.nero_scene)", "*.nero_scene"), Util::GetAssetDirectory());

					if (!filePath.empty())
						SerializeScene(currentScene, filePath);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();


		ImGui::Begin("Viewport");

		// No tab bar
		if (ImGui::IsWindowDocked())
		{
			auto* window = ImGui::FindWindowByName("Viewport");
			if (window)
			{
				ImGuiDockNode* node = window->DockNode;
				node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
			}
		}

		static glm::vec2 lastViewportSize(0.0f, 0.0f);

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();

		if (lastViewportSize != (glm::vec2&)viewportSize)
		{
			lastViewportSize = { viewportSize.x, viewportSize.y };
			
			Application::ExternalViewportResize(viewportSize.x, viewportSize.y);
		}
		
		ImGui::Image(ImTextureID(frameId), viewportSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();

		DrawModifyScenePanel(currentScene);

		return currentScene;
	}
}
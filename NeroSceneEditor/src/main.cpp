#include <Nero.h>

#include "Scene/SceneSerializer.h"

#include "SceneEditor/ImCustom/ImGuiAbstraction.h"
#include "SceneEditor/ImCustom/FileDialog.h"
#include "SceneEditor/Windows/SceneEditor.h"

using namespace Nero;

int main()
{
	Application::Create();
	ImCustom::CreateContext(Application::GetGlfwWindow());

	Ref<Camera> cam = EditorCamera::Create({ 1.0f, 1.5f, 0.0f }, { glm::radians(-10.0f), glm::radians(70.0f), 0.0f });

	Renderer::SetCamera(cam);

	while (Application::IsOpen())
	{
		int32_t frameId = Renderer::DrawExternal();

		ImCustom::NewFrame();

		const Ref<Scene> editorScene = ImCustom::DrawSceneEditor(frameId);

		Renderer::SetScene(editorScene);
		Renderer::DrawScene();

		glm::vec2 size = Application::GetWindowSize();
		ImCustom::RenderFrame(size.x, size.y);

		Application::OnUpdate();

		cam->OnUpdate(Time::DeltaTime);
	}

	ImCustom::DestroyContext();
	Application::Destroy();
}
#include <Nero.h>

using namespace Nero;

int main()
{
	Application::Create();

	Ref<Scene> myScene = Scene::Create("C:\\Users\\Lucas\\Desktop\\sachen\\sponza2.nero_scene");
	Ref<Camera> cam = EditorCamera::Create({ 1.0f, 1.5f, 0.0f }, { glm::radians(-10.0f), glm::radians(70.0f), 0.0f});

	Renderer::SetScene(myScene);
	Renderer::SetCamera(cam);

	while (Application::IsOpen())
	{
		Renderer::DrawScene();
		Renderer::DrawToScreen();

		Application::OnUpdate();

		cam->OnUpdate(Time::DeltaTime);
	}

	Application::Destroy();
}
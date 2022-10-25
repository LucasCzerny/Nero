#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "RenderParameters.h"
#include "RenderData.h"

#include "OpenGl/Shader/Shader.h"

#include "Scene/Scene.h"
#include "Camera/Camera.h"

namespace Nero
{
	class Renderer
	{
	private:
		static RenderParameters s_Parameters;
		static ViewData s_View;

		static Ref<Scene> s_Scene;

		static ShaderData s_Shaders;
		static FramebufferData s_Framebuffers;
		static TextureData s_Textures;
		
	public:
		static void Init(uint32_t width, uint32_t height);

		static void SetScene(const Ref<Scene> scene);
		static void SetCamera(const Ref<Camera> camera);

		static void DrawScene();

		static void DrawToScreen();
		static int32_t DrawExternal();

		static Ref<Cubemap> CreateIrradianceMap(const Ref<Cubemap> skybox);
		static Ref<Cubemap> PrefilerEnvironmentMap(const Ref<Cubemap> skybox);

	private:
		// Current Render Pipeline

		static void PrecalculateBrdfLut();

		static void FillGBuffer();
	
		static void FillShadowMap(const Ref<DirectionalLight> directionalLight, int32_t index);
		static void FillPointLightShadowMap(const Ref<PointLight> pointLight, int32_t index);

		static void Ssao();
		static void CalculateSsao();
		static void BlurSsao();

		static void DrawPbr();

		static void PostProcessing();
		static void Bloom();
		static void GodRays();

		static void DrawFinalFrame();

		// Generic Functions

		static void IterateOverGameObjects(std::function<void(const Ref<Model>, const glm::mat4&)> function);
		static void IterateOverMeshes(const Ref<Model> model, std::function<void(const Mesh&)> function);

		static void DrawSkybox();

		static void DrawQuad();
		static void DrawQuad(const Ref<Shader> shader);
		
		static void DrawCube();
		static void DrawCube(const Ref<Shader> shader);

		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();

		static void DrawArrays(uint32_t count);
		static void DrawElements(uint32_t count);
	};
}
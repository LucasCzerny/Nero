#include "Renderer.h"

#include "Application/Application.h"
#include "OpenGL/Setup.h"

#include "Scene/SceneSerializer.h"

#include "Settings/Settings.h"

namespace Nero
{
	RenderParameters Renderer::s_Parameters;
	ViewData Renderer::s_View;

	Ref<Scene> Renderer::s_Scene;
	TextureData Renderer::s_Textures;
	ShaderData Renderer::s_Shaders;
	FramebufferData Renderer::s_Framebuffers;

	void Renderer::Init(uint32_t width, uint32_t height)
	{
		FUNCTION_TIMER;

		s_View.Width = width, s_View.Height = height;

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Assert::Unreachable("Failed to initialize GLAD");
		}

		Util::OpenGlSettings();

		Application::OnResize([](int32_t width, int32_t height)
		{
			s_View.Width = width;
			s_View.Height = height;

			s_View.Camera->SetViewportSize(s_View.Width, s_View.Height);

			s_Framebuffers.Resize(width, height);
			s_Textures.Resize(width, height);

			glViewport(0, 0, s_View.Width, s_View.Height);
		});

		static unsigned char data[4] = { 0, 0, 255, 255 };
		Texture2D::BlueTexture = Texture2D::Create(1, 1, TextureFormat::RGBA8, data);

		s_Shaders.Intialize();
		s_Framebuffers.Intialize(s_View.Width, s_View.Height);
		s_Textures.Intialize(s_View.Width, s_View.Height);

		PrecalculateBrdfLut();
	}

	void Renderer::SetScene(const Ref<Scene> scene)
	{
		FUNCTION_TIMER;

		s_Scene = scene;
	}

	void Renderer::SetCamera(const Ref<Camera> camera)
	{
		FUNCTION_TIMER;

		s_View.Camera = camera;
		s_View.Camera->SetViewportSize(s_View.Width, s_View.Height);
	}

	void Renderer::DrawScene()
	{
		FUNCTION_TIMER;

		if (s_Scene->GameObjects.empty())
			return;

		FillGBuffer();

		const std::vector<Ref<DirectionalLight>>& directionalLights = s_Scene->DirectionalLights;
		int32_t shadowMaps = std::min(2, (int32_t)directionalLights.size());

		for (int32_t i = 0; i < shadowMaps; i++)
		{
			FillShadowMap(directionalLights[i], i);
		}

		const std::vector<Ref<PointLight>>& pointLights = s_Scene->PointLights;
		int32_t pointLightshadowMaps = std::min(5, (int32_t)pointLights.size());

		for (int32_t i = 0; i < pointLightshadowMaps; i++)
		{
			FillPointLightShadowMap(pointLights[i], i);
		}

		Ssao();

		DrawPbr();

		PostProcessing();
	}

	void Renderer::DrawToScreen()
	{
		FUNCTION_TIMER;

		DrawFinalFrame();
	}

	int32_t Renderer::DrawExternal()
	{
		FUNCTION_TIMER;

		s_Framebuffers.OutputFramebuffer->Bind();

		DrawFinalFrame();

		s_Framebuffers.OutputFramebuffer->Unbind();

		return s_Framebuffers.OutputFramebuffer->GetColorAttachmentRendererID();
	}

	Ref<Cubemap> Renderer::CreateIrradianceMap(const Ref<Cubemap> skybox)
	{
		FUNCTION_TIMER;

		Ref<Cubemap> result = Cubemap::Create(32, 32, false);

		static glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		static glm::mat4 views[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		s_Framebuffers.CubemapCaptureFramebuffer->Bind();

		s_Shaders.IrradianceMapShader->Bind();
		s_Shaders.IrradianceMapShader->SetMat4("u_ProjectionMatrix", projection);

		for (uint32_t i = 0; i < 6; i++)
		{
			s_Shaders.IrradianceMapShader->SetMat4("u_ViewMatrices[" + std::to_string(i) + "]", views[i]);
		}

		skybox->Bind(0);
		s_Shaders.IrradianceMapShader->SetInt("u_Skybox", 0);

		uint32_t irradianceMapId = result->GetRendererID();

		s_Framebuffers.CubemapCaptureFramebuffer->SetSpecification(32, 32, GL_DEPTH_COMPONENT24);
		s_Framebuffers.CubemapCaptureFramebuffer->SetTarget(GL_COLOR_ATTACHMENT0, irradianceMapId, 0);

		Clear();
		DrawCube(s_Shaders.IrradianceMapShader);

		s_Framebuffers.CubemapCaptureFramebuffer->Unbind();

		return result;
	}

	Ref<Cubemap> Renderer::PrefilerEnvironmentMap(const Ref<Cubemap> skybox)
	{
		FUNCTION_TIMER;

		Ref<Cubemap> result = Cubemap::Create(128, 128, true);
		result->GenerateMipMap();

		static glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		static glm::mat4 views[] =
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		s_Framebuffers.CubemapCaptureFramebuffer->Bind();

		s_Shaders.PrefilterEnvironmentMapShader->Bind();
		s_Shaders.PrefilterEnvironmentMapShader->SetMat4("u_ProjectionMatrix", projection);

		for (uint32_t i = 0; i < 6; i++)
		{
			s_Shaders.PrefilterEnvironmentMapShader->SetMat4("u_ViewMatrices[" + std::to_string(i) + "]", views[i]);
		}

		skybox->Bind(0);
		s_Shaders.PrefilterEnvironmentMapShader->SetInt("u_Skybox", 0);

		uint32_t prefilteredEnvironmentMapId = result->GetRendererID();

		constexpr uint32_t maxMipLevels = 5;
		for (uint32_t mip = 0; mip < maxMipLevels; mip++)
		{
			uint32_t mipSize = 128 * std::pow(0.5, mip);

			s_Framebuffers.CubemapCaptureFramebuffer->SetSpecification(mipSize, mipSize, GL_DEPTH_COMPONENT24);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			s_Shaders.PrefilterEnvironmentMapShader->SetFloat("u_Roughness", roughness);

			s_Framebuffers.CubemapCaptureFramebuffer->SetTarget(GL_COLOR_ATTACHMENT0, prefilteredEnvironmentMapId, mip);

			Clear();
			DrawCube(s_Shaders.PrefilterEnvironmentMapShader);
		}

		s_Framebuffers.CubemapCaptureFramebuffer->Unbind();

		return result;
	}

	void Renderer::PrecalculateBrdfLut()
	{
		FUNCTION_TIMER;

		s_Framebuffers.PrecalculateBrdfFramebuffer->Bind();

		DrawQuad(s_Shaders.PrecalculateBrdfShader);

		s_Framebuffers.PrecalculateBrdfFramebuffer->Unbind();
	}

	void Renderer::FillGBuffer()
	{
		FUNCTION_TIMER;

		s_Framebuffers.GBufferFramebuffer->Bind();

		s_Shaders.GBufferShader->Bind();
		s_Shaders.GBufferShader->Bind();

		s_Shaders.GBufferShader->SetBool("u_UseNormalMapping", Settings::UseNormalMapping);

		s_Shaders.GBufferShader->SetMat4("u_ViewProjectionMatrix", s_View.Camera->GetViewProjectionMatrix());
		// s_Shaders.GBufferShader->SetFloat3("u_ViewPosition", s_View.Camera->GetViewPosition());

		const std::vector<Ref<PointLight>> pointLights = s_Scene->PointLights;
		const std::vector<Ref<DirectionalLight>> directionalLights = s_Scene->DirectionalLights;

		IterateOverGameObjects([](const Ref<Model> model, const glm::mat4& transform)
		{
			s_Shaders.GBufferShader->SetMat4("u_ModelMatrix", transform);

			IterateOverMeshes(model, [](const Mesh& mesh)
			{
				mesh.GetAlbedoMap()->Bind(0);
				s_Shaders.GBufferShader->SetInt("u_AlbedoMap", 0);
				
				mesh.GetNormalMap()->Bind(1);
				s_Shaders.GBufferShader->SetInt("u_NormalMap", 1);

				mesh.GetMetallicRoughnessMap()->Bind(2);
				s_Shaders.GBufferShader->SetInt("u_MetallicRoughnessMap", 2);

				mesh.BindVertexArray();
				DrawElements(mesh.GetIndexCount());
			});
		});

		s_Framebuffers.GBufferFramebuffer->Unbind();
	}
	void Renderer::FillShadowMap(const Ref<DirectionalLight> directionalLight, int32_t index)
	{
		FUNCTION_TIMER;

		s_Framebuffers.ShadowMapFramebuffers[index]->Bind();

		Clear();

		s_Shaders.ShadowMapShader->Bind();

		s_Shaders.ShadowMapShader->SetMat4("u_LightSpaceTransform", directionalLight->GetLightSpaceTransform());

		IterateOverGameObjects([](const Ref<Model> model, const glm::mat4& transform)
		{
			s_Shaders.ShadowMapShader->SetMat4("u_ModelMatrix", transform);

			IterateOverMeshes(model, [](const Mesh& mesh)
			{
				mesh.GetAlbedoMap()->Bind(0);
				s_Shaders.ShadowMapShader->SetInt("u_AlbedoMap", 0);

				mesh.BindVertexArray();
				DrawElements(mesh.GetIndexCount());
			});
		});

		s_Framebuffers.ShadowMapFramebuffers[index]->Unbind();
	}

	void Renderer::FillPointLightShadowMap(const Ref<PointLight> pointLight, int32_t index)
	{
		FUNCTION_TIMER;

		s_Framebuffers.PointLightShadowMapFramebuffers[index]->Bind();

		s_Shaders.PointLightShadowShader->Bind();

		s_Shaders.PointLightShadowShader->SetFloat3("u_LightPosition", pointLight->GetPosition());
		s_Shaders.PointLightShadowShader->SetFloat("u_FarPlane", pointLight->GetFarPlane());

		const std::array<glm::mat4, 6>& lightSpaceTransforms = pointLight->GetLightSpaceTransforms();
		for (int i = 0; i < 6; i++)
		{
			s_Shaders.PointLightShadowShader->SetMat4("u_LightSpaceTransforms[" + std::to_string(i) + "]", lightSpaceTransforms[i]);
		}

		IterateOverGameObjects([](const Ref<Model> model, const glm::mat4& transform)
		{
			s_Shaders.PointLightShadowShader->SetMat4("u_ModelMatrix", transform);

			IterateOverMeshes(model, [](const Mesh& mesh)
			{
				mesh.GetAlbedoMap()->Bind(0);
				s_Shaders.PointLightShadowShader->SetInt("u_AlbedoMap", 0);

				mesh.BindVertexArray();
				DrawElements(mesh.GetIndexCount());
			});
		});

		s_Framebuffers.PointLightShadowMapFramebuffers[index]->Unbind();
	}

	void Renderer::Ssao()
	{
		FUNCTION_TIMER;

		CalculateSsao();
		BlurSsao();
	}

	void Renderer::CalculateSsao()
	{
		FUNCTION_TIMER;

		std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
		std::default_random_engine generator;

		std::vector<glm::vec3> ssaoKernel;
		ssaoKernel.reserve(64);

		for (uint32_t i = 0; i < 64; i++)
		{
			glm::vec3 sample(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator)
			);

			float scale = (float)i * 0.015625f; // 1 / 64
			scale = Util::Lerp(0.1f, 1.0f, scale * scale);

			sample = glm::normalize(sample);
			sample *= scale;

			ssaoKernel.push_back(sample);
		}

		std::vector<glm::vec3> ssaoNoise;
		ssaoNoise.reserve(16);

		for (uint32_t i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f);

			ssaoNoise.push_back(noise);
		}

		Ref<Texture2D> noiseTexture = Texture2D::Create(4, 4, TextureFormat::RGBA16F, &ssaoNoise[0]);

		s_Framebuffers.SsaoFramebuffer->Bind();

		s_Shaders.SsaoShader->Bind();

		for (uint32_t i = 0; i < 64; i++)
		{
			s_Shaders.SsaoShader->SetFloat3("u_SsaoKernel[" + std::to_string(i) + "]", ssaoKernel[i]);
		}

		s_Shaders.SsaoShader->SetMat4("u_ViewProjectionMatrix", s_View.Camera->GetViewProjectionMatrix());

		s_Shaders.SsaoShader->SetFloat("u_ScreenWidth", s_View.Width);
		s_Shaders.SsaoShader->SetFloat("u_ScreenHeight", s_View.Height);

		int32_t fragmentId = s_Framebuffers.GBufferFramebuffer->GetColorAttachmentRendererID(0);
		Texture2D::Bind(fragmentId, 0);
		s_Shaders.SsaoShader->SetInt("u_FragmentPosition", 0);

		int32_t normalId = s_Framebuffers.GBufferFramebuffer->GetColorAttachmentRendererID(1);
		Texture2D::Bind(normalId, 1);
		s_Shaders.SsaoShader->SetInt("u_Normal", 1);

		noiseTexture->Bind(2);
		s_Shaders.SsaoShader->SetInt("u_NoiseTexture", 2);

		DrawQuad(s_Shaders.SsaoShader);

		s_Framebuffers.SsaoFramebuffer->Unbind();
	}

	void Renderer::BlurSsao()
	{
		FUNCTION_TIMER;

		int32_t ssaoId = s_Framebuffers.SsaoFramebuffer->GetColorAttachmentRendererID();
		Texture2D::Bind(ssaoId, 0);

		s_Shaders.SsaoBlurShader->Bind();
		s_Shaders.SsaoBlurShader->SetInt("u_SsaoImage", 0);

		s_Framebuffers.SsaoBlurFramebuffer->Bind();

		DrawQuad(s_Shaders.SsaoBlurShader);

		s_Framebuffers.SsaoBlurFramebuffer->Unbind();
	}

	void Renderer::DrawPbr()
	{
		FUNCTION_TIMER;

		s_Framebuffers.PbrFramebuffers[s_Framebuffers.PbrIndex]->Bind();

		if (s_Scene->Skybox)
			DrawSkybox();

		s_Shaders.PbrShader->Bind();

		static const char* uniformNames[4] = { "u_FragmentPosition", "u_Normal", "u_Albedo", "u_MetallicRoughness" };

		for (int i = 0; i < 4; i++)
		{
			Texture2D::Bind(s_Framebuffers.GBufferFramebuffer->GetColorAttachmentRendererID(i), i);
			s_Shaders.PbrShader->SetInt(uniformNames[i], i);
		}
		
		s_Shaders.PbrShader->SetFloat3("u_ViewPosition", s_View.Camera->GetViewPosition());
		s_Shaders.PbrShader->SetMat4("u_ProjectionMatrix", s_View.Camera->GetProjectionMatrix());
		s_Shaders.PbrShader->SetMat4("u_ViewMatrix", s_View.Camera->GetViewMatrix());
		s_Shaders.PbrShader->SetMat4("u_InvViewMatrix", glm::inverse(s_View.Camera->GetViewMatrix()));

		if (s_Scene->Skybox != nullptr)
		{
			s_Scene->Skybox->GetIrradianceMap()->Bind(4);
			s_Scene->Skybox->GetPrefilteredEnvironmentMap()->Bind(5);
		}

		s_Shaders.PbrShader->SetInt("u_IrradianceMap", 4);
		s_Shaders.PbrShader->SetInt("u_PrefilteredEnvironmentMap", 5);

		static int32_t brdfId = s_Framebuffers.PrecalculateBrdfFramebuffer->GetColorAttachmentRendererID();
		Texture2D::Bind(brdfId, 6);
		s_Shaders.PbrShader->SetInt("u_BrdfLut", 6);

		Texture2D::Bind(s_Framebuffers.SsaoBlurFramebuffer->GetColorAttachmentRendererID(), 7);
		s_Shaders.PbrShader->SetInt("u_Ssao", 7);
	
		Texture2D::Bind(s_Framebuffers.PbrFramebuffers[!s_Framebuffers.PbrIndex]->GetColorAttachmentRendererID(), 8);
		s_Shaders.PbrShader->SetInt("u_PreviousFrame", 8);
		
		int32_t currentIndex = 9;

		const std::vector<Ref<DirectionalLight>>& directionalLights = s_Scene->DirectionalLights;

		int32_t numDirectionalLights = directionalLights.size();
		s_Shaders.PbrShader->SetInt("u_NumDirectionalLights", numDirectionalLights);

		for (int32_t i = 0; i < numDirectionalLights; i++)
		{
			bool isShadowCaster = i < 2;

			s_Shaders.PbrShader->SetDirectionalLight("u_DirectionalLights[" + std::to_string(i) + "]", directionalLights[i], isShadowCaster);

			if (isShadowCaster)
			{
				Texture2D::Bind(s_Framebuffers.ShadowMapFramebuffers[i]->GetDepthAttachmentRendererID(), currentIndex);

				s_Shaders.PbrShader->SetInt("u_ShadowMaps[" + std::to_string(i) + "]", currentIndex);
				currentIndex++;
			}
		}

		const std::vector<Ref<PointLight>>& pointLights = s_Scene->PointLights;

		int32_t numPointLights = pointLights.size();
		s_Shaders.PbrShader->SetInt("u_NumPointLights", numPointLights);

		for (int32_t i = 0; i < numPointLights; i++)
		{
			bool isShadowCaster = i < 5;

			s_Shaders.PbrShader->SetPointLight("u_PointLights[" + std::to_string(i) + "]", pointLights[i], isShadowCaster);
		
			if (isShadowCaster)
			{
				Cubemap::Bind(s_Framebuffers.PointLightShadowMapFramebuffers[i]->GetDepthAttachmentRendererID(), currentIndex);

				s_Shaders.PbrShader->SetInt("u_PointLightShadowMaps[" + std::to_string(i) + "]", currentIndex);
				currentIndex++;
			}
		}

		DrawQuad(s_Shaders.PbrShader);
		s_Framebuffers.PbrFramebuffers[s_Framebuffers.PbrIndex]->Unbind();

		s_Framebuffers.PbrIndex = !s_Framebuffers.PbrIndex;
	}

	void Renderer::PostProcessing()
	{
		FUNCTION_TIMER;

		Bloom();
		GodRays();
	}

	void Renderer::Bloom()
	{
		FUNCTION_TIMER;

		s_Framebuffers.CaptureFramebuffer->Bind();

		s_Shaders.DownsampleShader->Bind();

		int32_t imageId = s_Framebuffers.PbrFramebuffers[!s_Framebuffers.PbrIndex]->GetColorAttachmentRendererID();
		Texture2D::Bind(imageId, 0);

		s_Shaders.DownsampleShader->SetInt("u_Image", 0);
		s_Shaders.DownsampleShader->SetFloat2("u_ImageResolution", glm::vec2(s_View.Width, s_View.Height));

		for (int i = 0; i < s_Textures.BloomMips.size(); i++)
		{
			const Ref<Texture2D> mip = s_Textures.BloomMips[i];
			float width = mip->GetWidth();
			float height = mip->GetHeight();

			s_Framebuffers.CaptureFramebuffer->SetSpecification(width, height, GL_DEPTH_COMPONENT24);
			glViewport(0, 0, width, height);

			s_Framebuffers.CaptureFramebuffer->SetTarget(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip->GetRendererID());

			DrawQuad(s_Shaders.DownsampleShader);

			s_Shaders.DownsampleShader->SetFloat2("u_ImageResolution", glm::vec2(width, height));
		}

		s_Shaders.UpsampleShader->Bind();

		s_Shaders.UpsampleShader->SetInt("u_Image", 0);
		s_Shaders.UpsampleShader->SetFloat("u_FilterRadius", 0.005f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		for (int i = s_Textures.BloomMips.size() - 1; i > 0; i--)
		{
			const Ref<Texture2D> mip = s_Textures.BloomMips[i];

			const Ref<Texture2D> nextMip = s_Textures.BloomMips[i - 1];
			float width = nextMip->GetWidth();
			float height = nextMip->GetHeight();

			mip->Bind(0);

			s_Framebuffers.CaptureFramebuffer->SetSpecification(width, height, GL_DEPTH_COMPONENT24);
			glViewport(0, 0, width, height);

			s_Framebuffers.CaptureFramebuffer->SetTarget(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip->GetRendererID());

			DrawQuad(s_Shaders.UpsampleShader);
		}

		glDisable(GL_BLEND);

		s_Framebuffers.CaptureFramebuffer->Unbind();
	}

	void Renderer::GodRays()
	{
		FUNCTION_TIMER;

		s_Framebuffers.OcclusionMapFramebuffer->Bind();

		s_Shaders.OcclusionMapShader->Bind();

		Texture2D::Bind(s_Framebuffers.GBufferFramebuffer->GetDepthAttachmentRendererID(), 0);
		s_Shaders.OcclusionMapShader->SetInt("u_Depth", 0);

		DrawQuad(s_Shaders.OcclusionMapShader);

		s_Framebuffers.OutputFramebuffer->Unbind();

		s_Framebuffers.GodRaysFramebuffer->Bind();

		s_Shaders.GodRaysShader->Bind();

		s_Shaders.GodRaysShader->SetMat4("u_ViewProjectionMatrix", s_View.Camera->GetProjectionMatrix());

		Texture2D::Bind(s_Framebuffers.OcclusionMapFramebuffer->GetColorAttachmentRendererID(), 0);
		s_Shaders.GodRaysShader->SetInt("u_OcclusionMap", 0);

		const std::vector<Ref<DirectionalLight>>& directionalLights = s_Scene->DirectionalLights;

		int32_t numDirectionalLights = directionalLights.size();

		for (int32_t i = 0; i < numDirectionalLights; i++)
		{
			const Ref<DirectionalLight> light = directionalLights[i];
			s_Shaders.GodRaysShader->SetFloat3("u_LightDirections[" + std::to_string(i) + "]", light->GetDirection());
		}

		/*const std::vector<Ref<PointLight>>& pointLights = s_Scene->PointLights;

		int32_t numPointLights = pointLights.size();
		s_Shaders.GodRaysShader->SetInt("u_NumPointLights", numPointLights);

		for (int32_t i = 0; i < numPointLights; i++)
		{
			const Ref<PointLight> light = pointLights[i];
			s_Shaders.GodRaysShader->SetFloat3("u_LightPositions[" + std::to_string(numDirectionalLights + i) + "]", light->GetPosition());
		}*/

		s_Shaders.GodRaysShader->SetInt("u_NumLights", numDirectionalLights/* + numPointLights*/);

		DrawQuad(s_Shaders.GodRaysShader);

		s_Framebuffers.GodRaysFramebuffer->Unbind();
	}

	void Renderer::DrawFinalFrame()
	{
		FUNCTION_TIMER;

		glViewport(0, 0, s_View.Width, s_View.Height);

		s_Shaders.FinalFrameShader->Bind();

		s_Shaders.FinalFrameShader->SetBool("u_UseGodRays", Settings::UseGodRays);

		Texture2D::Bind(s_Framebuffers.PbrFramebuffers[!s_Framebuffers.PbrIndex]->GetColorAttachmentRendererID(), 0);
		s_Textures.BloomMips[0]->Bind(1);
		Texture2D::Bind(s_Framebuffers.GodRaysFramebuffer->GetColorAttachmentRendererID(), 2);

		s_Shaders.FinalFrameShader->SetInt("u_Image", 0);
		s_Shaders.FinalFrameShader->SetInt("u_Bloom", 1);
		s_Shaders.FinalFrameShader->SetInt("u_GodRays", 2);

		Clear();
		DrawQuad(s_Shaders.FinalFrameShader);
	}

	void Renderer::IterateOverGameObjects(std::function<void(const Ref<Model>, const glm::mat4&)> function)
	{
		FUNCTION_TIMER;

		const std::vector<Ref<GameObject>>& gameObjects = s_Scene->GameObjects;

		for (auto it = gameObjects.begin(); it != gameObjects.end(); it++)
		{
			const Ref<Model> model = (*it)->GetModel();
			const glm::mat4& transform = (*it)->GetModelMatrix();

			function(model, transform);
		}
	}

	void Renderer::IterateOverMeshes(const Ref<Model> model, std::function<void(const Mesh&)> function)
	{
		FUNCTION_TIMER;

		const std::vector<Mesh>& meshes = model->GetMeshes();

		for (auto it = meshes.begin(); it != meshes.end(); it++)
		{
			function(*it);
		}
	}

	void Renderer::DrawSkybox()
	{
		FUNCTION_TIMER;

		glDepthMask(GL_FALSE);

		s_Shaders.SkyboxShader->Bind();

		glm::mat4 viewProjectionMatrixNoTranslation = s_View.Camera->GetProjectionMatrix() * glm::mat4(glm::mat3(s_View.Camera->GetViewMatrix()));
		s_Shaders.SkyboxShader->SetMat4("u_ViewProjectionMatrix", viewProjectionMatrixNoTranslation);

		s_Scene->Skybox->Bind(0);
		s_Shaders.SkyboxShader->SetInt("u_Skybox", 0);

		DrawCube(s_Shaders.SkyboxShader);

		glDepthMask(GL_TRUE);
	}

	void Renderer::DrawQuad()
	{
		FUNCTION_TIMER;

		DrawQuad(s_Shaders.QuadShader);
	}

	void Renderer::DrawQuad(const Ref<Shader> shader)
	{
		FUNCTION_TIMER;

		static float vertices[] = {
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 1.0f,
		};

		static uint32_t indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		static Ref<VertexArray> vertexArray = VertexArray::Create();

		static Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		static Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices));

		static BufferLayout layout = {
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoords" },
		};

		vertexArray->AddVertexBuffer(vertexBuffer, layout);
		vertexArray->SetIndexBuffer(indexBuffer);

		vertexArray->Bind();
		shader->Bind();

		DrawElements(6);
	}

	void Renderer::DrawCube()
	{
		FUNCTION_TIMER;

		DrawCube(s_Shaders.CubeShader);
	}

	void Renderer::DrawCube(const Ref<Shader> shader)
	{
		FUNCTION_TIMER;

		static float vertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		static uint32_t indices[] = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36
		};

		Ref<VertexArray> vertexArray = VertexArray::Create();

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		static Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices));

		static BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" }
		};

		vertexArray->AddVertexBuffer(vertexBuffer, layout);
		vertexArray->SetIndexBuffer(indexBuffer);

		vertexArray->Bind();
		shader->Bind();

		DrawElements(36);
	}

	void Renderer::SetClearColor(float r, float g, float b, float a)
	{
		FUNCTION_TIMER;

		glClearColor(r, g, b, a);
	}

	void Renderer::Clear()
	{
		FUNCTION_TIMER;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::DrawArrays(uint32_t count)
	{
		FUNCTION_TIMER;

		glDrawArrays(GL_TRIANGLES, 0, count);
	}

	void Renderer::DrawElements(uint32_t count)
	{
		FUNCTION_TIMER;

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
}
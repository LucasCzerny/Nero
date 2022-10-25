#pragma once

#include "Camera/Camera.h"

#include "OpenGL/Shader/Shader.h"

#include "OpenGL/Framebuffer/Framebuffer.h"
#include "OpenGL/Framebuffer/CubemapFramebuffer.h"
#include "OpenGL/Framebuffer/RenderFramebuffer.h"
#include "OpenGL/Framebuffer/CubemapRenderFramebuffer.h"

#include "OpenGL/Texture/Cubemap.h"

namespace Nero
{
	struct ViewData
	{
		int32_t Width, Height;

		Ref<Camera> Camera;
		glm::vec3 Position;
	};

	struct ShaderData
	{
		Ref<Shader> PrecalculateBrdfShader;

		Ref<Shader> GBufferShader;

		Ref<Shader> PrefilterEnvironmentMapShader;
		Ref<Shader> IrradianceMapShader;

		Ref<Shader> ShadowMapShader;
		Ref<Shader> PointLightShadowShader;

		Ref<Shader> SsaoShader;
		Ref<Shader> SsaoBlurShader;

		Ref<Shader> PbrShader;

		Ref<Shader> DownsampleShader;
		Ref<Shader> UpsampleShader;

		Ref<Shader> OcclusionMapShader;
		Ref<Shader> GodRaysShader;

		Ref<Shader> FinalFrameShader;

		Ref<Shader> SkyboxShader;

		Ref<Shader> QuadShader;
		Ref<Shader> CubeShader;

		void Intialize();
	};

	struct FramebufferData
	{
		Ref<Framebuffer> PrecalculateBrdfFramebuffer;

		Ref<Framebuffer> GBufferFramebuffer;

		Ref<RenderFramebuffer> CaptureFramebuffer;
		Ref<CubemapRenderFramebuffer> CubemapCaptureFramebuffer;

		std::array<Ref<Framebuffer>, 2> ShadowMapFramebuffers;
		std::array<Ref<CubemapFramebuffer>, 5> PointLightShadowMapFramebuffers;

		Ref<Framebuffer> SsaoFramebuffer;
		Ref<Framebuffer> SsaoBlurFramebuffer;

		bool PbrIndex;
		Ref<Framebuffer> PbrFramebuffers[2];

		Ref<Framebuffer> OcclusionMapFramebuffer;
		Ref<Framebuffer> GodRaysFramebuffer;

		Ref<Framebuffer> OutputFramebuffer;

		void Intialize(int32_t windowWidth, int32_t windowHeight);
		void Resize(int32_t windowWidth, int32_t windowHeight);
	};

	struct TextureData
	{
		std::array<Ref<Texture2D>, 5> BloomMips;

		void Intialize(int32_t windowWidth, int32_t windowHeight);
		void Resize(int32_t windowWidth, int32_t windowHeight);
	};
}

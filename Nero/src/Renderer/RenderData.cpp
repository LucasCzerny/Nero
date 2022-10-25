#include "RenderData.h"

namespace Nero
{
	void ShaderData::Intialize()
	{
		FUNCTION_TIMER;

		Util::LoadInternalAssets();

		PrecalculateBrdfShader = Shader::Create("Shaders/precalculateBrdf.shader");

		GBufferShader = Shader::Create("Shaders/gbuffer.shader");

		PrefilterEnvironmentMapShader = Shader::Create("Shaders/prefilterEnvironmentMap.shader");
		IrradianceMapShader = Shader::Create("Shaders/irradianceMap.shader");

		ShadowMapShader = Shader::Create("Shaders/shadowMap.shader");
		PointLightShadowShader = Shader::Create("Shaders/pointLightShadowMap.shader");

		SsaoShader = Shader::Create("Shaders/ssao.shader");
		SsaoBlurShader = Shader::Create("Shaders/ssaoBlur.shader");

		PbrShader = Shader::Create("Shaders/pbr.shader");

		DownsampleShader = Shader::Create("Shaders/downsample.shader");
		UpsampleShader = Shader::Create("Shaders/upsample.shader");

		OcclusionMapShader = Shader::Create("Shaders/occlusionMap.shader");
		GodRaysShader = Shader::Create("Shaders/godRays.shader");
		
		FinalFrameShader = Shader::Create("Shaders/finalFrame.shader");

		SkyboxShader = Shader::Create("Shaders/skybox.shader");

		QuadShader = Shader::Create("Shaders/quad.shader");
		CubeShader = Shader::Create("Shaders/cube.shader");

		Util::StopLoadingInternalAssets();
	}

	void FramebufferData::Intialize(int32_t windowWidth, int32_t windowHeight)
	{
		FUNCTION_TIMER;

		{
			FramebufferSpecification spec;
			spec.Width = 512;
			spec.Height = 512;

			FramebufferAttachments attachments = { TextureFormat::RGBA16F };
			spec.Attachments = attachments;

			PrecalculateBrdfFramebuffer = Framebuffer::Create(spec);
		}

		{
			FramebufferSpecification spec;
			spec.Width = windowWidth;
			spec.Height = windowHeight;

			//                                     o_FragmentPosition      o_Normal                o_Albedo              o_MetallicRoughness
			FramebufferAttachments attachments = { TextureFormat::RGBA16F, TextureFormat::RGBA16F, TextureFormat::RGBA8, TextureFormat::RGBA16F, TextureFormat::DEPTH };
			spec.Attachments = attachments;

			GBufferFramebuffer = Framebuffer::Create(spec);
		}
		
		CaptureFramebuffer = RenderFramebuffer::Create();
		CubemapCaptureFramebuffer = CubemapRenderFramebuffer::Create();

		{
			FramebufferSpecification spec;
			spec.Width = 1024;
			spec.Height = 1024;

			FramebufferAttachments attachments = { TextureFormat::RGBA8, TextureFormat::DEPTH };
			spec.Attachments = attachments;

			for (int i = 0; i < ShadowMapFramebuffers.size(); i++)
			{
				ShadowMapFramebuffers[i] = Framebuffer::Create(spec);
			}
		}

		{
			FramebufferSpecification spec;
			spec.Width = 1024;
			spec.Height = 1024;

			FramebufferAttachments attachments = { TextureFormat::RGBA8, TextureFormat::DEPTH };
			spec.Attachments = attachments;

			for (int i = 0; i < PointLightShadowMapFramebuffers.size(); i++)
			{
				PointLightShadowMapFramebuffers[i] = CubemapFramebuffer::Create(spec);
			}
		}

		{
			FramebufferSpecification spec;
			spec.Width = windowWidth;
			spec.Height = windowHeight;

			FramebufferAttachments attachments = { TextureFormat::GRAYSCALE };
			spec.Attachments = attachments;

			SsaoFramebuffer = Framebuffer::Create(spec);
			SsaoBlurFramebuffer = Framebuffer::Create(spec);
		}
		
		{
			FramebufferSpecification spec;
			spec.Width = windowWidth;
			spec.Height = windowHeight;

			FramebufferAttachments attachments = { TextureFormat::RGBA16F, TextureFormat::DEPTH };
			spec.Attachments = attachments;

			PbrIndex = false;

			for (int i = 0; i < 2; i++)
				PbrFramebuffers[i] = Framebuffer::Create(spec);
		}

		{
			FramebufferSpecification spec;
			spec.Width = windowWidth * 0.5f;;
			spec.Height = windowHeight * 0.5f;

			FramebufferAttachments attachments = { TextureFormat::GRAYSCALE };
			spec.Attachments = attachments;

			OcclusionMapFramebuffer = Framebuffer::Create(spec);
		}
		
		{
			FramebufferSpecification spec;
			spec.Width = windowWidth;
			spec.Height = windowHeight;

			FramebufferAttachments attachments = { TextureFormat::RGBA8 };
			spec.Attachments = attachments;

			GodRaysFramebuffer = Framebuffer::Create(spec);
		}
		
		{
			FramebufferSpecification spec;
			spec.Width = windowWidth;
			spec.Height = windowHeight;

			FramebufferAttachments attachments = { TextureFormat::RGBA8 };
			spec.Attachments = attachments;

			OutputFramebuffer = Framebuffer::Create(spec);
		}
	}

	void FramebufferData::Resize(int32_t windowWidth, int32_t windowHeight)
	{
		FUNCTION_TIMER;

		GBufferFramebuffer->Resize(windowWidth, windowHeight);
		SsaoFramebuffer->Resize(windowWidth, windowHeight);

		for (int i = 0; i < 2; i++)
			PbrFramebuffers[i]->Resize(windowWidth, windowHeight);

		OcclusionMapFramebuffer->Resize(0.5f * windowWidth, 0.5f * windowHeight);
		GodRaysFramebuffer->Resize(windowWidth, windowHeight);
		OutputFramebuffer->Resize(windowWidth, windowHeight);
	}

	void TextureData::Intialize(int32_t windowWidth, int32_t windowHeight)
	{
		FUNCTION_TIMER;

		float mipWidth = windowWidth;
		float mipHeight = windowHeight;

		for (int i = 0; i < BloomMips.size(); i++)
		{
			mipWidth *= 0.5f;
			mipHeight *= 0.5f;

			BloomMips[i] = Texture2D::Create(mipWidth, mipHeight, TextureFormat::R11F_G11F_B10F);
		}
	}

	void TextureData::Resize(int32_t windowWidth, int32_t windowHeight)
	{
		FUNCTION_TIMER;

		float mipWidth = windowWidth;
		float mipHeight = windowHeight;

		for (int i = 0; i < BloomMips.size(); i++)
		{
			mipWidth *= 0.5f;
			mipHeight *= 0.5f;

			BloomMips[i] = Texture2D::Create(mipWidth, mipHeight, TextureFormat::R11F_G11F_B10F);
		}
	}
}
#pragma once

#include "Core.h"
#include "Dependencies.h"

#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"

namespace Nero
{
	class Shader
	{
	private:
		struct ShaderSource
		{
			bool HasGeometryShader = false;
			std::string VertexShaderSource, GeometryShaderSource, FragmentShaderSource;
		};

		uint32_t m_RendererId;
		std::unordered_map<std::string, int32_t> m_UniformCache;

	public:
		static Ref<Shader> Create(const fs::path& filePath);

		Shader(const fs::path& filePath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetBool(const std::string& uniformName, bool value);

		void SetInt(const std::string& uniformName, int value);
		void SetIntArray(const std::string& uniformName, int* values, uint32_t counter);

		void SetFloat(const std::string& uniformName, float value);
		void SetFloat2(const std::string& uniformName, glm::vec2 vector);
		void SetFloat3(const std::string& uniformName, glm::vec3 vector);
		void SetFloat4(const std::string& uniformName, glm::vec4 vector);

		void SetMat3(const std::string& uniformName, glm::mat3 matrix);
		void SetMat4(const std::string& uniformName, glm::mat4 matrix);

		void SetDirectionalLight(const std::string& uniformName, const Ref<DirectionalLight> light, bool isShadowCaster = false);
		void SetPointLight(const std::string& uniformName, const Ref<PointLight> light, bool isShadowCaster = false);

	private:
		int32_t GetUniformLocation(const std::string& uniformName);

		static ShaderSource GetShaderSourceFromFile(const fs::path& shaderPath);
		void CompileAndLinkShader(ShaderSource shaderSource);
	};

}
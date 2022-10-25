#include "Shader.h"

namespace Nero
{
	Ref<Shader> Shader::Create(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		return CreateRef<Shader>(filePath);
	}

	Shader::Shader(const fs::path& filePath)
	{
		FUNCTION_TIMER;

		fs::path fullPath = Util::GetAssetDirectory() / filePath;
		ShaderSource source = GetShaderSourceFromFile(fullPath);
		CompileAndLinkShader(source);
	}

	void Shader::CompileAndLinkShader(ShaderSource shaderSource)
	{
		FUNCTION_TIMER;

		bool hasGeometryShader = shaderSource.HasGeometryShader;

		std::string vertexSource = shaderSource.VertexShaderSource,
			geometrySource = shaderSource.GeometryShaderSource,
			fragmentSource = shaderSource.FragmentShaderSource;

		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const char* source = vertexSource.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		glCompileShader(vertexShader);

		int32_t compilationStatus = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compilationStatus);

		if (compilationStatus == 0)
		{
			int32_t length = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);

			char* infoLog = new char[length];
			glGetShaderInfoLog(vertexShader, length, &length, &infoLog[0]);

			glDeleteShader(vertexShader);

			Assert::Unreachable("Vertex Shader failed to compile!\nInfolog: " + std::string(infoLog));
		}

		uint32_t geometryShader;
		if (hasGeometryShader)
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

			source = geometrySource.c_str();
			glShaderSource(geometryShader, 1, &source, 0);

			glCompileShader(geometryShader);

			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &compilationStatus);

			if (compilationStatus == 0)
			{
				int32_t length = 0;
				glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &length);

				char* infoLog = (char*)alloca(length);
				glGetShaderInfoLog(geometryShader, length, &length, &infoLog[0]);

				glDeleteShader(geometryShader);

				Assert::Unreachable("Geometry Shader failed to compile!\nInfolog: " + std::string(infoLog));
			}
		}

		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = fragmentSource.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compilationStatus);

		if (compilationStatus == 0)
		{
			int32_t length = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);

			char* infoLog = (char*)alloca(length);
			glGetShaderInfoLog(fragmentShader, length, &length, &infoLog[0]);

			glDeleteShader(fragmentShader);

			Assert::Unreachable("Fragment Shader failed to compile!\nInfolog: " + std::string(infoLog));
		}

		m_RendererId = glCreateProgram();

		glAttachShader(m_RendererId, vertexShader);
		if (hasGeometryShader)
			glAttachShader(m_RendererId, geometryShader);
		glAttachShader(m_RendererId, fragmentShader);

		glLinkProgram(m_RendererId);

		int32_t linkingStatus = 0;
		glGetProgramiv(m_RendererId, GL_LINK_STATUS, (int32_t*)&linkingStatus);

		if (linkingStatus == 0)
		{
			int32_t length = 0;
			glGetProgramiv(m_RendererId, GL_INFO_LOG_LENGTH, &length);

			char* infoLog = (char*)alloca(length);
			glGetProgramInfoLog(m_RendererId, length, &length, &infoLog[0]);

			glDeleteProgram(m_RendererId);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			Assert::Unreachable("Failed to link shader program!\nInfolog: " + std::string(infoLog));
		}

		glDetachShader(m_RendererId, vertexShader);

		if (hasGeometryShader)
			glDetachShader(m_RendererId, geometryShader);
			
		glDetachShader(m_RendererId, fragmentShader);
	}

	Shader::~Shader()
	{
		FUNCTION_TIMER;

		glDeleteProgram(m_RendererId);
	}

	void Shader::Bind() const
	{
		FUNCTION_TIMER;

		glUseProgram(m_RendererId);
	}

	void Shader::Unbind() const
	{
		FUNCTION_TIMER;

		glUseProgram(0);
	}

	void Shader::SetBool(const std::string& uniformName, bool value)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform1i(location, value);
	}

	void Shader::SetInt(const std::string& uniformName, int value)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform1i(location, value);
	}

	void Shader::SetIntArray(const std::string& uniformName, int* values, uint32_t count)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform1iv(location, count, values);
	}

	void Shader::SetFloat(const std::string& uniformName, float value)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform1f(location, value);
	}

	void Shader::SetFloat2(const std::string& uniformName, glm::vec2 vector)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform2f(location, vector.x, vector.y);
	}

	void Shader::SetFloat3(const std::string& uniformName, glm::vec3 vector)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform3f(location, vector.x, vector.y, vector.z);
	}

	void Shader::SetFloat4(const std::string& uniformName, glm::vec4 vector)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
	}

	void Shader::SetMat3(const std::string& uniformName, glm::mat3 matrix)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetMat4(const std::string& uniformName, glm::mat4 matrix)
	{
		FUNCTION_TIMER;

		int32_t location = GetUniformLocation(uniformName);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetDirectionalLight(const std::string& uniformName, const Ref<DirectionalLight> light, bool isShadowCaster /* = false */)
	{
		SetFloat3(uniformName + ".Direction", light->GetDirection());
		SetFloat3(uniformName + ".Diffuse", light->GetDiffuse());
		SetBool(uniformName + ".ShadowCaster", isShadowCaster);

		if (isShadowCaster)
		{
			SetMat4(uniformName + ".LightSpaceTransform", light->GetLightSpaceTransform());
		}
	}

	void Shader::SetPointLight(const std::string& uniformName, const Ref<PointLight> light, bool isShadowCaster /* = false */)
	{
		SetFloat3(uniformName + ".Position", light->GetPosition());
		SetFloat3(uniformName + ".Diffuse", light->GetDiffuse());
		SetBool(uniformName + ".ShadowCaster", isShadowCaster);
	}

	int32_t Shader::GetUniformLocation(const std::string& uniformName)
	{
		FUNCTION_TIMER;

		auto loc = m_UniformCache.find(uniformName);
		if (loc != m_UniformCache.end())
			return loc->second;

		int32_t location = glGetUniformLocation(m_RendererId, uniformName.c_str());

		if (location == -1)
			Log::Error("Failed to find uniform " + uniformName + ". It might have been optimized away.");

		m_UniformCache[uniformName] = location;
		return location;
	}

	Shader::ShaderSource Shader::GetShaderSourceFromFile(const fs::path& shaderPath)
	{
		FUNCTION_TIMER;

		std::ifstream inputFile(shaderPath);

		ShaderSource source;
		std::string* currentSource = &source.VertexShaderSource;

		if (!inputFile.is_open())
			Assert::Unreachable("Could not open file " + shaderPath.string());

		std::string line;

		while (std::getline(inputFile, line))
		{
			if (line == "#type vertex")
			{
				currentSource = &source.VertexShaderSource; continue;
			}
			else if (line == "#type fragment")
			{
				currentSource = &source.FragmentShaderSource; continue;
			}
			else if (line == "#type geometry")
			{
				source.HasGeometryShader = true;
				currentSource = &source.GeometryShaderSource; continue;
			}
				
			*currentSource += line + '\n';
		}

		inputFile.close();
		return source;
	}
}

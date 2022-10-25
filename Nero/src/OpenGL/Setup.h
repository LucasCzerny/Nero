#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero { namespace Util
{
    static bool showOpenGlOutput = false;

    void GLAPIENTRY OpenGlErrorCallback(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar* message,
            const void* userParam)
    {
        if (showOpenGlOutput)
            Log::Warning("OpenGL Output: " + std::string(message));
    }

	void OpenGlSettings()
	{
        FUNCTION_TIMER;

        glEnable(GL_DEPTH_TEST);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        //glFrontFace(GL_CCW);

        glfwSwapInterval(0);

        glfwWindowHint(GLFW_SAMPLES, 4);

        glEnable(GL_MULTISAMPLE);

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGlErrorCallback, 0);
	}

    void ShowOpenGlOutput()
    {
        showOpenGlOutput = true;
    }
    
    void HideOpenGlOutput()
    {
        showOpenGlOutput = false;
    }
} }

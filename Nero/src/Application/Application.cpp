#include "Application.h"

#include "Renderer/Renderer.h"
#include "Time/Time.h"

namespace Nero
{
    GLFWwindow* Application::m_GlfwWindow;
    std::function<void(int, int)> Application::m_SizeCallback;
    int32_t Application::m_Width;
    int32_t Application::m_Height;

    void Application::Create(const char* name /* = "Nero Application" */, uint32_t width /* = 1280 */, uint32_t height /* = 720 */, bool vsync /* = true */)
    {
        FUNCTION_TIMER;

        START_INSTRUMENTATION("Main Profile");

        m_Width = width, m_Height = height;

        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_GlfwWindow = glfwCreateWindow(m_Width, m_Height, name, NULL, NULL);
        if (m_GlfwWindow == NULL)
        {
            glfwTerminate();
            Assert::Unreachable("Failed to create GLFW Application");
        }

        glfwMakeContextCurrent(m_GlfwWindow);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Assert::Unreachable("Failed to initialize GLAD");
        }

        Renderer::Init(width, height);
    }

    void Application::Destroy()
    {
        FUNCTION_TIMER;

        END_INSTRUMENTATION;
    }

    void Application::OnUpdate()
    {
        FUNCTION_TIMER;

        glfwPollEvents();

        Time::OnUpdate();

        {
            SCOPE_TIMER("glfwSwapBuffer Scope");
            glfwSwapBuffers(GetGlfwWindow());
        }
    }

    void Application::OnResize(std::function<void(int, int)> function)
    {
        FUNCTION_TIMER;

        m_SizeCallback = function;

        glfwSetWindowSizeCallback(m_GlfwWindow, [](GLFWwindow*, int width, int height)
        {
            m_Width = width;
            m_Height = height;

            m_SizeCallback(m_Width, m_Height);
        });
    }

    void Application::ExternalViewportResize(int32_t width, int32_t height)
    {
        m_SizeCallback(m_Width, m_Height);
    }

    glm::vec2 Application::GetWindowSize()
    {
        FUNCTION_TIMER;

        int32_t width, height;
        glfwGetFramebufferSize(m_GlfwWindow, &width, &height);
        return glm::vec2(width, height);
    }

    bool Application::IsOpen()
    {
        FUNCTION_TIMER;

        return !glfwWindowShouldClose(m_GlfwWindow);
    }
}

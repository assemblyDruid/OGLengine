// clang-format off
#include "GL/glew.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "windowing.h"

#include "logging.h"

// Forward Declaration
void
OnWindowResize(GLFWwindow* glfw_window, int new_width, int new_height);

Window::Window(const int&          _gl_major_version,
               const int&          _gl_minor_version,
               const unsigned int& _window_width,
               const unsigned int& _window_height,
               const char*         _window_title,
               bool&               _success)
{
    _success = true;
    if (!glfwInit())
    {
        Log_E("Unable to initialize GLFW.");
        _success = false;
        return;
    }

    // [ cfarvin::TODO ] Store these and other settings in a configuration file that is parsed on startup?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _gl_minor_version);

    // Note: nullptr parameters allow for fullscreen mode and resource sharing.
    glfw_window = glfwCreateWindow(_window_width, _window_height, _window_title, nullptr, nullptr);
    if (nullptr == glfw_window)
    {
        Log_E("Glfw failed to create a valid window.");
        _success = false;
        return;
    }

    glfwMakeContextCurrent(glfw_window);
    glfwSetWindowSizeCallback(glfw_window, OnWindowResize);

    if (GLEW_OK != glewInit())
    {
        Log_E("Unable to initialize GLEW.");
        _success = false;
        return;
    }

    // Note: Enables V-Sync
    // Note: Glfw windows are double-buffered by default.
    glfwSwapInterval(1);
}

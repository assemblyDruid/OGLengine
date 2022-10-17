// clang-format off
#include "GL/glew.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "windowing.h"

#include "logging.h"

Window::Window(const int&&          _gl_major_version,
               const int&&          _gl_minor_version,
               const unsigned int&& _window_width,
               const unsigned int&& _window_height,
               const char* const&&  _window_title,
               bool&                _success)
{
    _success = true;
    if (!glfwInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLFW.");
        _success = false;
    }

    // [ cfarvin::TODO ] Store these and other settings in a configuration file that is parsed on startup?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _gl_minor_version);

    // Note: nullptr parameters allow for fullscreen mode and resource sharing.
    glfw_window = glfwCreateWindow(_window_width, _window_height, _window_title, nullptr, nullptr);
    if (nullptr == glfw_window)
    {
        Log(LogType::ERROR, "Glfw failed to create a valid window.");
        _success = false;
    }

    glfwMakeContextCurrent(glfw_window);

    if (GLEW_OK != glewInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLEW.");
        _success = false;
    }

    // Note: Enables V-Sync
    // Note: Glfw windows are double-buffered by default.
    glfwSwapInterval(1);

    aspect_ratio = (float)_window_width / (float)_window_height;
}

void
Window::UpdateAspectRatio()
{
    int width  = 0;
    int height = 0;
    glfwGetFramebufferSize(glfw_window, &width, &height);
    aspect_ratio = (float)width / (float)height;
}

float
Window::GetAspectRatio()
{
    return aspect_ratio;
}

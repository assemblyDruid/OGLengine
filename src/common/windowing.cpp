#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "logging.h"

const bool
SetupWindow(GLFWwindow**         _window,
            const int&&          _gl_major_version,
            const int&&          _gl_minor_version,
            const unsigned int&& _window_width,
            const unsigned int&& _window_height,
            const char* const&&  _window_title)
{
    if (nullptr == _window)
    {
        Log(LogType::ERROR, "Invalid window pointer provided.");
        return false;
    }

    if (!glfwInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLFW.");
        return false;
    }

    // [ cfarvin::TODO ] Store these and other settings in a configuration file that is parsed on startup?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _gl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _gl_minor_version);

    // Note: nullptr parameters allow for fullscreen mode and resource sharing.
    *_window = glfwCreateWindow(_window_width, _window_height, _window_title, nullptr, nullptr);
    if (nullptr == *_window)
    {
        Log(LogType::ERROR, "Glfw failed to create a valid window.");
        return false;
    }

    glfwMakeContextCurrent(*_window);

    if (GLEW_OK != glewInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLEW.");
        return false;
    }

    // Note: Enables V-Sync
    // Note: Glfw windows are double-buffered by default.
    glfwSwapInterval(1);

    return true;
}

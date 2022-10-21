#ifndef windowing_h
#define windowing_h

#include "GLFW/glfw3.h"

class Window
{
  public:
    Window::Window(const int&&          _gl_major_version,
                   const int&&          _gl_minor_version,
                   const unsigned int&& _window_width,
                   const unsigned int&& _window_height,
                   const char* const&&  _window_title,
                   bool&                success);

    GLFWwindow* glfw_window;
};

#endif // windowing_h

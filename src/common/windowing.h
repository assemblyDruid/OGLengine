#ifndef windowing_h
#define windowing_h

#include "GLFW/glfw3.h"

struct Window
{
    int         width;
    int         height;
    float       aspect_ratio;
    GLFWwindow* window;
};

const bool
SetupWindow(GLFWwindow**         _window,
            const int&&          _gl_major_version,
            const int&&          _gl_minor_version,
            const unsigned int&& _window_width,
            const unsigned int&& _window_height,
            const char* const&&  _window_title);

#endif // windowing_h

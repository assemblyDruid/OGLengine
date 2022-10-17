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

    float
    GetAspectRatio();

    GLFWwindow* glfw_window;

  private:
    // These are already stored by GLFW, access them there.
    /* int width; */
    /* int height; */

    // [ cfarvin::NOTE ] Need up update aspect ratio on window resize.
    float aspect_ratio;

    void
    UpdateAspectRatio();
};

#endif // windowing_h

#ifndef app_window_h
#define app_window_h

// clang-format off
#include "pch.h"
#include "platform/windows_platform.h"
// clang-format on

namespace app_window
{
    void
    Initialize(bool& _success_out);

    void
    UpdateAspectRatio(const float& width, const float& height);

    void
    UpdateProjectionMatrix();

    void
    OnWindowResize(bool& _success_out, int _new_width_in, int _new_height_in);

    void
    SwapBuffers();

    void
    ProcessPlatformEvents();

} // namespace app_window

#endif // windowing_h

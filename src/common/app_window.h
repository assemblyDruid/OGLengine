#ifndef app_window_h
#define app_window_h

// clang-format off
#include "pch.h"
// clang-format on

namespace app_window
{
    void
    Initialize(bool& _success_out);

    // [ cfarvin::DANGER ] I believe that we will have to do this when we bind programs,
    //                     as different rendering programs / shader combinations will
    //                     each have their own copy of a reference to the projection
    //                     matrix. In this case, only the rendering program bound
    //                     during the window resize will get updated, and the others
    //                     will be the dirty.
    void
    UpdateRenderingProgramProjectionMatrix(bool& _success_out);

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

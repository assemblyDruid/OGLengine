// clang-format off
#include "pch.h"
// clang-format on

#include "app_window.h"

#include "gl_function_wrappers.h"
#include "logging.h"
#include "state_tools.h"
#include "windows_platform.h"

void
app_window::Initialize(bool& _success_out)
{
    state::StateCache* state_cache = state::StateCache::GetInstance();
    platform::win32::Initialize(_success_out);
    if (false == _success_out)
    {
        Log_e("Unable to initialize Win32 platform layer.");
        return;
    }

    glfn::Viewport(0,
                   0,
                   state_cache->window_state.window_width,
                   state_cache->window_state.window_height);

    UpdateAspectRatio(static_cast<float>(state_cache->window_state.window_width),
                      static_cast<float>(state_cache->window_state.window_height));

    UpdateProjectionMatrix();
    _success_out = true;
}

void
app_window::UpdateAspectRatio(const float& width, const float& height)
{
    state::StateCache* state_cache         = state::StateCache::GetInstance();
    state_cache->window_state.aspect_ratio = width / height;
}

void
app_window::UpdateProjectionMatrix()
{
    state::StateCache* state_cache  = state::StateCache::GetInstance();
    state_cache->window_state.p_mat = glm::perspective(
      state_cache->graphics_state.field_of_view_radians,
      state_cache->window_state.aspect_ratio,
      state_cache->graphics_state.near_clipping_plane_distance,
      state_cache->graphics_state.far_clipping_plane_distance);

    return;
}

void
app_window::OnWindowResize(bool& _success_out, int _new_width_in, int _new_height_in)
{
    _success_out = true;

    // Note: This check is for cases in which window initialization in the OS triggers
    //       paths that lead to OnWindowResize conditions before the graphics API is
    //       initialized as well. As a result, we do not set the success condition to false,
    //       but exit before any graphics API functions are called.
    state::StateCache* state_cache = state::StateCache::GetInstance();
    if ((0 == state_cache->window_state.aspect_ratio) ||
        (0 == state_cache->window_state.window_width) ||
        (0 == state_cache->window_state.window_width))
    {
        return;
    }

    if ((0 == _new_width_in) || (0 == _new_height_in))
    {
        Log_e("Invalid window or widnow dimensions.");
        _success_out = false;
        return;
    }

    glfn::Viewport(0, 0, _new_width_in, _new_height_in);
    UpdateAspectRatio(static_cast<float>(_new_width_in), static_cast<float>(_new_height_in));
    UpdateProjectionMatrix();

    // [ cfarvin::TODO ] There is some confusion on the best way to make sure that when the
    //                   window is resized, that all of the rendering programs that have a
    //                   potential to run after the change will get an updated value of the
    //                   projection matrix.
    //
    //                   Right now, the projection matrix information is being sent to the
    //                   rendering program on every draw, ensuring that this matrix stays
    //                   up to date.
    //
    //                   In the future, it would be ideal to find a way to store the
    //                   projection and view matrices somewhere on the GPU, and point the
    //                   layout indices within each shader to those values. There's currently
    //                   some confusion if Uniform Buffer Objects or another GLSL feature will
    //                   offer this kind of solution, if another solution is better suited, or
    //                   if sending the projection matrix information on every draw call (when
    //                   the rendering program is bound) is simply the best way to proceed.
    //
    //
    // IN THE MEANTIME - Simply to preseve the resizing functionality while we have a single
    //                   rendering program, we will update the current/active rendering
    //                   program's projection matrix in the scoped resolution operators below.
    {
        const GLuint&  rendering_program = state_cache->opengl_state->program_id;
        const GLfloat* p_mat             = glm::value_ptr(state_cache->window_state.p_mat);

        glfn::UseProgram(rendering_program);
        GLint p_location = glfn::GetUniformLocation(rendering_program, "proj_matrix");
        if (-1 == p_location)
        {
            Log_e("Unable to query location of projection matrix in rendering program.");
            _success_out = false;
            return;
        }

        glfn::UniformMatrix4fv(p_location, 1, false, p_mat);
        if (false == _success_out)
        {
            Log_e("Unable to update rendering program projection matrix.");
            return;
        }
    }

    _success_out = true;
}

void
app_window::SwapBuffers()
{
#ifdef _ENGINE_PLATFORM_WINDOWS_
    platform::win32::SwapBuffers();
#endif
}

void
app_window::ProcessPlatformEvents()
{
#ifdef _ENGINE_PLATFORM_WINDOWS_
    platform::win32::ProcessPlatformEvents();
#endif
}
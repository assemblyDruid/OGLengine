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

// [ cfarvin::DANGER ] I believe that we will have to do this when we bind programs,
//                     as different rendering programs / shader combinations will
//                     each have their own copy of a reference to the projection
//                     matrix. In this case, only the rendering program bound
//                     during the window resize will get updated, and the others
//                     will be the dirty.
void
app_window::UpdateRenderingProgramProjectionMatrix(bool& _success_out)
{
    UpdateProjectionMatrix();
    state::StateCache* state_cache       = state::StateCache::GetInstance();
    const GLuint&      rendering_program = state_cache->opengl_state->program_id;
    const GLfloat*     p_mat             = glm::value_ptr(state_cache->window_state.p_mat);

    glfn::UseProgram(rendering_program);
    GLint p_location = glfn::GetUniformLocation(rendering_program, "proj_matrix");
    if (-1 == p_location)
    {
        Log_e("Unable to query location of projection matrix in rendering program.");
        _success_out = false;
        return;
    }

    glfn::UniformMatrix4fv(p_location, 1, false, p_mat);

    _success_out = true;
}

void
app_window::OnWindowResize(bool& _success_out, int _new_width_in, int _new_height_in)
{
    if ((0 == _new_width_in) || (0 == _new_height_in))
    {
        Log_e("Invalid window or widnow dimensions.");
        return;
    }

    glfn::Viewport(0, 0, _new_width_in, _new_height_in);
    UpdateAspectRatio(static_cast<float>(_new_width_in), static_cast<float>(_new_height_in));
    UpdateProjectionMatrix();

    // [ cfarvin::DANGER ] See note on UpdateRenderingProgramProjectionMatrix()
    UpdateRenderingProgramProjectionMatrix(_success_out);
    if (false == _success_out)
    {
        Log_e("Unable to update rendering program projection matrix.");
        return;
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
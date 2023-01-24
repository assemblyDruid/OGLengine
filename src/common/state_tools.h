#ifndef state_tools_h
#define state_tools_h

// clang-format off
#include "pch.h"
// clang-format on

#include "app_window.h"
#include "gl_enum_tools.h"
#include "gl_tools.h"

// Forward declare for STATE_CACHE_MANAGED macro:
namespace state
{
    struct StateCache;
}

// -------------------------------------------------------------------------
//
//                           Design Philosophy
//
// -------------------------------------------------------------------------
// Note: Declare at the end of state structure definitions to ensure that
//       classes/structures do not attempt to create their own local copies.
//       All changes to to engine state must go through the StateCache or
//       through overloaded graphics api calls which in turn go through the
//       StateCache.
#define STATE_CACHE_MANAGED(_obj_name_in)                 \
  private:                                                \
    friend struct state::StateCache;                      \
    _obj_name_in(const _obj_name_in&)           = delete; \
    _obj_name_in operator=(const _obj_name_in&) = delete; \
    _obj_name_in() {};

namespace graphics
{
    enum struct API
    {
        OPENGL = 0,
        COUNT
    };

    struct State
    {
        API api;

        // Perspective Matrix
        float field_of_view_radians        = 0;
        float near_clipping_plane_distance = 0;
        float far_clipping_plane_distance  = 0;

        STATE_CACHE_MANAGED(State);
    };
} // namespace graphics

namespace app_window
{
    struct State
    {
        unsigned int window_height = 0;
        unsigned int window_width  = 0;
        bool         is_minimized  = 0;

        // Note: The following are not defined in SUS files.
        // [ cfarvin::TODO ] Define in DumpState().
        float     aspect_ratio = 0;
        glm::mat4 v_mat        = {};
        glm::mat4 p_mat        = {};

        STATE_CACHE_MANAGED(State);
    };
} // namespace app_window

namespace platform
{
    namespace win32
    {
        struct State
        {
            // [ cfarvin::TODO ] Not in dumpstate yet.
            HINSTANCE instance             = nullptr;
            HWND      window               = nullptr;
            HDC       device_context       = nullptr;
            HGLRC     gl_rendering_context = nullptr;
            LPCSTR    class_name           = nullptr;

            STATE_CACHE_MANAGED(State);
        };
    } // namespace win32
} // namespace platform

namespace state
{
    // Note: Additions and alterations to this structure need to be mirrored in the DumpState() method.
    struct StateCache
    {
        void
        DumpState();

        static StateCache*
        GetInstance() noexcept;

        void
        Initialize(bool& _success_out, const std::string& _start_up_state_file_path_in) noexcept;

        // Environment independent members.
        // --------------------------------
        graphics::State   graphics_state;
        app_window::State window_state;
        bool              is_paused  = false;
        bool              is_running = true;
        bool              success    = true; // [ cfarvin::TODO ]

        // Environment dependent members (pointers).
        // -----------------------------------------
        glt::GLState*           opengl_state = nullptr; // Note: Defined in gl_tools.h.
        platform::win32::State* win32_state  = nullptr;

      private:
        StateCache() {};
        ~StateCache();

        StateCache
        operator=(const StateCache&) = delete;
    };

} // namespace state

#endif // state_tools_h
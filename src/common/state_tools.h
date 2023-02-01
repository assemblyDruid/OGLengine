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

//
// OpenGL Function Wrapper Macros
//
// Note: These marco definitions were originally in the header which defines all wrapper
//       functions for OpenGL. To better enforce the philosophy that only the StateCache
//       and overriden API functions can change state, a state::StateCache::GetMutableState
//       function was introduce in the private scope of the StateCache object, and each
//       overriden API function with powers to modify the cache are explicitly listed as
//       friends to the StateCache object. Since those function definitions require these
//       macros, and since the header which defines the overriden API functions already
//       includes the StateCache headers, these macros needed to be moved here to avoid
//       a circular header dependency.
#ifdef _ENGINE_DEBUG_
#define _DEBUG_FILE_AND_LINE_PARAMS_ , const char *const &&_file_in, const unsigned int &&_line_in
#define _DEBUG_FILE_AND_LINE_ARGS_   , std::move(__FILE__), std::move(__LINE__)
#define _DEBUG_FILE_AND_LINE_PARAMS_NO_COMMA_ \
    const char *const &&_file_in, const unsigned int &&_line_in
#define _DEBUG_FILE_AND_LINE_ARGS_NO_COMMA_ std::move(__FILE__), std::move(__LINE__)
#define _DEBUG_QUERY_ERRORS_ARGS            std::move(_file_in), std::move(_line_in)
#else
#define _DEBUG_FILE_AND_LINE_PARAMS_          /* NO OP */
#define _DEBUG_FILE_AND_LINE_ARGS_            /* NO OP */
#define _DEBUG_FILE_AND_LINE_PARAMS_NO_COMMA_ /* NO OP */
#define _DEBUG_FILE_AND_LINE_ARGS_NO_COMMA_   /* NO OP */
#define _DEBUG_QUERY_ERRORS_ARGS              /* NO OP */
#endif

//
// Forward declarations for friends of state::StateCache::GetMutableInstance()
//
// Note: See the notes in the private scope of state::StateCache and the notes
//       titled "OpenGL Function Wrapper Notes".
namespace glfn
{
    void
    Impl_ActiveTexture(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_BindBuffer(const GLenum&&, const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_BindTexture(const GLenum&&, const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_BindVertexArray(const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_ClearColor(const float&&,
                    const float&&,
                    const float&&,
                    const float&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_CullFace(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_DepthFunc(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_Enable(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_EnableVertexAttribArray(const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_FrontFace(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_UseProgram(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

    void
    Impl_Viewport(const GLint&&,
                  const GLint&&,
                  const GLsizei&&,
                  const GLsizei&& _DEBUG_FILE_AND_LINE_PARAMS_);
} // namespace glfn

// Forward declarations for access to mutable state cache.
struct Renderer;
struct Engine;

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

        static const StateCache* const
        GetInstance() noexcept;

        static void
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

        //
        // Friend Objects & Functions for state::StateCache::GetMutableInstance();
        //
        // Note: Only the following classes and functions have the ability to
        //       modify the state cache. While potentially exhausting to list
        //       them explicitly, it enforces the philosophy of the StateCache.
        friend void
        glfn::Impl_ActiveTexture(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_BindBuffer(const GLenum&&, const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_BindTexture(const GLenum&&, const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_BindVertexArray(const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_CullFace(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_ClearColor(const float&&,
                              const float&&,
                              const float&&,
                              const float&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_DepthFunc(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_Enable(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_EnableVertexAttribArray(const GLuint&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_FrontFace(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_UseProgram(const GLenum&& _DEBUG_FILE_AND_LINE_PARAMS_);

        friend void
        glfn::Impl_Viewport(const GLint&&,
                            const GLint&&,
                            const GLsizei&&,
                            const GLsizei&& _DEBUG_FILE_AND_LINE_PARAMS_);

        // [ cfarvin::TESTING ]
        friend void
        Renderer::Display(bool& _success_out);

        // [ cfarvin::TESTING ]
        friend void
        Engine::Initialize(bool& _success_out, const std::string& _start_up_state_file_in);

        static state::StateCache* const
        GetMutableInstance() noexcept;
    };

} // namespace state

#endif // state_tools_h
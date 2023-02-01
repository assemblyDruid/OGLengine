#include "engine.h"

#include "gl_function_wrappers.h"
#include "pch.h"
#include "state_tools.h"

void
Engine::Initialize(bool& _success_out, const std::string& _start_up_state_file_in)
{
    _success_out            = false;
    static bool initialized = false;
    if (true == initialized)
    {
        Log_e(
          "The engine instance has already been initialized; cannot initialize more than once.");
        return;
    }

    // Initialize the state cache.
    {
        state_cache = state::StateCache::GetMutableInstance();
        state_cache->Initialize(_success_out, _start_up_state_file_in);
        if (false == _success_out)
        {
            Log_e("Unable to initialize state cache.");
            return;
        }
    }

    // Initialize the Graphics API and the Window.
    {
        app_window::Initialize(_success_out);
        if (false == _success_out)
        {
            Log_e("Unable to initialize the application window.");
            return;
        }

#ifdef _ENGINE_DEBUG_
        // [ cfarvin::TODO ] These will have to be moved to an OGL specific
        //                   initialization section when other graphics APIs
        //                   become supported.
        glfn::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glfn::DebugMessageCallback((glt::GLDebugCallback), (void*)0);
#endif
    }

    _success_out = true;
    initialized  = true;
}

void
Engine::AddRenderer(bool& _success_out, std::function<void(bool&)> _render_init_fn_in)
{
}

Engine&
Engine::GetInstance() noexcept
{
    static Engine engine;
    return engine;
}
#ifndef engine_h
#define engine_h

#include "logging.h"
#include "pch.h"
#include "renderer.h"
#include "state_tools.h"

struct Engine
{
    static Engine&
    GetInstance() noexcept;

    void
    Initialize(bool& _success_out, const std::string& _start_up_state_file_in);

    void
    AddRenderer(bool& _success_out, std::function<void(bool&)> _render_init_fn_in);

  private: /* Functions */
    Engine();
    Engine(const Engine&) = delete;

    Engine
    operator=(const Engine&) = delete;

  private: /* Members */
    state::StateCache* state_cache;
    Renderer renderer;
};

#endif // engine_he
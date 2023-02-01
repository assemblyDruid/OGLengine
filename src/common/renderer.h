#ifndef renderer_h
#define renderer_h

struct Renderer
{
    Renderer();

    static void
    Display(bool& _success_out)
    {
        static state::StateCache* const state_cache = state::StateCache::GetMutableInstance();

        // Note: Timer is started/stopped at the scope of this loop, while the
        //       ElapsedMs() function returns a global elapsed time since the
        //       timer object was initially instantiated.
        // timer.StartTimer();
        const float scaled_time = static_cast<float>(timer.ElapsedMs()) / 225.0f;

        //
        // Model Independent
        //
        {
            glfn::ClearColor(0.105f, cos(scaled_time), sin(scaled_time), 1.0f);

            // [ cfarvin::TODO ] These can go into state_cache, be read from SUS files,
            //                   and be changed only when necessary out of the draw loop.
            glfn::Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glfn::Enable(GL_CULL_FACE);
            glfn::FrontFace(GL_CCW);
            glfn::CullFace(GL_BACK);
            glfn::Enable(GL_DEPTH_TEST);
            glfn::DepthFunc(GL_LEQUAL);
        }

        //
        // Camera -- LookAt
        //
        {
            test_model.GetPosition(local_v3.x, local_v3.y, local_v3.z);
            camera.LookAt(local_v3, state_cache->window_state.v_mat);
            matrix_stack.push(state_cache->window_state.v_mat);
        }

        //
        // Test Model
        //
        {
            // Matrix Stacks
            test_model.GetPosition(local_v3.x, local_v3.y, local_v3.z);
            matrix_stack.push(matrix_stack.top()); //Translation slot
            matrix_stack.top() *= glm::translate(glm::mat4(1.0f),
                                                 local_v3); // Translation matrix

            matrix_stack.push(matrix_stack.top()); // Rotation slot
            matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                              scaled_time,
                                              glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation matrix

            matrix_stack.push(matrix_stack.top()); // Scale slot
            matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                             glm::vec3(4.25f, 4.25f, 4.25f)); // Scale matrix

            // Copy matrices to corresponding uniform values (no view yet).
            test_model.ModifyUniformMatrix(std::move(state_cache->opengl_state->program_id),
                                           MatrixType::mtMODEL_VIEW_MATRIX,
                                           matrix_stack.top());

            // Draw
            test_model.Draw();

            matrix_stack.pop(); // Scale matrix removed, rotation matrix on top.
            matrix_stack.pop(); // Rotation matrix removed, translation matrix on top.
        }

        // Clear matrix stack.
        for (auto i = 0; i < matrix_stack.size(); i++)
        {
            matrix_stack.pop();
        }

        _success_out = true;

        // timer.StopTimer();
        // timer.TimerElapsedMs(display_loop_miliseconds);
        // static unsigned int report_fps = 0;
        // if (5000 == report_fps)
        // {
        //     const float FPS = (1.0f / display_loop_miliseconds) * 1000.0f;
        //     Log_i("[ fps ] " + std::to_string(FPS));
        //     report_fps = 0;
        // }
        // report_fps++;

        return;
    }

  private:
    // [ cfarvin::TODO ] Should these be moved to object classes?
    //                   Possibly consumed by model importer along with which
    //                   shaders to use?
    static GLuint projection_matrix_uniform_location;

    static std::stack<glm::mat4> matrix_stack = {};
    static Camera                camera;
    static TexturedModel         test_model;
    static Timer                 timer;
    static float                 display_loop_miliseconds = 0.0f;

    static glm::vec3 local_v3;
};

#endif renderer_h
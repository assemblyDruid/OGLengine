// clang-format off
#include "pch.h"
// clang-format on

#include "app_window.h"
#include "camera.h"
#include "gl_function_wrappers.h"
#include "gl_tools.h"
#include "image_tools.h"
#include "logging.h"
#include "model.h"
#include "state_tools.h"
#include "timer.h"
#include "windows_platform.h"

#define PI 3.1415926535f

// [ cfarvin::TODO ] Should these be moved to object classes?
//                   Possibly consumed by model importer along with which
//                   shaders to use?
//GLuint rendering_program;
//GLuint mv_location;
GLuint p_location;

std::stack<glm::mat4> matrix_stack = {};
Camera                camera;
TexturedModel         test_model;

// [ cfarvin::TODO ] Should these be moved to shaders?

Timer timer;
float display_loop_miliseconds = 0.0f;

struct DisplayVars
{
    glm::vec3 v3;
    glm::mat4 m4;
};
DisplayVars dv;

void
DisplayLoop(bool& _success_out)
{
    static state::StateCache* const state_cache = state::StateCache::GetInstance();

    // [ cfarvin::REVISIT ]
    // timer.StartTimer();

    // [ cfarvin::REVISIT ]
    // const float current_time = timer.ElapsedMs();

    //
    // Model Independent
    //
    {
        // [ cfarvin::TODO ] These can go into state_cache, be read from SUS files,
        //                   and be changed only when necessary out of the draw loop.
        glfn::Clear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glfn::Enable(GL_CULL_FACE);
        glfn::FrontFace(GL_CCW);
        glfn::CullFace(GL_BACK);
        glfn::Enable(GL_DEPTH_TEST);
        glfn::DepthFunc(GL_LEQUAL);

        // [ cfarvin::TODO ] Uniform Buffer Object for projection
        // Projection Matrix
        p_location = glfn::GetUniformLocation(state_cache->opengl_state->program_id, "proj_matrix");
        glfn::UniformMatrix4fv(p_location,
                               1,
                               false,
                               glm::value_ptr(state_cache->window_state.p_mat));
    }

    //
    // Camera -- LookAt
    //
    {
        test_model.GetPosition(dv.v3.x, dv.v3.y, dv.v3.z);
        camera.LookAt(dv.v3, state_cache->window_state.v_mat);
        matrix_stack.push(state_cache->window_state.v_mat);
    }

    //
    // Test Model
    //
    {
        // Matrix Stacks
        test_model.GetPosition(dv.v3.x, dv.v3.y, dv.v3.z);
        matrix_stack.push(matrix_stack.top()); //Translation slot
        matrix_stack.top() *= glm::translate(glm::mat4(1.0f),
                                             dv.v3); // Translation matrix

        matrix_stack.push(matrix_stack.top()); // Rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          static_cast<float>(timer.ElapsedMs()) / 225.0f,
                                          glm::vec3(0.0f, 1.0f, 0.0f)); // Rotation matrix

        matrix_stack.push(matrix_stack.top()); // Scale slot
        matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                         glm::vec3(4.25f, 4.25f, 4.25f)); // Scale matrix

        // Copy matrices to corresponding uniform values (no view yet).
        test_model.
        glfn::UniformMatrix4fv(mv_location, 1, false, glm::value_ptr(matrix_stack.top()));

        // Draw
        test_model.Draw();

        matrix_stack.pop(); // Scale matrix removed, rotation matrix on top.
        matrix_stack.pop(); // Rotation matrix removed, translation matrix on top.
    }

    // Clear stack
    for (auto i = 0; i < matrix_stack.size(); i++)
    {
        matrix_stack.pop();
    }

    _success_out = true;

    timer.StopTimer();
    timer.TimerElapsedMs(display_loop_miliseconds);
    static unsigned int report_fps = 0;
    if (5000 == report_fps)
    {
        const float FPS = 1.0f / (display_loop_miliseconds / 1000.0f);
        Log_i("[ fps ] " + std::to_string(FPS));
        report_fps = 0;
    }
    report_fps++;
}

void
SetupModels(bool& _success_out)
{
    _success_out = false;

    camera.SetPosition(0.0f, 0.0f, 15.0f);
    test_model.SetPosition(0.0f, 0.0f, 0.0f);

    GLuint test_model_vao = 0;
    test_model.AddVertexArrayObject(_success_out, test_model_vao);
    if (false == _success_out)
    {
        Log_e("Unable to add a vertex array object.");
        return;
    }

    size_t                                      vertex_count;
    std::vector<float>                          buffer_data;
    std::vector<GLuint>                         buffer_elements_array;
    std::vector<glt::VertexDataType>            vertex_data_type_order;
    std::vector<glt::VertexAttributeDescriptor> vertex_attribute_pointers;

    // Attribute indices from shader layout declarations.
    constexpr GLuint position_attribute_index = 0;
    constexpr GLuint normal_attribute_index = 1; // Note: currently unused, but required for import.
    constexpr GLuint texture_coordinates_attribute_index = 2;

    LoadModelFromAsciiPlyFile(_success_out,
                              std::move(test_model_vao),
                              "./../src/common/test_assets/blender_monkey.ply",
                              std::move(position_attribute_index),
                              std::move(normal_attribute_index),
                              std::move(texture_coordinates_attribute_index),
                              vertex_count,
                              buffer_data,
                              buffer_elements_array,
                              vertex_data_type_order,
                              vertex_attribute_pointers);

    if (false == _success_out)
    {
        Log_e("Unable to load model");
        return;
    }

    // Test Model
    {
        const GLenum draw_type                     = GL_TRIANGLES;
        GLuint       test_model_pos_norm_tc_vbo_id = 0;

        //  Packed: positions, normals, texture coordinates.
        {
            test_model_pos_norm_tc_vbo_id = test_model.AddArrayBuffer(
              _success_out,
              buffer_data.data(),
              buffer_data.size() * sizeof(GL_FLOAT),
              static_cast<GLsizei>(vertex_count),
              GL_STATIC_DRAW,
              std::move(draw_type),
              vertex_attribute_pointers);

            if (false == _success_out)
            {
                Log_e("Unable to create position buffer.");
                return;
            }

            test_model.EnableVertexAttribute(_success_out, std::move(position_attribute_index));

            if (false == _success_out)
            {
                Log_e("Unable to enable position vertex attribute.");
                return;
            }
        }

        // ElementArray
        {
            test_model.AddElementBuffer(_success_out,
                                        buffer_elements_array.data(),
                                        buffer_elements_array.size() * sizeof(GL_UNSIGNED_INT),
                                        static_cast<GLsizei>(buffer_elements_array.size()),
                                        GL_STATIC_DRAW,
                                        std::move(draw_type));

            if (false == _success_out)
            {
                Log_e("Unable to add element buffer.");
                return;
            }
        }

        // Texture image.
        {
            GLuint texture_id;
            test_model.AddTexture(_success_out,
                                  "./../src/common/test_assets/brick.png",
                                  std::move(test_model_pos_norm_tc_vbo_id),
                                  std::move(texture_coordinates_attribute_index),
                                  GL_TEXTURE0,
                                  GL_TEXTURE_2D,
                                  texture_id);

            if (false == _success_out)
            {
                Log_e("Unable to add texture.");
                return;
            }

            test_model.EnableVertexAttribute(_success_out,
                                             std::move(texture_coordinates_attribute_index));

            if (false == _success_out)
            {
                Log_e("Unable to enable texture coordinate vertex attribute.");
                return;
            }

            test_model.EnableTexture(_success_out, std::move(texture_id));

            if (false == _success_out)
            {
                Log_e("Unable to enable texture.");
                return;
            }
        }
    }

    return;
}

void
Initialize(bool& _success_out)
{
    _success_out                   = false;
    state::StateCache* state_cache = state::StateCache::GetInstance();
    state_cache->Initialize(_success_out, "../src/chapter_6/chapter_6.sus");
    if (false == _success_out)
    {
        Log_e("Unable to initialize state cache.");
        return;
    }

    // Setup OpenGL and the Window.
    {
        app_window::Initialize(_success_out);

#ifdef _ENGINE_DEBUG_
        glfn::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glfn::DebugMessageCallback((glt::GLDebugCallback), (void*)0);
#endif

        glfn::ClearColor(0.105f, 0.125f, 0.1505f, 1.0f);
    }

    // Create the shader program.
    {
        // [ cfarvin::TODO ] Move these and other options into "state file", load on startup.
        glt::CreateShaderProgram(_success_out,
                                 "./../src/chapter_6/src/shaders/vertex_shader.glsl",
                                 "./../src/chapter_6/src/shaders/fragment_shader.glsl",
                                 state_cache->opengl_state->program_id);
        if ((0 == state_cache->opengl_state->program_id) || (_success_out == false))
        {
            Log_e("Unable to create a shader program.");
            _success_out = false;
            return;
        }
    }

    // Set initial camera & model positions.
    {
        SetupModels(_success_out);
        if (false == _success_out)
        {
            Log_e("Unable to set up models.");
            _success_out = false;
            return;
        }
    }

    _success_out = true;
}

int
main()
{
    state::StateCache* state_cache = state::StateCache::GetInstance();

    bool success; // [ cfarvin::TODO ] Now goes in state cache.
    Initialize(success);
    if (false == success)
    {
        Log_e("Unable to initialize.");
        std::exit(EXIT_FAILURE);
    }

    while ((true == state_cache->is_running) && (true == success))
    {
        app_window::SwapBuffers();
        app_window::ProcessPlatformEvents();
        DisplayLoop(success);
    }

    if (false == success)
    {
        Log_e("Error during render loop.");
    }

    state_cache->DumpState();

    if (true == success)
    {
        Log_i("Graceful exit.");
        std::exit(EXIT_SUCCESS);
    }
    else
    {
        Log_e("Exited with errors.");
        std::exit(EXIT_FAILURE);
    }
}

// clang-format off
#include "pch.h"
// clang-format on

// [ cfarvin::TODO ] After adding the renderer and engine classes,
//                   it is likely that many of these can be removed.
#include "app_window.h"
#include "camera.h"
#include "engine.h"
#include "gl_function_wrappers.h"
#include "gl_tools.h"
#include "image_tools.h"
#include "logging.h"
#include "model.h"
#include "state_tools.h"
#include "timer.h"
#include "windows_platform.h"

#define PI 3.1415926535f

void
SetupGraphicsAPI(bool& _success_out)
{
    const state::StateCache* const state_cache = state::StateCache::GetInstance();
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

    // Ensure that the aspect ratio is set (non-zero).
    if ((0 == state_cache->window_state.window_height) ||
        (0 == state_cache->window_state.window_width) ||
        (0 == state_cache->window_state.aspect_ratio))
    {
        Log_e("Unable to set projection matrix for this rendering program. Invalid window "
              "state.");
        _success_out = false;
        return;
    }
}

void
SetupModels(bool& _success_out)
{
    _success_out = false;

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

    // [ cfarvin::TODO ] Should this "Test Model" scope resolution operator be moved up a few lines?
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

        // Add matrices.
        {
            const state::StateCache* const state_cache = state::StateCache::GetInstance();

            // [ cfarvin::TEMPORARY ] There is currently only one rendering program in use.
            test_model.AddUniformMatrix(_success_out,
                                        std::move(state_cache->opengl_state->program_id),
                                        MatrixType::mtMODEL_VIEW_MATRIX,
                                        "mv_matrix");

            test_model.ModifyUniformMatrix(std::move(state_cache->opengl_state->program_id),
                                           MatrixType::mtMODEL_VIEW_MATRIX,
                                           glm::mat4(1.0f));
        }
    }

    return;
}

void
Chapter6Initialize(bool& _success_out)
{
    SetupGraphicsAPI(_success_out);
    if (false == _success_out)
    {
        Log_e("Unable to initialize graphics API.");
        return;
    }

    SetupModels(_success_out);
    if (false == _success_out)
    {
        Log_e("Unable to initialize models.");
        return;
    }
}

int
main()
{
    bool    success; // [ cfarvin::REVISIT ] Into state cache?
    Engine& engine = Engine::GetInstance();
    engine.Initialize(success, "../src/chapter_6/chapter_6.sus");
    if (false == success)
    {
        Log_e("Unable to initialize the engine.");
        std::exit(EXIT_FAILURE);
    }

    engine.AddRenderer(success, Chapter6Initialize);

    while ((true == state_cache->is_running) && (true == success))
    {
        app_window::SwapBuffers();
        app_window::ProcessPlatformEvents();

        if (false == state_cache->is_running || false == success) break;
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

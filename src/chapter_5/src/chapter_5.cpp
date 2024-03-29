#define PI 3.1415926535f

// #define GLM_FORCE_MESSAGES

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gl_tools.h"
#include "glm/glm.hpp"
#pragma warning(disable : 4201)
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr and glm::translate
#pragma warning(default : 4201)
#include "assert.h"
#include "camera.h"
#include "image_tools.h"
#include "logging.h"
#include "model.h"
#include "app_window.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>

GLuint                rendering_program;
GLuint                vao;
std::stack<glm::mat4> matrix_stack = {};
AppWindow*               window;
Camera                camera;
TexturedModel         sun_pyramid;
glm::mat4             v_mat;
glm::mat4             p_mat;
GLuint                mv_location;
GLuint                p_location;

struct DisplayVars
{
    glm::vec3 v3;
    glm::mat4 m4;
};
DisplayVars dv;

void
Display(bool& _success_out)
{
    const float current_time = static_cast<float>(glfwGetTime());
    dv.v3                    = glm::vec3(8 * sin(current_time), 0, 8 * cos(current_time));

    //
    // Model Independent
    //
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(rendering_program);
        mv_location = glt::GetUniformLocation(std::move(rendering_program), "mv_matrix");

        // Projection Matrix
        p_location = glt::GetUniformLocation(std::move(rendering_program), "proj_matrix");
        glt::SetUniformValueMat4(std::move(p_location), glm::value_ptr(p_mat));
    }

    //
    // Camera -- LookAt
    //
    {
        sun_pyramid.GetPosition(dv.v3.x, dv.v3.y, dv.v3.z);
        camera.LookAt(dv.v3, v_mat);
        matrix_stack.push(v_mat);
    }

    //
    // Pyramid
    //
    {
        // Matrix Stacks
        sun_pyramid.GetPosition(dv.v3.x, dv.v3.y, dv.v3.z);
        matrix_stack.push(matrix_stack.top()); // Translation slot
        matrix_stack.top() *= glm::translate(glm::mat4(1.0f),
                                             dv.v3); // Translation matrix

        matrix_stack.push(matrix_stack.top()); // Rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          current_time,
                                          glm::vec3(1.0f, 0.0f, 0.0f)); // Rotation matrix

        matrix_stack.push(matrix_stack.top()); // Scale slot
        matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                         glm::vec3(2.25f, 2.25f, 2.25f)); // Scale matrix

        // Copy matrices to corresponding uniform values (no view yet).
        glt::SetUniformValueMat4(std::move(mv_location), glm::value_ptr(matrix_stack.top()));

        // Draw
        sun_pyramid.Draw();

        matrix_stack.pop(); // Scale matrix removed, rotation matrix on top.
        matrix_stack.pop(); // Rotation matrix removed, translation matrix on top.
    }

    // Clear stack
    for (auto i = 0; i < matrix_stack.size(); i++)
    {
        matrix_stack.pop();
    }

    // [ cfarvin::DEBUG ]
    glt::QueryGLErrors(_success_out);
    if (false == _success_out)
    {
        return;
    }

    _success_out = true;
}

// Note: This function is forward declared in app_window.cpp.
void
OnWindowResize(GLFWwindow* glfw_window, int new_width, int new_height)
{
    if (glfw_window)
    {
    }

    // View Port
    glViewport(0, 0, new_width, new_height);

    // Aspect Ratio
    float aspect_ratio = (float)new_width / (float)new_height;

    // Perspective Matrix
    p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                             aspect_ratio,
                             0.1f,     // Near clipping plane.
                             1000.0f); // Far clipping plane.

    p_location = glt::GetUniformLocation(std::move(rendering_program), "proj_matrix");
    glt::SetUniformValueMat4(std::move(p_location), glm::value_ptr(p_mat));
}

void
SetupModels(bool& _success_out)
{
    _success_out = false;
    glGenVertexArrays(1, &vao);

    // Pyramid
    {
        // clang-format off
        const float pyramid_vertex_positions[54] =
        {
            -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +0.0f, +1.0f, +0.0f, // Front face
            +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +0.0f, +1.0f, +0.0f, // Right face
            +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +0.0f, +1.0f, +0.0f, // Back face
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, +0.0f, +1.0f, +0.0f, // Left face
            -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, // Base - left
            +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f  // Base - right
        };
        // clang-format on

        GLuint                                       position_attribute_id = 0;
        std::vector<glt::VertexAttributeDescriptor> position_attributes;
        position_attributes.push_back(
          { position_attribute_id, // Index of generic attribute to be modified.
            3,                     // Number of components per attribute [1, 4].
            GL_FLOAT,              // Data type of elements.
            GL_FALSE,              // Should be normalized.
            0,                     // Byte offset between elements within array.
            0 });                  // Offset of 1st component in array.

        sun_pyramid.AddBuffer(_success_out,
                              std::move(vao),
                              (GLvoid*)pyramid_vertex_positions,
                              sizeof(pyramid_vertex_positions),
                              GL_ARRAY_BUFFER,
                              (sizeof(pyramid_vertex_positions) / 3),
                              GL_STATIC_DRAW,
                              GL_TRIANGLES,
                              glt::VertexDataType::POSITION,
                              position_attributes);

        if (false == _success_out)
        {
            Log_e("Unable to create pyramid position buffer.");
            return;
        }

        sun_pyramid.EnableVertexAttribute(_success_out,
                                          glt::VertexDataType::POSITION,
                                          std::move(position_attribute_id));

        if (false == _success_out)
        {
            Log_e("Unable to enable position vertex attribute.");
            return;
        }

        // clang-format off
        const float pyramid_texture_coordinates[36] =
        {
            0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // top and right faces
            0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f, // back and left faces
            0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f  // base triangles
        };
        // clang-format on

        GLuint                                       texture_coordinate_attribute_id = 1;
        std::vector<glt::VertexAttributeDescriptor> texture_coordinate_attributes;
        texture_coordinate_attributes.push_back(
          { texture_coordinate_attribute_id, 2, GL_FLOAT, GL_FALSE, 0, 0 });

        GLuint texture_id;
        sun_pyramid.CreateTexture(_success_out,
                                  "./../src/chapter_5/assets/texture.png",
                                  texture_id,
                                  std::move(vao),
                                  pyramid_texture_coordinates,
                                  sizeof(pyramid_texture_coordinates),
                                  GL_ARRAY_BUFFER,
                                  sizeof(pyramid_texture_coordinates) / 3,
                                  GL_STATIC_DRAW,
                                  GL_TRIANGLES,
                                  GL_TEXTURE_2D,
                                  texture_coordinate_attributes);
        if (false == _success_out)
        {
            Log_e("Unable to create pyramid texture.");
            return;
        }

        sun_pyramid.EnableVertexAttribute(_success_out,
                                          glt::VertexDataType::TEXTURE,
                                          std::move(texture_coordinate_attribute_id));
        if (false == _success_out)
        {
            Log_e("Unable to enable pyramid texture coordinate vertex attribute.");
            return;
        }

        sun_pyramid.SetTextureUnit(GL_TEXTURE0);
    }

    glt::QueryGLErrors(_success_out);
    if (false == _success_out)
    {
        Log_e("OpenGL errors during model setup.");
    }

    return;
}

void
Initialize(bool& _success_out)
{
    _success_out                            = false;
    constexpr int          gl_major_version = 4;
    constexpr int          gl_minor_version = 6;
    constexpr unsigned int height           = 600;
    constexpr unsigned int width            = 600;

    // Setup GLEW, GLFW, and the window.
    {
        window = new AppWindow(gl_major_version, // GL Major Version
                            gl_minor_version, // GL Minor Version
                            width,            // AppWindow Width
                            height,           // AppWindow Height
                            "Chapter 5",      // AppWindow Title
                            _success_out);    // Error checking

        if (false == _success_out)
        {
            Log_e("Unable to set up GLEW/GLFW.");
            return;
        }

        glt::QueryGLErrors(_success_out);
        if (false == _success_out)
        {
            Log_e("OpenGL errors in window setup.");
            return;
        }
    }

    // Create the shader program.
    {
        glt::CreateShaderProgram(_success_out,
                                 "./../src/chapter_5/src/shaders/vertex_shader.glsl",
                                 "./../src/chapter_5/src/shaders/fragment_shader.glsl",
                                 rendering_program);
        if ((0 == rendering_program) || (_success_out == false))
        {
            Log_e("Unable to create a shader program.");
            glfwDestroyWindow(window->glfw_window);
            glfwTerminate();
            _success_out = false;
            return;
        }
    }

    // Initial setup perspective matrix
    {
        p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                                 ((float)width / (float)height),
                                 0.1f,     // Near clipping plane.
                                 1000.0f); // Far clipping plane.

        glUseProgram(rendering_program);
        p_location = glt::GetUniformLocation(std::move(rendering_program), "proj_matrix");
        glt::SetUniformValueMat4(std::move(p_location), glm::value_ptr(p_mat));

        glt::QueryGLErrors(_success_out);
        if (false == _success_out)
        {
            Log_e("Unable to setup perspective matrix.");
            return;
        }
    }

    // Set initial camera & model positions.
    {
        camera.SetPosition(0.0f, 0.0f, 15.0f);
        sun_pyramid.SetPosition(0.0f, 0.0f, 0.0f);

        SetupModels(_success_out);
        if (false == _success_out)
        {
            Log_e("Unable to set up models.");
            _success_out = false;
            return;
        }

        glt::QueryGLErrors(_success_out);
        if (false == _success_out)
        {
            Log_e("OpenGL errors in model setup.");
            return;
        }
    }

    _success_out = true;
}

int
main()
{
    bool success;
    Initialize(success);
    if (false == success)
    {
        Log_e("Unable to initialize.");
        std::exit(EXIT_FAILURE);
    }

    while ((false == glfwWindowShouldClose(window->glfw_window)) && success)
    {
        Display(success);
        glfwSwapBuffers(window->glfw_window);
        glfwPollEvents();
    }

    if (false == success)
    {
        Log_e("Error during render loop.");
    }

    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();

    if (nullptr != window)
    {
        delete window;
    }

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

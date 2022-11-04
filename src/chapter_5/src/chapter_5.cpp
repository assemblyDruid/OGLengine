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
#include "windowing.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>

GLuint                rendering_program;
GLuint                vao;
std::stack<glm::mat4> matrix_stack = {};
Window*               window;
Camera                camera;
BufferedModel         cube;
TexturedModel         sun_pyramid;
BufferedModel         sphere;
Model                 earth_cube;
Model                 moon_cube;
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
Display(bool& _success)
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
        mv_location = GetUniformLocation(rendering_program, "mv_matrix");

        // [ cfarvin::TODO ] Uniform Buffer Object for projection
        // Projection Matrix
        p_location = GetUniformLocation(rendering_program, "proj_matrix");
        SetUniformValueMat4(p_location, glm::value_ptr(p_mat));
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
    // Pyramid: Vertex Buffer Object 1
    // "Sun"
    //
    {
        // Matrix Stacks
        sun_pyramid.GetPosition(dv.v3.x, dv.v3.y, dv.v3.z);
        matrix_stack.push(matrix_stack.top()); // Sun translation slot
        matrix_stack.top() *= glm::translate(glm::mat4(1.0f),
                                             dv.v3); // Sun translation matrix

        matrix_stack.push(matrix_stack.top()); // Sun rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          current_time,
                                          glm::vec3(1.0f, 0.0f, 0.0f)); // Sun rotation matrix

        matrix_stack.push(matrix_stack.top()); // Sun scale slot
        matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                         glm::vec3(2.25f, 2.25f, 2.25f)); // Sun scale matrix

        // Copy matrices to corresponding uniform values (no view yet).
        SetUniformValueMat4(mv_location, glm::value_ptr(matrix_stack.top()));

        // Texturing

        glBindBuffer(GL_ARRAY_BUFFER, sun_pyramid.texture_vbo);
        glVertexAttribPointer(1,        // Index
                              2,        // Size
                              GL_FLOAT, // Type
                              GL_FALSE, // Normalized
                              0,        // Stride
                              0);       // Pointer
        glEnableVertexAttribArray(1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sun_pyramid.texture_id);

        // Draw
        sun_pyramid.Draw();

        matrix_stack.pop(); // Sun scale matrix removed, sun rotation matrix on top.
        matrix_stack.pop(); // Sun rotation matrix removed, sun translation matrix on top.
    }

    // Clear stack
    for (auto i = 0; i < matrix_stack.size(); i++)
    {
        matrix_stack.pop();
    }

    // [ cfarvin::DEBUG ]
    QueryGlErrors(_success);
    if (false == _success)
    {
        return;
    }

    _success = true;
}

// Note: This function is forward declared in windowing.cpp.
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

    p_location = GetUniformLocation(rendering_program, "proj_matrix");
    SetUniformValueMat4(p_location, glm::value_ptr(p_mat));
}

void
SetupModels(bool& _success)
{
    _success = false;
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

        GLuint                       position_attribute_id = 0;
        std::vector<VertexAttribute> position_attributes;
        position_attributes.push_back(
          { position_attribute_id, // Index of generic attribute to be modified.
            3,                     // Number of components per attribute [1, 4].
            GL_FLOAT,              // Data type of elements.
            GL_FALSE,              // Should be normalized.
            0,                     // Byte offset between elements within array.
            0 });                  // Offset of 1st component in array.

        sun_pyramid.AddBuffer(_success,
                              vao,
                              (GLvoid*)pyramid_vertex_positions,
                              sizeof(pyramid_vertex_positions),
                              (sizeof(pyramid_vertex_positions) / 3),
                              GL_STATIC_DRAW,
                              GL_TRIANGLES,
                              GLBufferType::POSITION_COORDINATES,
                              position_attributes);

        if (false == _success)
        {
            Log_e("Unable to create pyramid position buffer.");
            return;
        }

        sun_pyramid.EnableVertexAttribute(_success, position_attribute_id);
        if (false == _success)
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

        sun_pyramid.CreateTexture(_success, "./../src/chapter_5/assets/texture.png");
        // Load textures.
        {
            glGenBuffers(1, &sun_pyramid.texture_vbo);
            glBindBuffer(GL_ARRAY_BUFFER, sun_pyramid.texture_vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         sizeof(pyramid_texture_coordinates),
                         pyramid_texture_coordinates,
                         GL_STATIC_DRAW);
        }
    }

    QueryGlErrors(_success);
    if (false == _success)
    {
        Log_e("OpenGL errors during model setup.");
    }

    return;
}

void
Initialize(bool& _success)
{
    _success                                = false;
    constexpr int          gl_major_version = 4;
    constexpr int          gl_minor_version = 6;
    constexpr unsigned int height           = 600;
    constexpr unsigned int width            = 600;

    // Setup GLEW, GLFW, and the window.
    {
        window = new Window(gl_major_version, // GL Major Version
                            gl_minor_version, // GL Minor Version
                            width,            // Window Width
                            height,           // Window Height
                            "Chapter 5",      // Window Title
                            _success);        // Error checking

        if (false == _success)
        {
            Log_e("Unable to set up GLEW/GLFW.");
            return;
        }

        QueryGlErrors(_success);
        if (false == _success)
        {
            Log_e("OpenGL errors in window setup.");
            return;
        }
    }

    // Create the shader program.
    {
        rendering_program = CreateShaderProgram(
          "./../src/chapter_5/src/shaders/vertex_shader.glsl",
          "./../src/chapter_5/src/shaders/fragment_shader.glsl");
        if (0 == rendering_program)
        {
            Log_e("Unable to create a shader program.");
            glfwDestroyWindow(window->glfw_window);
            glfwTerminate();
            _success = false;
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
        p_location = GetUniformLocation(rendering_program, "proj_matrix");
        SetUniformValueMat4(p_location, glm::value_ptr(p_mat));

        QueryGlErrors(_success);
        if (false == _success)
        {
            Log_e("Unable to setup perspective matrix.");
            return;
        }
    }

    // [ cfarvin::TODO ] These need to be moved.
    // Set initial camera & model positions.
    {
        camera.SetPosition(0.0f, 0.0f, 15.0f);
        earth_cube.SetPosition(0.0f, -2.0f, 0.0f);
        moon_cube.SetPosition(0.0f, 0.0f, 0.0f);
        sun_pyramid.SetPosition(0.0f, 0.0f, 0.0f);
        sphere.SetPosition(0.0f, 0.0f, 0.0f);

        SetupModels(_success);
        if (false == _success)
        {
            Log_e("Unable to set up models.");
            _success = false;
            return;
        }

        QueryGlErrors(_success);
        if (false == _success)
        {
            Log_e("OpenGL errors in model setup.");
            return;
        }
    }

    _success = true;
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
        Log_e("Exitied with errors.");
        std::exit(EXIT_FAILURE);
    }
}

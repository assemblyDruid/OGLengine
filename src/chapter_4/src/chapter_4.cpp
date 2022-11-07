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
BufferedModel         sun_pyramid;
BufferedModel         sphere;
Model                 earth_cube;
Model                 moon_cube;
glm::mat4             v_mat;
glm::mat4             p_mat;
GLuint                mv_location;
GLuint                p_location;

void
SetupModels(bool& _success)
{
    _success = false;
    glGenVertexArrays(1, &vao);

    // Cube
    {
        const float cube_vertex_positions[108] = {
            -1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f,
            +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f,
            +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f,
            +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
            -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f,
            -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f,
            -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f,
            +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f
        };

        GLuint                            position_attribute_id = 0;
        std::vector<glt::VertexAttribute> position_attributes;
        position_attributes.push_back({ 0, 3, GL_FLOAT, GL_FALSE, 0, 0 });

        cube.AddBuffer(_success,
                       std::move(vao),
                       (GLvoid*)cube_vertex_positions,
                       sizeof(cube_vertex_positions),
                       (sizeof(cube_vertex_positions) / 3),
                       GL_STATIC_DRAW,
                       GL_TRIANGLES,
                       glt::GLBufferType::POSITION_COORDINATES,
                       position_attributes);

        if (false == _success)
        {
            Log_e("Unable to create cube buffer");
            return;
        }

        cube.EnableVertexAttribute(_success,
                                   glt::GLBufferType::POSITION_COORDINATES,
                                   std::move(position_attribute_id));

        if (false == _success)
        {
            Log_e("Unable to enable cube position vertex attribute.");
            return;
        }
    }

    // Pyramid
    {
        // clang-format off
        const float pyramid_vertex_positions[54] =
        {
            -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +0.0f, +1.0f, +0.0f, // Front face
            +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +0.0f, +1.0f, +0.0f, // Right face
            +1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +0.0f, +1.0f, +0.0f, // Back face
            -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, +0.0f, +1.0f, +0.0f, // Left face
            -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, // Base - left front
            +1.0f, -1.0f, +1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f  // Base - right back
        };
        // clang-format on

        GLuint                            position_attribute_id = 0;
        std::vector<glt::VertexAttribute> position_attributes;
        position_attributes.push_back({ 0, 3, GL_FLOAT, GL_FALSE, 0, 0 });

        sun_pyramid.AddBuffer(_success,
                              std::move(vao),
                              (GLvoid*)pyramid_vertex_positions,
                              sizeof(pyramid_vertex_positions),
                              (sizeof(pyramid_vertex_positions) / 3),
                              GL_STATIC_DRAW,
                              GL_TRIANGLES,
                              glt::GLBufferType::POSITION_COORDINATES,
                              position_attributes);

        if (false == _success)
        {
            Log_e("Unable to create pyramid buffer");
            return;
        }

        sun_pyramid.EnableVertexAttribute(_success,
                                          glt::GLBufferType::POSITION_COORDINATES,
                                          std::move(position_attribute_id));

        if (false == _success)
        {
            Log_e("Unable to enable pyramid position vertex attribute.");
            return;
        }
    }

    // Sphere
    {
        constexpr float  ring_count            = 25.0f;
        constexpr float  points_per_ring_count = 55.0f;
        constexpr size_t sphere_index_count    = static_cast<size_t>(ring_count *
                                                                  points_per_ring_count * 3);

        size_t          index   = 0;
        float           theta   = 0.0f;
        float           phi     = 0.0f;
        constexpr float d_theta = PI / ring_count;              // Spacing between rings.
        constexpr float d_phi = 2 * PI / points_per_ring_count; // Spacing between points in a ring.

        float sphere_vertex_positions[sphere_index_count] = {};
        for (auto ring_number = 0; ring_number < ring_count; ring_number++)
        {
            theta += d_theta;
            for (auto point_number = 0; point_number < points_per_ring_count; point_number++)
            {
                phi += d_phi;
                sphere_vertex_positions[index++] = static_cast<float>(sin(theta) * cos(phi)); // X
                sphere_vertex_positions[index++] = static_cast<float>(sin(theta) * sin(phi)); // Y
                sphere_vertex_positions[index++] = static_cast<float>(cos(theta));            // Z
            }
        }

        GLuint                            position_attribute_id = 0;
        std::vector<glt::VertexAttribute> position_attributes;
        position_attributes.push_back({ 0, 3, GL_FLOAT, GL_FALSE, 0, 0 });

        sphere.AddBuffer(_success,
                         std::move(vao),
                         (GLvoid*)sphere_vertex_positions,
                         sizeof(sphere_vertex_positions),
                         (sizeof(sphere_vertex_positions) / 3),
                         GL_STATIC_DRAW,
                         GL_TRIANGLE_FAN,
                         glt::GLBufferType::POSITION_COORDINATES,
                         position_attributes);

        if (false == _success)
        {
            Log_e("Unable to create pyramid buffer");
            return;
        }

        sphere.EnableVertexAttribute(_success,
                                     glt::GLBufferType::POSITION_COORDINATES,
                                     std::move(position_attribute_id));

        if (false == _success)
        {
            Log_e("Unable to enable pyramid position vertex attribute.");
            return;
        }
    }
}

struct DisplayVars
{
    glm::vec3 v3;
    glm::mat4 m4;
};
DisplayVars dv;

void
Display(bool& _success)
{
    _success = false;

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
        glt::SetUniformValueMat4(std::move(mv_location), glm::value_ptr(matrix_stack.top()));

        // Draw
        sun_pyramid.Draw();

        matrix_stack.pop(); // Sun scale matrix removed, sun rotation matrix on top.
        matrix_stack.pop(); // Sun rotation matrix removed, sun translation matrix on top.
    }

    //
    // Sphere: Vertex Buffer Object 2
    // "Chapter 4 Exercise 1"
    //
    {
        // Matrix Stacks
        matrix_stack.push(matrix_stack.top()); // Sphere translation slot
        matrix_stack.top() *= glm::translate(glm::mat4(1.0f),
                                             glm::vec3(10.0f * cos(current_time),
                                                       10.0f * sin(current_time),
                                                       0.0f)); // Sphere translation matrix
        matrix_stack.push(matrix_stack.top());                 // Sphere rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          current_time,
                                          glm::vec3(0.0f, 1.0f, 1.0f)); // Sphere rotation matrix
        //
        // matrix_stack.push(matrix_stack.top());                          // Sphere scale slot
        // matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
        //                                  glm::vec3(2.25f, 2.25f, 2.25f)); // Sphere scale matrix
        //

        // Copy matrices to corresponding uniform values (no view yet).
        glt::SetUniformValueMat4(std::move(mv_location), glm::value_ptr(matrix_stack.top()));

        // Draw
        sphere.Draw();

        //
        // matrix_stack.pop(); // Sphere scale matrix removed, sphere rotation matrix on top.
        //

        matrix_stack.pop(); // Sphere rotation matrix removed, sphere translation matrix on top.
        matrix_stack.pop(); // Sphere translation matrix removed, sun translation matrix on top.
    }

    //
    // Cube: Vertex Buffer Object  0
    // "Earth"
    //
    {
        // Matrix Stacks
        matrix_stack.push(matrix_stack.top()); // Earth translate (around sun) slot
        matrix_stack.top() *= glm::translate(
          glm::mat4(1.0f),
          glm::vec3(8.0f * sin(current_time),
                    0.0f,
                    8.0f * cos(current_time))); // Earth translate (around sun) matrix
        matrix_stack.push(matrix_stack.top());  // Earth rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          5 * current_time,
                                          glm::vec3(0.0f, 1.0f, 0.0f)); // Earth rotation matrix

        // Copy matrices to corresponding uniform values (no view yet).
        glt::SetUniformValueMat4(std::move(mv_location), glm::value_ptr(matrix_stack.top()));

        // Draw
        cube.Draw();

        matrix_stack.pop(); // Earth rotation matrix removed, earth translate (around sun) on top.
    }

    //
    // Cube: Vertex Buffer Object 0
    // "Moon"
    //
    {
        // Matrix Stacks
        matrix_stack.push(matrix_stack.top()); // Moon translate (around earth) slot
        matrix_stack.top() *= glm::translate(
          glm::mat4(1.0f),
          glm::vec3(0.0f,
                    2.0f * sin(current_time),
                    2.0f * cos(current_time))); // Moon translate (around earth) matrix
        matrix_stack.push(matrix_stack.top());  // Moon rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          3 * current_time,
                                          glm::vec3(0.0f, 0.0f, 1.0f)); // Moon rotation matrix
        matrix_stack.push(matrix_stack.top());                          // Moon scale slot
        matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                         glm::vec3(0.25f,
                                                   0.25f,
                                                   0.25f)); // Moon scale rotation

        // Copy matrices to corresponding uniform values (no view yet).
        glt::SetUniformValueMat4(std::move(mv_location), glm::value_ptr(matrix_stack.top()));

        // Draw
        cube.Draw();
    }

    // Clear stack
    for (auto i = 0; i < matrix_stack.size(); i++)
    {
        matrix_stack.pop();
    }

    glt::QueryGLErrors(_success);
    if (false == _success)
    {
        Log_e("OpenGL error in reder loop.");
    }

    return;
}

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
Initialize(bool& _success)
{
    _success = false;

    constexpr int          gl_major_version = 4;
    constexpr int          gl_minor_version = 5;
    constexpr unsigned int height           = 600;
    constexpr unsigned int width            = 600;

    // Setup GLEW, GLFW, and the window.
    {
        bool glew_glfw_window_success = true;

        window = new Window(gl_major_version,          // GL Major Version
                            gl_minor_version,          // GL Minor Version
                            width,                     // Window Width
                            height,                    // Window Height
                            "Chapter 4",               // Window Title
                            glew_glfw_window_success); // Error checking

        if (false == glew_glfw_window_success)
        {
            Log_e("Unable to set up GLEW/GLFW.");
            return;
        }
    }

    // Create the shader program.
    {
        rendering_program = glt::CreateShaderProgram(
          "./../src/chapter_4/src/shaders/vertex_shader.glsl",
          "./../src/chapter_4/src/shaders/fragment_shader.glsl");
        if (0 == rendering_program)
        {
            Log_e("Unable to create a shader program.");
            return;
        }
    }

    // Initial setup perspective matrix
    {
        p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                                 ((float)width / (float)height),
                                 0.1f,     // Near clipping plane.
                                 1000.0f); // Far clipping plane.

        p_location = glt::GetUniformLocation(std::move(rendering_program), "proj_matrix");
        glt::SetUniformValueMat4(std::move(p_location), glm::value_ptr(p_mat));
    }

    // [ cfarvin::TODO ] These need to be moved.
    // Set initial camera & model positions.
    {
        camera.SetPosition(0.0f, 0.0f, 20.0f);
        earth_cube.SetPosition(0.0f, -2.0f, 0.0f);
        moon_cube.SetPosition(0.0f, 0.0f, 0.0f);
        sun_pyramid.SetPosition(0.0f, 0.0f, 0.0f);
        sphere.SetPosition(0.0f, 0.0f, 0.0f);

        SetupModels(_success);
        if (false == _success)
        {
            Log_e("Unable to setup models.");
        }
    }

    _success = true;
}

int
main()
{
    bool success = false;

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

    glfwDestroyWindow(window->glfw_window);
    glfwTerminate();

    if (nullptr != window)
    {
        delete window;
    }

    if (true == success)
    {
        Log_e("Graceful exit.");
        std::exit(EXIT_SUCCESS);
    }
    else
    {
        Log_e("Exited with errors.");
        std::exit(EXIT_FAILURE);
    }
}

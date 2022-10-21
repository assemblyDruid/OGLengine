#define NUM_VAO 1
#define NUM_VBO 3
#define PI      3.1415926535f

// #define GLM_FORCE_MESSAGES

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "SOIL2/SOIL2.h"
#include "gl_tools.h"
#include "glm/glm.hpp"
#pragma warning(disable : 4201)
#include "glm/gtc/type_ptr.hpp"
#pragma warning(default : 4201)
#include "logging.h"
#include "windowing.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>

GLuint rendering_program;
GLuint vao[NUM_VAO];
GLuint vbo[NUM_VBO];

glm::mat4 t_mat; // General Translation Matrix
glm::mat4 r_mat; // General Rotation Matrix

struct MVP
{
    GLuint mv_location;
    GLuint p_location;

    glm::mat4 p_mat;
    glm::mat4 v_mat;
    glm::mat4 m_mat;
    glm::mat4 mv_mat;
};
MVP                   mvp          = {};
std::stack<glm::mat4> matrix_stack = {};

Window* window;

struct Position3
{
    void
    SetPosition(const float&& _x, const float&& _y, const float&& _z)
    {
        x = _x;
        y = _y;
        z = _z;
    };

    float x;
    float y;
    float z;
};

struct Camera : Position3
{
};
Camera camera;

struct Cube : Position3
{
    const unsigned int vert_count = 36;
};
Cube earth;
Cube moon;

struct Pyramid : Position3
{
    const unsigned int vert_count = 54;
};
Pyramid sun;

struct Sphere : Position3
{
    // Non const, may change.
    unsigned int vert_count = 0;
};
Sphere sphere;

void
SetupModels()
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

    // Generate Sphere
    constexpr float  ring_count            = 25.0f;
    constexpr float  points_per_ring_count = 55.0f;
    constexpr size_t sphere_vert_count = static_cast<size_t>(ring_count * points_per_ring_count *
                                                             3);

    size_t          index   = 0;
    float           theta   = 0.0f;
    float           phi     = 0.0f;
    constexpr float d_theta = PI / ring_count;                // Spacing between rings.
    constexpr float d_phi   = 2 * PI / points_per_ring_count; // Spacing between points in a ring.

    float sphere_vertex_positions[sphere_vert_count] = {};
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
    sphere.vert_count = static_cast<unsigned int>(index);

    // VAOs and VBOs
    glGenVertexArrays(NUM_VAO, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(NUM_VBO, vbo);

    // Cube
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(cube_vertex_positions),
                 cube_vertex_positions,
                 GL_STATIC_DRAW);

    // Pyramid
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(pyramid_vertex_positions),
                 pyramid_vertex_positions,
                 GL_STATIC_DRAW);

    // Sphere
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(sphere_vertex_positions),
                 sphere_vertex_positions,
                 GL_STATIC_DRAW);
}

bool
Display()
{
    const float current_time = static_cast<float>(glfwGetTime());

    //
    // Model Independent
    //
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glUseProgram(rendering_program);
        mvp.mv_location = GetUniformLocation(rendering_program, "mv_matrix");

        // Projection Matrix
        mvp.p_location = GetUniformLocation(rendering_program, "proj_matrix");
        SetUniformValueMat4(mvp.p_location, glm::value_ptr(mvp.p_mat));

        // View Matrix
        mvp.v_mat = glm::translate(glm::mat4(1.0f), glm::vec3(-camera.x, -camera.y, -camera.z));
        matrix_stack.push(mvp.v_mat);
    }

    //
    // Pyramid: Vertex Buffer Object 1
    // "Sun"
    //
    {
        // Matrix Stacks
        matrix_stack.push(matrix_stack.top()); // Sun translation slot
        matrix_stack.top() *= glm::translate(
          glm::mat4(1.0f),
          glm::vec3(sun.x, sun.y, sun.z));     // Sun translation matrix
        matrix_stack.push(matrix_stack.top()); // Sun rotation slot
        matrix_stack.top() *= glm::rotate(glm::mat4(1.0f),
                                          current_time,
                                          glm::vec3(1.0f, 0.0f, 0.0f)); // Sun rotation matrix
        matrix_stack.push(matrix_stack.top());                          // Sun scale slot
        matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                         glm::vec3(2.25f, 2.25f, 2.25f)); // Sun scale matrix

        // Copy matricies to corresponding uniform values (no view yet).
        SetUniformValueMat4(mvp.mv_location, glm::value_ptr(matrix_stack.top()));

        // Associate VBO with vertex shader attributes.
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, sun.vert_count);
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

        // Copy matricies to corresponding uniform values (no view yet).
        SetUniformValueMat4(mvp.mv_location, glm::value_ptr(matrix_stack.top()));

        // Associate VBO with vertex shader attributes.
        glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sphere.vert_count);

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
                                          current_time,
                                          glm::vec3(0.0f, 1.0f, 0.0f)); // Earth rotation matrix

        // Copy matricies to corresponding uniform values (no view yet).
        SetUniformValueMat4(mvp.mv_location, glm::value_ptr(matrix_stack.top()));

        // Associate VBO with vertex shader attributes.
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, earth.vert_count);
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
                                          current_time,
                                          glm::vec3(0.0f, 0.0f, 1.0f)); // Moon rotation matrix
        matrix_stack.push(matrix_stack.top());                          // Moon scale slot
        matrix_stack.top() *= glm::scale(glm::mat4(1.0f),
                                         glm::vec3(0.25f,
                                                   0.25f,
                                                   0.25f)); // Moon scale rotation

        // Copy matricies to corresponding uniform values (no view yet).
        SetUniformValueMat4(mvp.mv_location, glm::value_ptr(matrix_stack.top()));

        // Associate VBO with vertex shader attributes.
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, moon.vert_count);

        // Clear stack
        for (auto i = 0; i < matrix_stack.size(); i++)
        {
            matrix_stack.pop();
        }
    }

    return true;
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
    mvp.p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                                 aspect_ratio,
                                 0.1f,     // Near clipping plane.
                                 1000.0f); // Far clipping plane.
}

const bool
Initialize()
{
    constexpr int height = 600;
    constexpr int width  = 600;

    // Setup GLEW, GLFW, and the window.
    {
        bool glew_glfw_window_success = true;

        window = new Window(4,                         // GL Major Version
                            5,                         // GL Minor Version
                            width,                     // Window Width
                            height,                    // Window Height
                            "Chapter 4",               // Window Title
                            glew_glfw_window_success); // Error checking

        if (false == glew_glfw_window_success)
        {
            Log(LogType::ERROR, "Unable to set up GLEW/GLFW.");
            return false;
        }
    }

    // Create the shader program.
    {
        rendering_program = CreateShaderProgram(
          "./../src/chapter_4/src/shaders/vertex_shader.glsl",
          "./../src/chapter_4/src/shaders/fragment_shader.glsl");
        if (0 == rendering_program)
        {
            Log(LogType::ERROR, "Unable to create a shader program.");
            return false;
        }
    }

    // Setup perspective matrix
    {
        mvp.p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                                     ((float)width / (float)height),
                                     0.1f,     // Near clipping plane.
                                     1000.0f); // Far clipping plane.
    }

    // [ cfarvin::TODO ] These need to be moved.
    // Set initial camera & model positions.
    {
        camera.SetPosition(0.0f, 0.0f, 20.0f);
        earth.SetPosition(0.0f, -2.0f, 0.0f);
        moon.SetPosition(0.0f, 0.0f, 0.0f);
        sun.SetPosition(0.0f, 0.0f, 0.0f);
        sphere.SetPosition(0.0f, 0.0f, 0.0f);

        SetupModels();
    }

    return true;
}

int
main()
{
    bool success = Initialize();
    if (false == success)
    {
        Log(LogType::ERROR, "Unable to initialize.");
        std::exit(EXIT_FAILURE);
    }

    while ((false == glfwWindowShouldClose(window->glfw_window)) && success)
    {
        success = Display();
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
        Log(LogType::INFO, "Graceful exit.");
        std::exit(EXIT_SUCCESS);
    }
    else
    {
        Log(LogType::ERROR, "Exitied with errors.");
        std::exit(EXIT_FAILURE);
    }
}

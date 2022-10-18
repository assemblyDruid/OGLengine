#define NUM_VAO 1
#define NUM_VBO 2

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

    glGenVertexArrays(NUM_VAO, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(2, vbo);

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

        glUseProgram(rendering_program);
        mvp.mv_location = GetUniformLocation(rendering_program, "mv_matrix");
        mvp.p_location  = GetUniformLocation(rendering_program, "proj_matrix");

        // Perspective Matrix
        mvp.p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                                     window->GetAspectRatio(),
                                     0.1f,
                                     1000.0f);
        SetUniformValueMat4(mvp.p_location, glm::value_ptr(mvp.p_mat));

        // View Matrix
        mvp.v_mat = glm::translate(glm::mat4(1.0f), glm::vec3(-camera.x, -camera.y, -camera.z));
        matrix_stack.push(mvp.v_mat);
    }

    //
    // Pyramid: Buffer 1
    // "Sun"
    //
    {
        // Matrix Stacks
        matrix_stack.push(matrix_stack.top()); // Sun transloation slot
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
    // Cube: Buffer 0
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
    // Cube: Buffer 0
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

const bool
Initialize()
{
    bool glew_glfw_window_success = true;

    // Setup GLEW, GLFW, and the window.
    window = new Window(4,                         // GL Major Version
                        5,                         // GL Minor Version
                        600,                       // Window Width
                        600,                       // Window Height
                        "Chapter 4",               // Window Title
                        glew_glfw_window_success); // Error checking

    if (false == glew_glfw_window_success)
    {
        Log(LogType::ERROR, "Unable to set up GLEW/GLFW.");
        return false;
    }

    // Create the shader program.
    rendering_program = CreateShaderProgram("./../src/chapter_4/src/shaders/vertex_shader.glsl",
                                            "./../src/chapter_4/src/shaders/fragment_shader.glsl");
    if (0 == rendering_program)
    {
        Log(LogType::ERROR, "Unable to create a shader program.");
        return false;
    }

    // [ cfarvin::TODO ] These need to be moved.
    // Set initial camera & cube, and pyramid position.
    camera.SetPosition(0.0f, 0.0f, 20.0f);
    earth.SetPosition(0.0f, -2.0f, 0.0f);
    moon.SetPosition(0.0f, -2.0f, 0.0f);
    sun.SetPosition(0.0f, 0.0f, 0.0f);

    // Send cube vert data.
    SetupModels();

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

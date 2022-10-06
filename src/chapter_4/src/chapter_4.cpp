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
#include <stdio.h>
#include <string>

GLuint rendering_program;
GLuint vao[NUM_VAO];
GLuint vbo[NUM_VBO];

struct MVP
{
    void
    UpdateMVMat()
    {
        mv_mat = m_mat * p_mat;
    }

    GLuint mv_location;
    GLuint proj_location;

    glm::mat4 p_mat;
    glm::mat4 v_mat;
    glm::mat4 m_mat;
    glm::mat4 mv_mat;
};
MVP mvp = {};

Window window = {};

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
Cube cube;

void
SetUpCubeVerts()
{
    const float vertex_positions[108] = {
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

    glGenVertexArrays(NUM_VAO, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_STATIC_DRAW);
}

bool
Display()
{
    // const double current_time = glfwGetTime();

    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(rendering_program);

    SetUniformValue(mvp.mv_location, glm::value_ptr(mvp.m_mat));
    SetUniformValue(mvp.proj_location, glm::value_ptr(mvp.p_mat));

    const bool success = SendVertexData(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    if (true != success)
    {
        Log(LogType::ERROR, "Failed to send vertex data.");
        return false;
    }
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, cube.vert_count);

    return true;
}

const bool
Initialize()
{
    // Setup GLFW
    if (false == SetupWindow(&(window.window), // Window
                             4,                // GL Major Version
                             5,                // GL Minor Version
                             600,              // Window Width
                             600,              // Window Height
                             "Chapter 4"))     // Window Title
    {
        Log(LogType::ERROR, "Unable to set up GLFW.");
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

    // Set initial camera position.
    camera.SetPosition(0.0f, 0.0f, 8.0f);

    // Send cube vert data.
    SetUpCubeVerts();

    // Build matrices.
    // [ cfarvin::TODO ] This needs to be done on window re-size as well.
    glfwGetFramebufferSize(window.window, &window.width, &window.height);
    window.aspect_ratio = (float)window.width / (float)window.height;

    // Perspective Matrix
    mvp.p_mat = glm::perspective(1.0472f, // 1.0472 radians == 60 degrees
                                 window.aspect_ratio,
                                 0.1f,
                                 1000.0f);
    mvp.UpdateMVMat();

    // View Matrix
    mvp.v_mat = glm::translate(glm::mat4(1.0f), glm::vec3(-camera.x, -camera.y, -camera.z));

    // Model Matrix
    mvp.m_mat = glm::translate(glm::mat4(1.0f), glm::vec3(cube.x, cube.y, cube.z));

    mvp.mv_location   = GetUniformLocation(rendering_program, "mv_matrix");
    mvp.proj_location = GetUniformLocation(rendering_program, "proj_matrix");

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

    while (!glfwWindowShouldClose(window.window) && success)
    {
        success = Display();
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window.window);
    glfwTerminate();

    if (true == success)
    {
        std::exit(EXIT_SUCCESS);
    }
    else
    {
        std::exit(EXIT_FAILURE);
    }
}

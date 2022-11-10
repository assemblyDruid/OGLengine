#define NUM_VAO 1

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gl_tools.h"
#include "glm/glm.hpp"
#include "logging.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

GLuint rendering_program;
GLuint vao[NUM_VAO];
float  t  = 0.0f;
float  dt = 0.01f;

void
Init(bool& _success_out)
{
    glt::CreateShaderProgram(_success_out,
                             "./../src/chapter_2/src/shaders/vertex_shader.glsl",
                             "./../src/chapter_2/src/shaders/fragment_shader.glsl",
                             rendering_program);

    if ((0 == rendering_program) || (false == _success_out))
    {
        Log_e("Unable to create rendering program.");
        return;
    }

    // Note: OGL requires at least one VAO when shaders are being used.
    glGenVertexArrays(NUM_VAO, vao);
    glBindVertexArray(vao[0]);
}

void
Display(GLFWwindow* window, double current_time)
{
    if (window || current_time)
    {
    }

    t += dt;
    if (abs(t) >= 1)
    {
        dt *= -1;
    }

    glPointSize(50.0f);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(rendering_program);
    GLuint t_offset_location = glGetUniformLocation(rendering_program, "t");
    glProgramUniform1f(rendering_program, t_offset_location, t);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int
main()
{
    bool success = true;
    if (!glfwInit())
    {
        Log_e("Unable to initialize GLFW.");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    // Note: nullptr parameters allow for fullscreen mode and resource sharing.
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 2", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit())
    {
        Log_e("Unable to initialize GLEW.");
        std::exit(EXIT_FAILURE);
    }

    // Note: Enables V-Sync
    // Note: Glfw windows are double-buffered by default.
    glfwSwapInterval(1);

    Init(success);
    if (false == success)
    {
        Log_e("Unable to initialize.");
        std::exit(EXIT_FAILURE);
    }

    while (!glfwWindowShouldClose(window))
    {
        Display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    std::exit(EXIT_SUCCESS);
}

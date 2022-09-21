#define NUM_VAO 1
#define NUM_VBO 2

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "SOIL2/SOIL2.h"
#include "gl_tools.h"
#include "glm/glm.hpp"
#include "logging.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

GLuint rendering_program;
GLuint vao[NUM_VAO];
GLuint vbo[NUM_VBO];

void
Init(GLFWwindow* window)
{
    // [ cfarvin::TODO ] We are not using the window argument from the chapter yet.
    if (window)
    {
    }

    rendering_program = CreateShaderProgram("./chapter_4/src/shaders/vertex_shader.glsl",
                                            "./chapter_4/src/shaders/fragment_shader.glsl");

    // Note: OGL requires at least one VAO when shaders are being used.
    glGenVertexArrays(NUM_VAO, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(2, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
}

bool
Display(GLFWwindow* window, double current_time)
{
    // [ cfarvin::TODO ] We are not using the window or current_time arguments from the chapter yet.
    if (window || current_time)
    {
    }

    glPointSize(50.0f);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    const bool success = SendVertexData(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    if (true != success)
    {
        Log(LogType::ERROR, "Failed to send vertex data.");
        return false;
    }
    glEnableVertexAttribArray(0);

    GLint mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
    GLint proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
    glUniformMatrix4fv(mv_location, 1, GL_FALSE, glm::value_ptr(mv_matrix));
    glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glUseProgram(rendering_program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int
main()
{
    if (!glfwInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLFW.");
        std::exit(EXIT_FAILURE);
    }

    // [ cfarvin::TODO ] Store these and other settings in a configuration file that is parsed on startup?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    // Note: nullptr parameters allow for fullscreen mode and resource sharing.
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLEW.");
        std::exit(EXIT_FAILURE);
    }

    // Note: Enables V-Sync
    // Note: Glfw windows are double-buffered by default.
    glfwSwapInterval(1);

    Init(window);

    bool success = true;
    while (!glfwWindowShouldClose(window) && success)
    {
        success = Display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    std::exit(EXIT_SUCCESS);
}

#define GLEW_STATIC
#define NUM_VAO 1

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "SOIL2/SOIL2.h"
#include "glm/glm.hpp"

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

enum class LogType
{
    INFO = 0,
    WARNING,
    ERROR,
};

void
Log(const LogType lt, const char* const c_str)
{
    FILE*       output_dest = stdout;
    std::string output_tag  = "";

    switch (lt)
    {
        case LogType::INFO:
            output_tag = "[ info ]";
            break;
        case LogType::WARNING:
            output_tag = "[ warning ]";
            break;
        case LogType::ERROR:
            output_tag  = "[ error ]";
            output_dest = stderr;
            break;
    }

    std::fprintf(output_dest, "%s: %s", output_tag.c_str(), c_str);
    std::fflush(output_dest);
}

void
Log(const LogType lt, const std::stringstream& ss)
{
    Log(lt, ss.str().c_str());
}

void
Log(const LogType lt, const std::string& str)
{
    Log(lt, str.c_str());
}

bool
QueryGlErrors()
{
    bool found_error = false;
    int  gl_error    = glGetError();
    while (gl_error != GL_NO_ERROR)
    {
        gl_error = glGetError();
        Log(LogType::ERROR, std::stringstream(gl_error));
        found_error = true;
    }

    return found_error;
}

const std::string
ReadShaderSource(const char* const file_path)
{
    std::ifstream file_stream(file_path, std::ios::in);
    if (false == file_stream.good())
    {
        std::stringstream error_ss;
        error_ss << "Shader source file does not exist. Given:\n\t" << file_path;
        Log(LogType::ERROR, error_ss);
        file_stream.close();
    }

    std::stringstream ss("");
    ss << file_stream.rdbuf();
    const std::string file_contents = ss.str();

    if (1 >= file_contents.length())
    {
        std::stringstream error_ss;
        error_ss << "Shader source file was empty. Given:\n\t" << file_path;
        Log(LogType::ERROR, error_ss);
        file_stream.close();
    }

    file_stream.close();
    return file_contents;
}

GLuint
CreateShaderProgram()
{
    GLuint              vertex_shader        = glCreateShader(GL_VERTEX_SHADER);
    const std::string   vs_string            = ReadShaderSource("./src/shaders/vertex_shader.glsl");
    const GLchar* const vertex_shader_source = vs_string.c_str();
    glShaderSource(vertex_shader, 1, &(vertex_shader_source), nullptr);

    glCompileShader(vertex_shader);
    GLint vertex_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compiled);
    if (GL_TRUE != vertex_compiled)
    {
        GLchar error_message[1024];
        glGetShaderInfoLog(vertex_shader, 1024, nullptr, error_message);
        Log(LogType::ERROR, std::string(error_message));
        return 0;
    }

    GLuint            fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const std::string fs_string       = ReadShaderSource("./src/shaders/fragment_shader.glsl");
    const GLchar*     fragment_shader_source = fs_string.c_str();
    glShaderSource(fragment_shader, 1, &(fragment_shader_source), nullptr);

    glCompileShader(fragment_shader);
    GLint fragment_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compiled);
    if (fragment_compiled != GL_TRUE)
    {
        GLchar error_message[1024];
        glGetShaderInfoLog(vertex_shader, 1024, nullptr, error_message);
        Log(LogType::ERROR, std::string(error_message));
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int error_log_list_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &error_log_list_length);
    if (error_log_list_length > 0)
    {
        GLchar error_message[1024];
        glGetProgramInfoLog(program, 1024, nullptr, error_message);
        Log(LogType::ERROR, std::string(error_message));
        return 0;
    }

    if (true == QueryGlErrors())
    {
        return 0;
    }

    return program;
}

void
Init(GLFWwindow* window)
{
    // [ cfarvin::TODO ] We are not using the window argument from the chapter yet.
    if (window)
    {
    }

    rendering_program = CreateShaderProgram();

    // Note: OGL requires at least one VAO when shaders are being used.
    glGenVertexArrays(NUM_VAO, vao);
    glBindVertexArray(vao[0]);
}

void
Display(GLFWwindow* window, double current_time)
{
    // [ cfarvin::TODO ] We are not using the window or current_time arguments from the chapter yet.
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
    if (!glfwInit())
    {
        Log(LogType::ERROR, "Unable to initialize GLFW.");
        std::exit(EXIT_FAILURE);
    }

    // [ cfarvin::TODO ] Store these and other settings in a configuration file that is parsed on startup?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    // Note: nullptr parameters allow for fullscreen mode and resource sharing.
    GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 2 - program1", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        Log(LogType::ERROR, "Unable to initialize GLEW.");
        std::exit(EXIT_FAILURE);
    }

    // Note: Enables V-Sync
    // Note: Glfw windows are double-buffered by default.
    glfwSwapInterval(1);

    Init(window);

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

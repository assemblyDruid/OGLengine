#include <fstream>

#include "gl_tools.h"
#include "logging.h"

const bool
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

// Private to this file.
static inline const bool
ReadShaderSource(const char* const&& file_path,
                 std::string& return_shader_source)
{
    std::ifstream file_stream(file_path, std::ios::in);
    if (false == file_stream.good())
    {
        std::stringstream error_ss;
        error_ss << "Shader source file does not exist. Given:\n\t" << file_path;
        Log(LogType::ERROR, error_ss);
        file_stream.close();
        return false;
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
        return false;
    }

    file_stream.close();
    return_shader_source = file_contents;
    return true;
}

const GLuint
CreateShaderProgram(const char* const&& vertex_shader_file_path,
                    const char* const&& fragment_shader_file_path)
{
    GLuint              vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_source_buffer;
    ReadShaderSource(std::move(vertex_shader_file_path), vertex_shader_source_buffer);
    const GLchar* const vertex_shader_source = vertex_shader_source_buffer.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);

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
    std::string fragment_shader_source_buffer;
    ReadShaderSource(std::move(fragment_shader_file_path), fragment_shader_source_buffer);
    const GLchar* const fragment_shader_source = fragment_shader_source_buffer.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);

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

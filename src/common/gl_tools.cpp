#include "gl_tools.h"

#include "logging.h"

#include <fstream>

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
static const bool
ReadShaderSource(const char* const&& file_path, std::string& return_shader_source)
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

static void
LogGLCompilationError(const GLuint& compilation_step)
{
    constexpr unsigned int max_error_message_length                = 512;
    GLchar                 error_message[max_error_message_length] = {};

    if (GL_TRUE == glIsShader(compilation_step))
    {
        glGetShaderInfoLog(compilation_step,
                           max_error_message_length,
                           nullptr,
                           &(error_message[0]));
    }
    else if (GL_TRUE == glIsProgram(compilation_step))
    {
        glGetProgramInfoLog(compilation_step,
                            max_error_message_length,
                            nullptr,
                            &(error_message[0]));
    }
    else
    {
        Log(LogType::ERROR, "Invalid compilation step, cannot print error message.");
        return;
    }

    Log(LogType::ERROR, error_message);
}

const GLuint
CreateShaderProgram(const char* const&& vertex_shader_file_path,
                    const char* const&& fragment_shader_file_path)
{
    GLuint      vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_source_buffer;
    ReadShaderSource(std::move(vertex_shader_file_path), vertex_shader_source_buffer);
    const GLchar* const vertex_shader_source = vertex_shader_source_buffer.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);

    glCompileShader(vertex_shader);
    GLint vertex_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compiled);
    if (GL_TRUE != vertex_compiled)
    {
        LogGLCompilationError(vertex_shader);
        return 0;
    }

    GLuint      fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_source_buffer;
    ReadShaderSource(std::move(fragment_shader_file_path), fragment_shader_source_buffer);
    const GLchar* const fragment_shader_source = fragment_shader_source_buffer.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);

    glCompileShader(fragment_shader);
    GLint fragment_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compiled);
    if (fragment_compiled != GL_TRUE)
    {
        LogGLCompilationError(fragment_shader);
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
        LogGLCompilationError(program);
        return 0;
    }

    if (true == QueryGlErrors())
    {
        return 0;
    }

    return program;
}

#define GetUniformLocationImpl                                                                    \
    const GLint u_loc = glGetUniformLocation(shader_program, uniform_name);                       \
    if (-1 == u_loc)                                                                              \
    {                                                                                             \
        std::string error_message = std::string("No such uniform: ") + std::string(uniform_name); \
        Log(LogType::ERROR, error_message);                                                       \
    }                                                                                             \
    return u_loc

const GLint
GetUniformLocation(const GLuint& shader_program, const char* const&& uniform_name)
{
    GetUniformLocationImpl;
}

const GLint
GetUniformLocation(const GLuint& shader_program, const char* const& uniform_name)
{
    GetUniformLocationImpl;
}
#undef GetUniformLocationImpl

// [ cfarvin::TODO ] Templatize these SetUniformValue calls, remove suffixes
void
SetUniformValueMat4(const GLint& uniform_location, const GLfloat* value)
{
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, value);
}

void
SetUniformValueMat4(const GLuint&       shader_program,
                    const char* const&& uniform_name,
                    const GLfloat*      value)
{
    SetUniformValueMat4(GetUniformLocation(shader_program, uniform_name), value);
}

void
SetUniformValue1F(const GLint& uniform_location, const GLfloat& value)
{
    glUniform1f(uniform_location, value);
}

void
SetUniformValue1F(const GLuint&       shader_program,
                  const char* const&& uniform_name,
                  const GLfloat&      value)
{
    SetUniformValue1F(GetUniformLocation(shader_program, uniform_name), value);
}

#ifndef gl_tools_h
#define gl_tools_h

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>

const inline bool
QueryGlErrors();

const GLuint
CreateShaderProgram(const char* const&& vertex_shader_file_path,
                    const char* const&& fragment_shader_file_path);

extern const inline GLint
GetUniformLocation(const GLuint& shader_program, const char* const&& uniform_name);

extern const inline GLint
GetUniformLocation(const GLuint& shader_program, const char* const& uniform_name);

extern inline void
SetUniformValue(const GLint& uniform_location, const GLfloat* value);

extern inline void
SetUniformValue(const GLuint&       shader_program,
                const char* const&& uniform_name,
                const GLfloat*      value);

#endif

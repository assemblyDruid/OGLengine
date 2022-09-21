#ifndef gl_tools_h
#define gl_tools_h

#include <string>

#include "GL/glew.h"
#include "GLFW/glfw3.h"

const bool
QueryGlErrors();

const GLuint
CreateShaderProgram(const char* const&& vertex_shader_file_path,
                    const char* const&& fragment_shader_file_path);

#endif

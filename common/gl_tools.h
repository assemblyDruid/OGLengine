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

extern inline const bool
SendVertexData(const GLuint&& vertex_shader_attrib_location,
               const GLint&& vertex_shader_attrib_num_elements,
               const GLenum&& data_type,
               const GLboolean&& vertex_shadeer_attrib_should_normalize,
               const GLsizei&& stride,
               const GLvoid*&& data);

#endif

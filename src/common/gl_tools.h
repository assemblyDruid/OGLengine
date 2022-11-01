#ifndef gl_tools_h
#define gl_tools_h

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>

struct VertexAttribute
{
    GLuint    index;      // Index of generic attribute to be modified.
    GLint     size;       // Number of components per attribute [1, 4].
    GLenum    type;       // Data type of elements.
    GLboolean normalized; // Should be normalized.
    GLsizei   stride;     // Byte offset between elements within array.
    GLvoid*   pointer;    // Offset of 1st component in array.
};

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
SetUniformValueMat4(const GLint& uniform_location, const GLfloat* value);

extern inline void
SetUniformValueMat4(const GLuint&       shader_program,
                    const char* const&& uniform_name,
                    const GLfloat*      value);

extern inline void
SetUniformValue1F(const GLint& uniform_location, const GLfloat& value);

extern inline void
SetUniformValue1F(const GLuint&       shader_program,
                  const char* const&& uniform_name,
                  const GLfloat&      value);

#endif

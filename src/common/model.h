#ifndef model_h
#define model_h

#include "GL/glew.h"
#include "gl_tools.h"
#include "glm/glm.hpp"
#include "object3.h"

struct Model : protected Object3
{
    using Object3::GetOrientation;
    using Object3::GetPosition;
    using Object3::SetOrientation;
    using Object3::SetPosition;

  protected:
    glm::mat4 model_matrix;
};

struct BufferedModel : Model
{
    void
    ArrayBuffer(GLuint     _vao,
                GLvoid*    _data,
                GLsizeiptr _size,
                GLsizei    _vertex_count,
                GLenum     _usage,
                GLenum     _drawing_mode);

    // clang-format off
    void
    FormatVertexAttribute(GLuint    _index,      // Index of generic attribute to be modified.
                          GLint     _size,       // Number of components per attribute [1, 4].
                          GLenum    _type,       // Data type of elements.
                          GLboolean _normalized, // Should be normalized.
                          GLsizei   _stride,     // Byte offset between elements within array.
                          GLvoid*   _pointer);   // Offset of 1st component in array.
    // clang-format on

    void
    Draw();

  protected:
    GLuint  vbo;
    GLsizei vertex_count;
    GLvoid* vertex_data;
    GLenum  drawing_mode;

  private:
    VertexAttribute vertex_attribute;
};

#endif
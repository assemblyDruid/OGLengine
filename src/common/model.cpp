#include "model.h"

#include "GL/glew.h"
#include "assert.h"

void
BufferedModel::ArrayBuffer(GLuint     _vao,
                           GLvoid*    _data,
                           GLsizeiptr _size,
                           GLsizei    _vertex_count,
                           GLenum     _usage,
                           GLenum     _drawing_mode)
{
    assert(nullptr != _data);
    assert(0 != _size);
    assert(0 != _vertex_count);

    if (GL_FALSE == glIsBuffer(vbo))
    {
        glGenBuffers(1, &vbo);
    }

    vertex_count = _vertex_count;
    drawing_mode = _drawing_mode;

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _size, _data, _usage);
}

void
BufferedModel::FormatVertexAttribute(GLuint    _index,
                                     GLint     _size,
                                     GLenum    _type,
                                     GLboolean _normalized,
                                     GLsizei   _stride,
                                     GLvoid*   _pointer)
{
    vertex_attribute.index      = _index;      // Index of generic attribute to be modified.
    vertex_attribute.size       = _size;       // Number of components per attribute [1, 4].
    vertex_attribute.type       = _type;       // Data type of elements.
    vertex_attribute.normalized = _normalized; // Should be normalized.
    vertex_attribute.stride     = _stride;     // Byte offset between elements within array.
    vertex_attribute.pointer    = _pointer;    // Offset of 1st component in array.
}

void
BufferedModel::Draw()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(vertex_attribute.index,
                          vertex_attribute.size,
                          vertex_attribute.type,
                          vertex_attribute.normalized,
                          vertex_attribute.stride,
                          vertex_attribute.pointer);
    glEnableVertexAttribArray(0);
    glDrawArrays(drawing_mode, 0, vertex_count);
}
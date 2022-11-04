#ifndef gl_tools_h
#define gl_tools_h

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>
#include <unordered_map>
#include <vector>

struct VertexAttribute
{
    GLuint    index;      // Index of generic attribute to be modified.
    GLint     size;       // Number of components per attribute [1, 4].
    GLenum    type;       // Data type of elements.
    GLboolean normalized; // Should be normalized.
    GLsizei   stride;     // Byte offset between elements within array.
    GLvoid*   pointer;    // Offset of 1st component in array.
};

enum class GLBufferType
{
    POSITION_COORDINATES,
    TEXTURE_COORDINATES
};

struct GLBuffer
{
    GLBuffer(const GLuint                        _id,
             const GLsizei                       _vertex_count,
             const GLenum                        _drawing_mode,
             const std::vector<VertexAttribute>& _vertex_attributes,
             const GLBufferType                  _buffer_type);

    const GLuint                                id;
    GLBufferType                                type;
    std::unordered_map<GLuint, VertexAttribute> vertex_attributes;
    GLsizei                                     vertex_count;
    GLenum                                      drawing_mode;

  private:
    friend class BufferStore;
    GLBuffer();
};

struct GLBufferStore
{
    GLuint
    AddBuffer(bool&                               _success,
              const GLuint                        _vao,
              const GLvoid* const                 _data,
              const GLsizeiptr                    _size,
              const GLsizei                       _vertex_count,
              const GLenum                        _usage,
              const GLenum                        _drawing_mode,
              const GLBufferType                  _buffer_type,
              const std::vector<VertexAttribute>& _vertex_attribute);

    void
    DeleteAllBuffers();

    void const
    GetBufferByBufferId(const GLuint& _buffer_id, GLBuffer*& _buffer);

    void const
    GetBufferByAttributeId(const GLuint& _attribute_id, GLBuffer*& _buffer);

  protected:
    void
    ModifyBuffer(bool&               _success,
                 const GLuint&       _buffer_id,
                 const GLuint        _vao,
                 const GLvoid* const _data,
                 const GLsizeiptr    _size,
                 const GLsizei       _vertex_count,
                 const GLenum        _usage,
                 const GLenum        _drawing_mode);
    void
    ModifyBufferFormat(bool&                  _success,
                       const GLuint&          _attribute_id,
                       const VertexAttribute& _vertex_attribute);

  private:
    std::vector<GLBuffer>                 buffer_store;
    std::unordered_map<GLuint, GLBuffer*> buffers_by_buffer_id;
    std::unordered_map<GLuint, GLBuffer*> buffers_by_attribute_id;
};

inline void
QueryGlErrors(bool& _success);

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

GLuint
GetTexture2DFromImage(bool&             _success,
                      const char* const _file_path,
                      const bool        _attempt_ansiotropic_filtering);

GLuint
GetTestTextureRGB(bool&               _success,
                  const char* const   _output_file, // Provide nullptr to skip writing to disk.
                  const unsigned int& _image_width  = 600,
                  const unsigned int& _image_height = 600);

#endif

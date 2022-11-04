#ifndef model_h
#define model_h

#include "GL/glew.h"
#include "gl_tools.h"
#include "glm/glm.hpp"
#include "object3.h"

#include <unordered_set>

struct Model : protected Object3
{
    using Object3::GetOrientation;
    using Object3::GetPosition;
    using Object3::SetOrientation;
    using Object3::SetPosition;

  protected:
    glm::mat4 model_matrix;
};

struct EnabledAttributes
{
    GLBuffer*                  buffer;
    std::unordered_set<GLuint> vertex_attribute_ids;
};

struct BufferedModel : Model
{
    GLuint
    AddBuffer(bool&                               _success,
              const GLuint                        _vao,
              const GLvoid* const                 _data,
              const GLsizeiptr                    _size,
              const GLsizei                       _vertex_count,
              const GLenum                        _usage,
              const GLenum                        _drawing_mode,
              const GLBufferType                  _buffer_tye,
              const std::vector<VertexAttribute>& _vertex_attribute);

    void
    EnableVertexAttribute(bool& _success, const GLuint& _attribute_id);

    void
    DisableVertexAttribute(bool& _success, const GLuint& _attribute_id);

    void const
    Draw();

  protected:
    GLBufferStore                                                 buffer_store;
    std::unordered_map</*GlBuffer ID*/ GLuint, EnabledAttributes> enabled_attributes;
};

struct TexturedModel : BufferedModel
{
    void
    CreateTexture(bool& _success, const char* const _file_path);

    void
    CreateTestTexture(bool&             _success,
                      const char* const _file_path); // Nullptr to skip write to disk.

    // [ cfarvin::TODO ] should not be public
    GLuint texture_vbo;
    GLuint texture_id;

  protected:
};

#endif
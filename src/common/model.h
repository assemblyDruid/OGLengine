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

struct EnabledAttributes
{
    glt::GLBuffer*                     buffer = nullptr;
    std::vector<glt::VertexAttribute*> vertex_attributes;
};

struct BufferedModel : Model
{
    GLuint
    AddBuffer(bool&                                    _success,
              const GLuint&&                           _vao,
              const GLvoid* const                      _data,
              const GLsizeiptr&&                       _size,
              const GLsizei&&                          _vertex_count,
              const GLenum&&                           _usage,
              const GLenum&&                           _drawing_mode,
              const glt::GLBufferType&&                _buffer_tye,
              const std::vector<glt::VertexAttribute>& _vertex_attribute);

    void
    EnableVertexAttribute(bool&                     _success,
                          const glt::GLBufferType&& _buffer_type,
                          const GLuint&             _attribute_id);

    void const
    Draw();

  protected:
    glt::GLBufferStore             buffer_store;
    std::vector<EnabledAttributes> enabled_attributes;
};

struct TextureInfo
{
    TextureInfo() = delete;
    TextureInfo(const GLuint&& _texture_id,
                const GLuint&& _texture_coordinates_vbo_id,
                const GLenum&& _texture_target);

    const GLuint texture_id;
    const GLuint texture_coordinates_vbo_id;
    const GLenum texture_target;
};

struct TexturedModel : BufferedModel
{
    void
    CreateTexture(bool&                                    _success,
                  const char* const                        _file_path,
                  GLuint&                                  _texture_id,
                  const GLuint&&                           _vao,
                  const GLvoid*                            _texcoord_data,
                  const GLsizeiptr&&                       _texcoord_data_size,
                  const GLsizei&&                          _texcoord_vertex_count,
                  const GLenum&&                           _texcoord_usage,
                  const GLenum&&                           _texcoord_drawing_mode,
                  const GLenum&&                           _texture_traget,
                  const std::vector<glt::VertexAttribute>& _texture_coordinate_vertex_attributes);

    void
    SetTextureUnit(const GLenum&& _texture_unit);

    void
    EnableVertexAttribute(bool&                     _success,
                          const glt::GLBufferType&& _buffer_type,
                          const GLuint&&            _attribute_id);

    void const
    GetTextureInfoByAttributeId(bool&          _success,
                                const GLuint&& _attribute_id,
                                TextureInfo*&  _texture_info);

    void const
    Draw();

  protected:
    GLenum                    texture_unit;
    std::vector<TextureInfo>  textures;
    std::vector<TextureInfo*> enabled_textures_by_attribute_index;

  private:
    void
    ResizeEnabledTexturesVector(const std::vector<glt::VertexAttribute>& _vertex_attributes);

    void
    ResizeEnabledTexturesVector(const GLuint&& _vertex_attribute_id);
};

#endif
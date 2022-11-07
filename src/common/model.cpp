#include "model.h"

#include "GL/glew.h"
#include "assert.h"
#include "logging.h"

#include <sstream>

GLuint
BufferedModel::AddBuffer(bool&                                    _success,
                         const GLuint&&                           _vao,
                         const GLvoid* const                      _data,
                         const GLsizeiptr&&                       _size,
                         const GLsizei&&                          _vertex_count,
                         const GLenum&&                           _usage,
                         const GLenum&&                           _drawing_mode,
                         const glt::GLBufferType&&                _buffer_type,
                         const std::vector<glt::VertexAttribute>& _vertex_attribute)
{
    return buffer_store.AddBuffer(_success,
                                  std::move(_vao),
                                  _data,
                                  std::move(_size),
                                  std::move(_vertex_count),
                                  std::move(_usage),
                                  std::move(_drawing_mode),
                                  std::move(_buffer_type),
                                  _vertex_attribute);
}

void
BufferedModel::EnableVertexAttribute(bool&                     _success,
                                     const glt::GLBufferType&& _buffer_type,
                                     const GLuint&             _attribute_id)
{
    _success = false;

    glt::GLBuffer* buffer;
    buffer_store.GetBufferByAttributeId(std::move(_attribute_id), std::move(_buffer_type), buffer);
    if (nullptr == buffer) // Indicates fail condition for retrieval by attribute id.
    {
        std::stringstream ss;
        ss << "Unable to enable vertex attribute with index: " << _attribute_id
           << ". No such buffer in buffer store.";
        Log_e(ss);
        return;
    }

    // Note: For small numbers of attributes, this is still more efficient than a
    //       hash-based data structure.
    size_t enabled_attribute_index   = 0;
    bool   attribute_already_enabled = false;
    bool   buffer_already_added      = false;
    for (auto& enabled_attribute : enabled_attributes)
    {
        if (buffer->buffer_id == enabled_attribute.buffer->buffer_id)
        {
            for (auto& attribute : enabled_attribute.vertex_attributes)
            {
                if (_attribute_id == attribute->index)
                {
                    attribute_already_enabled = true;
                    break;
                }
            }

            buffer_already_added = true;
            break;
        }

        enabled_attribute_index++;
    }

    // Note: We have already confirmed that this _attribute_id is within this buffer.
    glt::VertexAttribute* vertex_attribute = nullptr;
    for (auto& attribute : buffer->vertex_attributes)
    {
        if (_attribute_id == attribute.index)
        {
            vertex_attribute = &attribute;
            break;
        }
    }
    assert(nullptr != vertex_attribute);

    if (false == attribute_already_enabled)
    {
        if (false == buffer_already_added)
        {
            EnabledAttributes newly_enabled_attributes = {};
            newly_enabled_attributes.buffer            = buffer;
            newly_enabled_attributes.vertex_attributes.push_back(vertex_attribute);

            enabled_attributes.push_back(newly_enabled_attributes);
        }
        else
        {
            enabled_attributes[enabled_attribute_index].vertex_attributes.push_back(
              vertex_attribute);
        }
    }
    else
    {
        std::stringstream ss;
        ss << "The attribute with id: " << _attribute_id << " was already enabled.";
        Log_w(ss);
    }

    _success = true;
}

void const
BufferedModel::Draw()
{
    glt::GLBuffer* buffer;
    for (auto& Q : enabled_attributes)
    {
        buffer = Q.buffer;
        glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
        for (auto& attribute : Q.vertex_attributes)
        {
            glVertexAttribPointer(attribute->index,
                                  attribute->size,
                                  attribute->type,
                                  attribute->normalized,
                                  attribute->stride,
                                  attribute->pointer);
            glEnableVertexAttribArray(attribute->index);

            glDrawArrays(buffer->drawing_mode, 0, buffer->vertex_count);
        }
    }
}

void
TexturedModel::ResizeEnabledTexturesVector(const GLuint&& _vertex_attribute_id)
{
    // Ensure that there is enough allocated space in the enabled buffer to store
    // the TextureInfo at the index associated with any known attribute id.
    if (enabled_textures_by_attribute_index.size() < _vertex_attribute_id)
    {
        enabled_textures_by_attribute_index.resize((_vertex_attribute_id + 1), nullptr);
    }

#define WARN_ON_ATTRIBUTE_ID_VALUE 15
    if ((_vertex_attribute_id > WARN_ON_ATTRIBUTE_ID_VALUE) ||
        (enabled_textures_by_attribute_index.size() > WARN_ON_ATTRIBUTE_ID_VALUE))
    {
        std::stringstream ss;
        ss << "The current implementation was intended for small numbers of textures and vertex "
              "attributes per model. The current model exceeds"
           << WARN_ON_ATTRIBUTE_ID_VALUE << " elements in size.";
        Log_w(ss);
    }
#undef WARN_ON_ATTRIBUTE_ID_VALUE
}

void
TexturedModel::ResizeEnabledTexturesVector(
  const std::vector<glt::VertexAttribute>& _vertex_attributes)
{
    GLuint max_attribute_id = 0;
    for (auto& attribute : _vertex_attributes)
    {
        if (attribute.index > max_attribute_id) max_attribute_id = attribute.index;
    }

    ResizeEnabledTexturesVector(std::move(max_attribute_id));
}

TextureInfo::TextureInfo(const GLuint&& _texture_id,
                         const GLuint&& _texture_coordinates_vbo_id,
                         const GLenum&& _texture_target)
  : texture_id(_texture_id)
  , texture_coordinates_vbo_id(_texture_coordinates_vbo_id)
  , texture_target(_texture_target)
{
}

void
TexturedModel::CreateTexture(
  bool&                                    _success,
  const char* const                        _file_path,
  GLuint&                                  _texture_id,
  const GLuint&&                           _vao,
  const GLvoid*                            _texcoord_data,
  const GLsizeiptr&&                       _texcoord_data_size,
  const GLsizei&&                          _texcoord_vertex_count,
  const GLenum&&                           _texcoord_usage,
  const GLenum&&                           _texcoord_drawing_mode,
  const GLenum&&                           _texture_target,
  const std::vector<glt::VertexAttribute>& _texture_coordinate_vertex_attributes)
{
    _success = false;

    // Note: glt::GetTexture2DFromImage handles nullptr file path.
    _texture_id = glt::GetTexture2DFromImage(_success, std::move(_file_path), true);
    if (false == _success)
    {
        std::stringstream ss;
        ss << "Unable to load texture at: " << _file_path;
        Log_e(ss);
        return;
    }

    bool is_unique = true;
    for (size_t index = 0; index < textures.size(); index++)
    {
        if (textures[index].texture_id == _texture_id) is_unique = false;
    }

    if (false == is_unique)
    {
        Log_e("The provided texture information is not unique; already loaded.");
        glDeleteTextures(1, &_texture_id);
        return;
    }

    const GLuint vbo_id = buffer_store.AddBuffer(_success,
                                                 std::move(_vao),
                                                 _texcoord_data,
                                                 std::move(_texcoord_data_size),
                                                 std::move(_texcoord_vertex_count),
                                                 std::move(_texcoord_usage),
                                                 std::move(_texcoord_drawing_mode),
                                                 glt::GLBufferType::TEXTURE_COORDINATES,
                                                 _texture_coordinate_vertex_attributes);
    if (false == _success)
    {
        Log_e("Unable to create texture coordinate buffer.");
        glDeleteTextures(1, &_texture_id);
        return;
    }

    ResizeEnabledTexturesVector(_texture_coordinate_vertex_attributes);

    textures.push_back(
      TextureInfo(std::move(_texture_id), std::move(vbo_id), std::move(_texture_target)));
    _success = true;
}

void
TexturedModel::SetTextureUnit(const GLenum&& _texture_unit)
{
    texture_unit = _texture_unit;
}

void const
TexturedModel::Draw()
{
    glt::GLBuffer* buffer;
    TextureInfo*   texture;

    for (auto& Q : enabled_attributes)
    {
        buffer = Q.buffer;
        glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
        for (auto& attribute : Q.vertex_attributes)
        {
            glVertexAttribPointer(attribute->index,
                                  attribute->size,
                                  attribute->type,
                                  attribute->normalized,
                                  attribute->stride,
                                  attribute->pointer);
            glEnableVertexAttribArray(attribute->index);

            if (glt::GLBufferType::TEXTURE_COORDINATES == buffer->buffer_type)
            {
                texture = enabled_textures_by_attribute_index[attribute->index];
                if (nullptr != texture)
                {
                    glActiveTexture(texture_unit);
                    glBindTexture(texture->texture_target, texture->texture_id);
                }
                else
                {
                    Log_e("A texture was enabled but could not be found.");
                }
            }

            glDrawArrays(buffer->drawing_mode, 0, buffer->vertex_count);
        }
    }
}

void const
TexturedModel::GetTextureInfoByAttributeId(bool&          _success,
                                           const GLuint&& _attribute_id,
                                           TextureInfo*&  _texture_info)
{
    _success      = false;
    _texture_info = nullptr;

    glt::GLBuffer* buffer = nullptr;
    buffer_store.GetBufferByAttributeId(std::move(_attribute_id),
                                        glt::GLBufferType::TEXTURE_COORDINATES,
                                        buffer);
    if (nullptr == buffer)
    {
        std::stringstream ss;
        ss << "Unalbe to obtain buffer with attribute id: " << _attribute_id
           << ". from buffer store.";
        Log_e(ss);
        return;
    }

    for (auto& texture : textures)
    {
        if (buffer->buffer_id == texture.texture_coordinates_vbo_id)
        {
            _texture_info = &texture;
            break;
        }
    }

    if (nullptr == _texture_info)
    {
        std::stringstream ss;
        ss << "Unalbe to obtain buffer with attribute id: " << _attribute_id
           << ". from model textures.";
        Log_e(ss);
        return;
    }

    _success = true;
}

void
TexturedModel::EnableVertexAttribute(bool&                     _success,
                                     const glt::GLBufferType&& _buffer_type,
                                     const GLuint&&            _attribute_id)
{
    if (glt::GLBufferType::TEXTURE_COORDINATES == _buffer_type)
    {
        ResizeEnabledTexturesVector(std::move(_attribute_id));

        TextureInfo* texture_info;
        GetTextureInfoByAttributeId(_success, std::move(_attribute_id), texture_info);
        if (nullptr == texture_info)
        {
            Log_e("Unable to enable texture via vertex attribute.");
            return;
        }

        enabled_textures_by_attribute_index[_attribute_id] = texture_info;
    }

    BufferedModel::EnableVertexAttribute(_success, std::move(_buffer_type), _attribute_id);
}
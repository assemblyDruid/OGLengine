#include "model.h"

#include "GL/glew.h"
#include "assert.h"
#include "logging.h"

#include <sstream>

GLuint
BufferedModel::AddBuffer(bool&                               _success,
                         const GLuint                        _vao,
                         const GLvoid* const                 _data,
                         const GLsizeiptr                    _size,
                         const GLsizei                       _vertex_count,
                         const GLenum                        _usage,
                         const GLenum                        _drawing_mode,
                         const GLBufferType                  _buffer_type,
                         const std::vector<VertexAttribute>& _vertex_attribute)
{
    return buffer_store.AddBuffer(_success,
                                  _vao,
                                  _data,
                                  _size,
                                  _vertex_count,
                                  _usage,
                                  _drawing_mode,
                                  _buffer_type,
                                  _vertex_attribute);
}

void
BufferedModel::EnableVertexAttribute(bool& _success, const GLuint& _attribute_id)
{
    _success = false;

    GLBuffer* buffer;
    buffer_store.GetBufferByAttributeId(_attribute_id, buffer);

    if (nullptr == buffer) // Indicates fail condition for retrieval by attribute id.
    {
        Log_e("Unable to enable vertex attribute.");
        return;
    }

    EnabledAttributes* attribs = &(enabled_attributes[buffer->id]);
    attribs->buffer            = buffer;
    attribs->vertex_attribute_ids.insert(_attribute_id);

    _success = true;
}

void
BufferedModel::DisableVertexAttribute(bool& _success, const GLuint& _attribute_id)
{
    _success = false;

    GLBuffer* buffer;
    buffer_store.GetBufferByAttributeId(_attribute_id, buffer);

    if (nullptr == buffer) // Indicates fail condition for retrieval by attribute id.
    {
        Log_e("Unable to disable vertex attribute.");
        return;
    }

    EnabledAttributes*          attribs              = &(enabled_attributes[buffer->id]);
    std::unordered_set<GLuint>* vertex_attribute_ids = &(attribs->vertex_attribute_ids);
    vertex_attribute_ids->erase(_attribute_id);
    if (0 == vertex_attribute_ids->size())
    {
        enabled_attributes.erase(buffer->id);
    }

    _success = true;
}

void const
BufferedModel::Draw()
{
    GLBuffer*        buffer;
    VertexAttribute* attributes;

    for (auto& Q : enabled_attributes)
    {
        buffer = Q.second.buffer;
        glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
        for (auto& attribute_id : Q.second.vertex_attribute_ids)
        {
            attributes = &(buffer->vertex_attributes[attribute_id]);

            glVertexAttribPointer(attributes->index,
                                  attributes->size,
                                  attributes->type,
                                  attributes->normalized,
                                  attributes->stride,
                                  attributes->pointer);
            glEnableVertexAttribArray(attributes->index);
            glDrawArrays(buffer->drawing_mode, 0, buffer->vertex_count);
        }
    }
}

void
TexturedModel::CreateTexture(bool& _success, const char* const _file_path)
{
    _success = false;

    // Note: GetTexture2DFromImage handles nullptr file path.
    texture_id = GetTexture2DFromImage(_success, _file_path, true);
    if (false == _success)
    {
        std::stringstream ss;

        ss << "Unable to load texture at: " << _file_path;
        Log_e(ss);
        return;
    }
}

void
TexturedModel::CreateTestTexture(bool&             _success,
                                 const char* const _file_path) // Nullptr to skip write  to disk.
{
    _success = false;

    // Note: GetTexture2DFromImage handles nullptr file path.
    texture_id = GetTestTextureRGB(_success, _file_path, true);
    if (false == _success)
    {
        Log_e("Unable to load test texture");
        return;
    }
}
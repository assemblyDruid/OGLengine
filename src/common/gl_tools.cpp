#include "gl_tools.h"

#include "assert.h"
#include "image_tools.h"
#include "logging.h"
#include "stb/stb_image_write.h"

#include <fstream>

glt::GLBuffer::GLBuffer(const GLuint&&                      _buffer_id,
                        const GLsizei&&                     _vertex_count,
                        const GLenum&&                      _drawing_mode,
                        const GLBufferType&&                _buffer_type,
                        const std::vector<VertexAttribute>& _vertex_attributes)
  : buffer_id(_buffer_id)
  , vertex_count(_vertex_count)
  , drawing_mode(_drawing_mode)
  , buffer_type(_buffer_type)
  , vertex_attributes(_vertex_attributes)
{
    assert(0 != vertex_count);
    assert(GLBufferType::COUNT != buffer_type);
};

glt::GLBuffer::GLBuffer(const GLBuffer& _)
  : buffer_id(_.buffer_id)
  , vertex_count(_.vertex_count)
  , drawing_mode(_.drawing_mode)
  , buffer_type(_.buffer_type)
  , vertex_attributes(_.vertex_attributes)
{
    assert(0 != vertex_count);
    assert(GLBufferType::COUNT != buffer_type);
}

glt::GLBufferStore::~GLBufferStore() { DeleteAllBuffers(); }

const GLuint
glt::GLBufferStore::AddBuffer(bool&                               _success,
                              const GLuint&&                      _vao,
                              const GLvoid* const                 _data,
                              const GLsizeiptr&&                  _size,
                              const GLsizei&&                     _vertex_count,
                              const GLenum&&                      _usage,
                              const GLenum&&                      _drawing_mode,
                              const GLBufferType&&                _buffer_type,
                              const std::vector<VertexAttribute>& _vertex_attributes)
{
    assert(nullptr != _data);
    assert(0 != _size);
    assert(0 != _vertex_count);

    _success = false;

    GLuint buffer_id;
    glBindVertexArray(_vao);
    glGenBuffers(1, &buffer_id);

    buffer_store[static_cast<size_t>(_buffer_type)] = new GLBuffer(std::move(buffer_id),
                                                                   std::move(_vertex_count),
                                                                   std::move(_drawing_mode),
                                                                   std::move(_buffer_type),
                                                                   std::move(_vertex_attributes));

    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, _size, _data, _usage);

    _success = true;
    return buffer_id;
}

void
glt::GLBufferStore::DeleteAllBuffers()
{
    for (auto& buffer : buffer_store)
    {
        if (nullptr != buffer) delete buffer;
    }
}

void const
glt::GLBufferStore::GetBufferByBufferId(const GLuint&& _buffer_id, GLBuffer*& _buffer)
{
    _buffer = nullptr;
    for (size_t index = 0; index < buffer_store_elements; index++)
    {
        if ((nullptr != buffer_store[index]) && (_buffer_id == buffer_store[index]->buffer_id))
        {
            _buffer = buffer_store[index];
            return;
        }
    }
}

void const
glt::GLBufferStore::GetBufferByBufferId(const GLuint&&       _buffer_id,
                                        const GLBufferType&& _buffer_type,
                                        GLBuffer*&           _buffer)
{
    _buffer          = nullptr;
    GLBuffer* buffer = buffer_store[static_cast<size_t>(_buffer_type)];
    if ((nullptr != buffer) && (_buffer_id == buffer->buffer_id)) _buffer = buffer;
}

void const
glt::GLBufferStore::GetBufferByAttributeId(const GLuint&& _attribute_id, GLBuffer*& _buffer)
{
    _buffer = nullptr;
    for (size_t index = 0; index < buffer_store_elements; index++)
    {
        if (nullptr != buffer_store[index])
        {
            for (size_t a_index = 0; a_index < buffer_store[index]->vertex_attributes.size();
                 a_index++)
            {
                if (_attribute_id == buffer_store[index]->vertex_attributes[a_index].index)
                {
                    _buffer = buffer_store[index];
                    return;
                }
            }
        }
    }
}

void const
glt::GLBufferStore::GetBufferByAttributeId(const GLuint&&       _attribute_id,
                                           const GLBufferType&& _buffer_type,
                                           GLBuffer*&           _buffer)
{
    _buffer          = nullptr;
    GLBuffer* buffer = buffer_store[static_cast<size_t>(_buffer_type)];
    if (nullptr != buffer)
    {
        for (size_t index = 0; index < buffer->vertex_attributes.size(); index++)
        {
            if (_attribute_id == buffer->vertex_attributes[index].index)
            {
                _buffer = buffer;
                return;
            }
        }
    }
}

void
glt::GLBufferStore::ModifyBuffer(bool&               _success,
                                 const GLuint&       _buffer_id,
                                 const GLuint        _vao,
                                 const GLvoid* const _data,
                                 const GLsizeiptr    _size,
                                 const GLsizei       _vertex_count,
                                 const GLenum        _usage,
                                 const GLenum        _drawing_mode)
{
    _success = false;

    GLBuffer* buffer = nullptr;
    GetBufferByBufferId(std::move(_buffer_id), buffer);
    if (nullptr != buffer)
    {
        std::stringstream ss;
        ss << "Unable to modify buffer with id " << _buffer_id
           << ". No such buffer in buffer store";
        Log_e(ss);
        _success = false;
        return;
    }

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer_id);
    glBufferData(GL_ARRAY_BUFFER, _size, _data, _usage);

    buffer->vertex_count = _vertex_count;
    buffer->drawing_mode = _drawing_mode;

    _success = true;
}

void
glt::GLBufferStore::ModifyBufferFormat(bool&                   _success,
                                       const GLuint&&          _attribute_id,
                                       const VertexAttribute&& _vertex_attribute)
{
    _success = false;

    for (auto& buffer : buffer_store)
    {
        if (nullptr != buffer)
        {
            for (auto& attribute : buffer->vertex_attributes)
            {
                if (_attribute_id == attribute.index)
                {
                    attribute = _vertex_attribute;
                    break;
                }
            }
        }
    }

    _success = true;
}

void
glt::QueryGLErrors(bool& _success)
{
    int gl_error = glGetError();
    while (gl_error != GL_NO_ERROR)
    {
        gl_error = glGetError();
        std::stringstream ss;
        ss << gl_error;
        Log_e(ss);
        _success = false;
    }

    _success = true;
}

static const bool
ReadShaderSource(const char* const&& file_path, std::string& return_shader_source)
{
    std::ifstream file_stream(file_path, std::ios::in);
    if (false == file_stream.good())
    {
        std::stringstream error_ss;
        error_ss << "Shader source file does not exist. Given:\n\t" << file_path;
        Log_e(error_ss);
        file_stream.close();
        return false;
    }

    std::stringstream ss("");
    ss << file_stream.rdbuf();
    const std::string file_contents = ss.str();

    if (1 >= file_contents.length())
    {
        std::stringstream error_ss;
        error_ss << "Shader source file was empty. Given:\n\t" << file_path;
        Log_e(error_ss);
        file_stream.close();
        return false;
    }

    file_stream.close();
    return_shader_source = file_contents;
    return true;
}

static void
LogGLCompilationError(const GLuint& compilation_step)
{
    constexpr unsigned int max_error_message_length                = 512;
    GLchar                 error_message[max_error_message_length] = {};

    if (GL_TRUE == glIsShader(compilation_step))
    {
        glGetShaderInfoLog(compilation_step,
                           max_error_message_length,
                           nullptr,
                           &(error_message[0]));
    }
    else if (GL_TRUE == glIsProgram(compilation_step))
    {
        glGetProgramInfoLog(compilation_step,
                            max_error_message_length,
                            nullptr,
                            &(error_message[0]));
    }
    else
    {
        Log_e("Invalid compilation step, cannot print error message.");
        return;
    }

    Log_e(error_message);
}

const GLuint
glt::CreateShaderProgram(const char* const&& vertex_shader_file_path,
                         const char* const&& fragment_shader_file_path)
{
    GLuint      vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_source_buffer;
    ReadShaderSource(std::move(vertex_shader_file_path), vertex_shader_source_buffer);
    const GLchar* const vertex_shader_source = vertex_shader_source_buffer.c_str();
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);

    glCompileShader(vertex_shader);
    GLint vertex_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compiled);
    if (GL_TRUE != vertex_compiled)
    {
        LogGLCompilationError(vertex_shader);
        return 0;
    }

    GLuint      fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_source_buffer;
    ReadShaderSource(std::move(fragment_shader_file_path), fragment_shader_source_buffer);
    const GLchar* const fragment_shader_source = fragment_shader_source_buffer.c_str();
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);

    glCompileShader(fragment_shader);
    GLint fragment_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compiled);
    if (fragment_compiled != GL_TRUE)
    {
        LogGLCompilationError(fragment_shader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int error_log_list_length = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &error_log_list_length);
    if (error_log_list_length > 0)
    {
        LogGLCompilationError(program);
        return 0;
    }

    bool success;
    glt::QueryGLErrors(success);
    if (false == success)
    {
        return 0;
    }

    return program;
}

const GLint
glt::GetUniformLocation(const GLuint&& shader_program, const char* const&& uniform_name)
{
    const GLint u_loc = glGetUniformLocation(shader_program, uniform_name);
    if (-1 == u_loc)
    {
        std::string error_message = std::string("No such uniform: ") + std::string(uniform_name);
        Log_e(error_message);
    }
    return u_loc;
}

// [ cfarvin::TODO ] Templatize these SetUniformValue calls, remove suffixes
void
glt::SetUniformValueMat4(const GLint&& uniform_location, const GLfloat* value)
{
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, std::move(value));
}

void
glt::SetUniformValueMat4(const GLuint&&      shader_program,
                         const char* const&& uniform_name,
                         const GLfloat*      value)
{
    SetUniformValueMat4(GetUniformLocation(std::move(shader_program), std::move(uniform_name)),
                        std::move(value));
}

void
glt::SetUniformValue1F(const GLint&& uniform_location, const GLfloat&& value)
{
    glUniform1f(uniform_location, value);
}

void
glt::SetUniformValue1F(const GLuint&&      shader_program,
                       const char* const&& uniform_name,
                       const GLfloat&&     value)
{
    SetUniformValue1F(GetUniformLocation(std::move(shader_program), std::move(uniform_name)),
                      std::move(value));
}

GLuint
glt::GetTexture2DFromImage(bool&               _success,
                           const char* const&& _file_path,
                           const bool&&        _attempt_ansiotropic_filtering)
{
    _success = true;

    unsigned char* image_data;
    int            image_width;
    int            image_height;
    int            image_channels;

    // Note: ImageLoader::LoadImageToMemory handles nullptr file path.
    ImageLoader::LoadImageToMemory(_file_path,
                                   &image_data,
                                   image_width,
                                   image_height,
                                   image_channels,
                                   _success);

    if (false == _success) // From ImageLoader::LoadImageToMemory
    {
        Log_e("Unable to create texture from file.");
        _success = false;
        return 0;
    }

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Note: The image loader defaults to 4 bpp, which is why argument 7 to glTexImage2D below
    //       is hard-coded at { GL_RGBA }.
    GLenum internal_color_components;
    switch (image_channels)
    {
        // [ cfarvin::TODO ] case: 1 is grayscale
        case 3:
        {
            internal_color_components = GL_RGB;
            break;
        }
        case 4:
        {
            internal_color_components = GL_RGBA;
            break;
        }
        default:
        {
            std::stringstream ss;
            ss << "Invalid number of image channels provided: " << image_channels;
            Log_e(ss);
            _success = false;
            glDeleteTextures(1, &texture_id);
            return 0;
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,             // Texture type.
                 0,                         // Level of mipmap detail.
                 internal_color_components, // Number of internal color components
                 image_width,               // Image width.
                 image_height,              // Image height,.
                 0,                         // Boarder. Must be zero.
                 GL_RGBA,                   // Pixel format. (We default to loading as RGBA)
                 GL_UNSIGNED_BYTE,          // Pixel type.
                 image_data);               // Image data.

    glGenerateMipmap(GL_TEXTURE_2D);
    if ((true == _attempt_ansiotropic_filtering) &&
        (true == static_cast<bool>(glewIsSupported("GL_EXT_texture_filter_anisotropic"))))
    {
        GLfloat ansiotropic_sampling;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ansiotropic_sampling);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ansiotropic_sampling);
    }

    stbi_image_free(image_data);

    glt::QueryGLErrors(_success);
    if (false == _success)
    {
        Log_e("OpenGL errors during texture creation");
        glDeleteTextures(1, &texture_id);
        return 0;
    }

    return texture_id;
}

GLuint
glt::GetTestTextureRGB(bool&               _success,
                       const char* const&& _output_file, // Provide nullptr to skip writing to disk.
                       const unsigned int&& _image_width,
                       const unsigned int&& _image_height)
{
    _success = true;

    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char*          pixels;
    constexpr unsigned char channels = 3;
    // Create test texture data.
    {
        size_t                  row        = 0;
        constexpr unsigned char red[]      = { 255, 0, 0, 0 };
        constexpr unsigned char green[]    = { 0, 255, 0, 0 };
        const unsigned int      num_pixels = _image_width * _image_height;

        pixels = new unsigned char[num_pixels * sizeof(unsigned char) * channels];

        for (size_t pixel_index = 0; pixel_index < (num_pixels * channels); pixel_index += channels)
        {
            row = pixel_index / _image_width;
            for (size_t channel = 0; channel < channels; channel++)
            {
                if (pixel_index - (row * _image_width) >= (_image_width / 2))
                {
                    pixels[pixel_index + channel] = red[channel];
                }
                else
                {
                    pixels[pixel_index + channel] = green[channel];
                }
            }
        }
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,    // Texture type.
                 0,                // Level of mipmap detail.
                 GL_RGB,           // Number of internal color components
                 _image_width,     // Image width.
                 _image_height,    // Image height,.
                 0,                // Boarder. Must be zero.
                 GL_RGB,           // Pixel format.
                 GL_UNSIGNED_BYTE, // Pixel type.
                 pixels);          // Image data.
    glGenerateMipmap(GL_TEXTURE_2D);

    if (nullptr != _output_file)
    {
        int non_exit_disk_write_success = stbi_write_bmp(_output_file,
                                                         _image_width,
                                                         _image_height,
                                                         channels,
                                                         pixels);
        if (0 == non_exit_disk_write_success)
        {
            Log_w("Unable to write test texture to disk");
        }
    }

    delete pixels;

    glt::QueryGLErrors(_success);
    if (false == _success)
    {
        Log_e("OpenGL errors during test texture creation");
        glDeleteTextures(1, &texture_id);
        return 0;
    }

    return texture_id;
}
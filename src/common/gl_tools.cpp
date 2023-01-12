// clang-format off
#include "pch.h"
// clang-format on

#include "gl_tools.h"

#include "fileio.h"
#include "gl_function_wrappers_COPY.h"
#include "image_tools.h"
#include "logging.h"

void
glt::GLDebugCallback(GLenum        _source_in,
                     GLenum        _type_in,
                     GLuint        _id_in,
                     GLenum        _severity_in,
                     GLsizei       _length_in,
                     const GLchar* _message_in,
                     void*         _user_params_in)
{
    // Pacify unused formal variable compiler warning/errors.
    if (_source_in && _type_in && _id_in && _severity_in && _length_in && _message_in &&
        _user_params_in)
    {
    }

    // [ cfarvin::TODO ] Rely on this when it becomes useful. So far in testing,
    //                   even with purposeful errors, the glGetError() method in
    //                   the function wrapper implementation is catching everything
    //                   while this function never gets called.
    assert(false);
}

glt::VertexAttributeDescriptor::VertexAttributeDescriptor(
  const GLuint&&         _vertex_array_object_id_in,
  const GLuint&&         _vertex_attribute_index_in,
  const GLint&&          _component_count_in,
  const GLenum&&         _data_type_in,
  const GLboolean&&      _should_be_normalized_by_gpu_in,
  const GLsizei&&        _byte_stride_between_elements_in,
  const GLvoid*&&        _first_component_byte_offset_in,
  const VertexDataType&& _vertex_data_type_in)
  : vertex_array_object_id(_vertex_array_object_id_in)
  , vertex_attribute_index(_vertex_attribute_index_in)
  , component_count(_component_count_in)
  , data_type(_data_type_in)
  , should_be_normalized_by_gpu(_should_be_normalized_by_gpu_in)
  , byte_stride_between_elements(_byte_stride_between_elements_in)
  , first_component_byte_offset(_first_component_byte_offset_in)
  , vertex_data_type(_vertex_data_type_in)
{
    assert(0 != _component_count_in);
    assert(GL_FLOAT == _data_type_in);
    assert(false == _should_be_normalized_by_gpu_in);
    assert(VertexDataType::COUNT != _vertex_data_type_in);
}

glt::GLArrayBuffer::GLArrayBuffer(
  const GLuint&&                                _buffer_id_in,
  const GLsizei&&                               _target_buffer_element_count_in,
  const GLenum&&                                _primitive_drawing_mode_in,
  const std::vector<VertexAttributeDescriptor>& _glt_vertex_attribute_pointers_in) noexcept
  : buffer_info({ _buffer_id_in, _target_buffer_element_count_in, _primitive_drawing_mode_in })
  , vertex_attribute_pointers(_glt_vertex_attribute_pointers_in)
{
    assert(0 != _target_buffer_element_count_in);
};

glt::GLArrayBuffer::GLArrayBuffer(const GLArrayBuffer& _) noexcept
  : buffer_info({ _.buffer_info.buffer_id,
                  _.buffer_info.target_buffer_element_count,
                  _.buffer_info.primitive_drawing_mode })
  , vertex_attribute_pointers(_.vertex_attribute_pointers)
{
    assert(0 != _.buffer_info.target_buffer_element_count);
}

glt::GLElementBuffer::GLElementBuffer(const GLuint&&  _buffer_id_in,
                                      const GLsizei&& _target_buffer_element_count_in,
                                      const GLenum&&  _primitive_drawing_mode_in) noexcept
  : buffer_info({ _buffer_id_in, _target_buffer_element_count_in, _primitive_drawing_mode_in })
{
    assert(0 != _target_buffer_element_count_in);
}

glt::GLElementBuffer::GLElementBuffer(const GLElementBuffer& _) noexcept
  : buffer_info({ _.buffer_info.buffer_id,
                  _.buffer_info.target_buffer_element_count,
                  _.buffer_info.primitive_drawing_mode })
{
    assert(0 != _.buffer_info.target_buffer_element_count);
}

// [ cfarvin::TODO ] We have done no OpenGL cleanup
//                   via glDelete* and ilk.
glt::GLBufferStore::~GLBufferStore()
{
    if (nullptr != array_buffer)
    {
        delete array_buffer;
    }

    if (nullptr != element_buffer)
    {
        delete element_buffer;
    }
}

const GLuint
glt::GLBufferStore::AddArrayBuffer(
  bool&                                         _success_out,
  const GLuint&&                                _vao_in,
  const GLvoid* const                           _data_in,
  const GLsizeiptr&&                            _data_size_bytes_in,
  const GLsizei&&                               _target_buffer_element_count_in,
  const GLenum&&                                _target_buffer_usage_type_in,
  const GLenum&&                                _primitive_drawing_mode_in,
  const std::vector<VertexAttributeDescriptor>& _glt_vertex_attribute_pointers_in) noexcept
{
    assert(nullptr != _data_in);
    assert(0 != _data_size_bytes_in);
    assert(0 != _target_buffer_element_count_in);

    _success_out                        = false;
    ScopedVaoBinding scoped_vao_binding = ScopedVaoBinding(std::move(_vao_in));

    GLuint array_buffer_id;
    glfn::GenBuffers(1, &array_buffer_id);

    array_buffer = new GLArrayBuffer(std::move(array_buffer_id),
                                     std::move(_target_buffer_element_count_in),
                                     std::move(_primitive_drawing_mode_in),
                                     std::move(_glt_vertex_attribute_pointers_in));

    ScopedVboBinding scoped_vbo_binding = ScopedVboBinding(std::move(GL_ARRAY_BUFFER),
                                                           std::move(array_buffer_id));
    glfn::BufferData(GL_ARRAY_BUFFER, _data_size_bytes_in, _data_in, _target_buffer_usage_type_in);

    _success_out = true;
    return array_buffer_id;
}

const GLuint
glt::GLBufferStore::AddElementBuffer(bool&               _success_out,
                                     const GLuint&&      _vao_in,
                                     const GLvoid* const _data_in,
                                     const GLsizeiptr&&  _data_size_bytes_in,
                                     const GLsizei&&     _target_buffer_element_count_in,
                                     const GLenum&&      _target_buffer_usage_type_in,
                                     const GLenum&&      _primitive_drawing_mode_in) noexcept
{
    assert(nullptr != _data_in);
    assert(0 != _data_size_bytes_in);
    assert(0 != _target_buffer_element_count_in);

    _success_out = false;
    if (nullptr != element_buffer)
    {
        Log_e("Element buffers can be added exactly once.");
        return 0;
    }

    ScopedVaoBinding scoped_vao_binding = ScopedVaoBinding(std::move(_vao_in));

    GLuint element_buffer_id;
    glfn::GenBuffers(1, &element_buffer_id);

    element_buffer = new GLElementBuffer(std::move(element_buffer_id),
                                         std::move(_target_buffer_element_count_in),
                                         std::move(_primitive_drawing_mode_in));

    ScopedVboBinding scoped_vbo_binding = ScopedVboBinding(std::move(GL_ELEMENT_ARRAY_BUFFER),
                                                           std::move(element_buffer_id));
    glfn::BufferData(GL_ELEMENT_ARRAY_BUFFER,
                     _data_size_bytes_in,
                     _data_in,
                     _target_buffer_usage_type_in);

    _success_out = true;
    return element_buffer_id;
}

static void
LogGLCompilationError(const GLuint& compilation_step)
{
    constexpr unsigned int max_error_message_length                = 512;
    GLchar                 error_message[max_error_message_length] = {};

    if (GL_TRUE == glfn::IsShader(compilation_step))
    {
        glfn::GetShaderInfoLog(compilation_step,
                               max_error_message_length,
                               nullptr,
                               &(error_message[0]));
    }
    else if (GL_TRUE == glfn::IsProgram(compilation_step))
    {
        glfn::GetProgramInfoLog(compilation_step,
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

void
glt::CreateShaderProgram(bool&               _success_out,
                         const char* const&& _vertex_shader_file_path_in,
                         const char* const&& _fragment_shader_file_path_in,
                         GLuint&             _program_id_out)
{
    _program_id_out                = 0;
    _success_out                   = true;
    size_t file_data_size_bytes_in = 0;

    GLuint      vertex_shader = glfn::CreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_source_std_string;
    LoadFileToStdString(_success_out,
                        _vertex_shader_file_path_in,
                        file_data_size_bytes_in,
                        vertex_shader_source_std_string);
    const GLchar* const vertex_shader_source = vertex_shader_source_std_string.c_str();
    glfn::ShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);

    glfn::CompileShader(vertex_shader);
    GLint vertex_compiled;
    glfn::GetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_compiled);
    if (GL_TRUE != vertex_compiled)
    {
        LogGLCompilationError(vertex_shader);
        _success_out = false;
        return;
    }

    GLuint      fragment_shader = glfn::CreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_source_std_string;
    LoadFileToStdString(_success_out,
                        _fragment_shader_file_path_in,
                        file_data_size_bytes_in,
                        fragment_shader_source_std_string);
    const GLchar* const fragment_shader_source = fragment_shader_source_std_string.c_str();
    glfn::ShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);

    glfn::CompileShader(fragment_shader);
    GLint fragment_compiled;
    glfn::GetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_compiled);
    if (fragment_compiled != GL_TRUE)
    {
        LogGLCompilationError(fragment_shader);
        _success_out = false;
        return;
    }

    GLuint program = glfn::CreateProgram();
    glfn::AttachShader(program, vertex_shader);
    glfn::AttachShader(program, fragment_shader);
    glfn::LinkProgram(program);

    int error_log_list_length = 0;
    glfn::GetProgramiv(program, GL_INFO_LOG_LENGTH, &error_log_list_length);
    if (error_log_list_length > 1) // Note: The list length includes the null termination character.
    {
        LogGLCompilationError(program);
        _success_out = false;
        return;
    }

    _program_id_out = program;
}

GLuint
glt::GetTexture2DFromImage(bool&               _success_out,
                           const char* const&& _file_path,
                           const bool&&        _attempt_ansiotropic_filtering)
{
    _success_out = true;

    unsigned char* image_data_in;
    int            image_width;
    int            image_height;
    int            image_channels;

    // Note: LoadImageToMemory handles nullptr file path.
    LoadImageToMemory(_success_out,
                      _file_path,
                      &image_data_in,
                      image_width,
                      image_height,
                      image_channels);
    if (false == _success_out)
    {
        Log_e("Unable to create texture from file.");
        _success_out = false;
        return 0;
    }

    GLuint texture_id = 0;
    glfn::GenTextures(1, &texture_id);
    ScopedTextureBinding scoped_texture_binding = ScopedTextureBinding(std::move(GL_TEXTURE_2D),
                                                                       std::move(texture_id));

    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
            _success_out = false;
            glfn::DeleteTextures(1, &texture_id);
            return 0;
        }
    }

    glfn::PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glfn::TexImage2D(GL_TEXTURE_2D,             // Texture type.
                     0,                         // Level of mipmap detail.
                     internal_color_components, // Number of internal color components
                     image_width,               // Image width.
                     image_height,              // Image height,.
                     0,                         // Boarder. Must be zero.
                     GL_RGBA,                   // Pixel format. (We default to loading as RGBA)
                     GL_UNSIGNED_BYTE,          // Pixel type.
                     image_data_in);            // Image data.

    glfn::GenerateMipmap(GL_TEXTURE_2D);
    if (true == _attempt_ansiotropic_filtering)
    {
        // [ cfarvin::TODO ] Need to check that ansiotropic filtering is supported.
        //                   String is: "GL_EXT_texture_filter_anisotropic"
        GLfloat ansiotropic_sampling;
        glfn::GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ansiotropic_sampling);
        glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, ansiotropic_sampling);
    }

    stbi_image_free(image_data_in);
    return texture_id;
}

GLuint
glt::GetTestTextureRGB(bool&               _success_out,
                       const char* const&& _output_file, // Provide nullptr to skip writing to disk.
                       const unsigned int&& _image_width,
                       const unsigned int&& _image_height) noexcept
{
    _success_out = true;

    GLuint texture_id = 0;
    glfn::GenTextures(1, &texture_id);
    ScopedTextureBinding scoped_texture_binding = ScopedTextureBinding(std::move(GL_TEXTURE_2D),
                                                                       std::move(texture_id));

    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glfn::TexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    glfn::PixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glfn::TexImage2D(GL_TEXTURE_2D,    // Texture type.
                     0,                // Level of mipmap detail.
                     GL_RGB,           // Number of internal color components
                     _image_width,     // Image width.
                     _image_height,    // Image height,.
                     0,                // Boarder. Must be zero.
                     GL_RGB,           // Pixel format.
                     GL_UNSIGNED_BYTE, // Pixel type.
                     pixels);          // Image data.
    glfn::GenerateMipmap(GL_TEXTURE_2D);

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

    return texture_id;
}

glt::ScopedVaoBinding::ScopedVaoBinding(const GLuint&& _vertex_array_object_id_in)
{
    assert(0 != _vertex_array_object_id_in);
    glfn::BindVertexArray(_vertex_array_object_id_in);
}

glt::ScopedVaoBinding::~ScopedVaoBinding() { glfn::BindVertexArray(0); }

glt::ScopedVboBinding::ScopedVboBinding(const GLenum&& _buffer_target_in,
                                        const GLuint&& _buffer_object_id_in)
  : buffer_target(_buffer_target_in)
{
    assert(0 != _buffer_object_id_in);
    glfn::BindBuffer(_buffer_target_in, _buffer_object_id_in);
}

glt::ScopedVboBinding::~ScopedVboBinding()
{
    glfn::BindVertexArray(0);
    glfn::BindBuffer(buffer_target, 0);
}

glt::ScopedTextureBinding::ScopedTextureBinding(const GLenum&& _texture_target_in,
                                                const GLuint&& _texture_object_id_in)
  : texture_target(_texture_target_in)
{
    assert(0 != _texture_object_id_in);
    glfn::BindTexture(_texture_target_in, _texture_object_id_in);
}

glt::ScopedTextureBinding::~ScopedTextureBinding() { glfn::BindTexture(texture_target, 0); }

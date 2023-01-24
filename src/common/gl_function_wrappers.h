#ifndef gl_function_wrappers_h
#define gl_function_wrappers_h

// clang-format off
#include "pch.h"
// clang-format on

#include "logging.h"
#include "state_tools.h"

#ifdef _ENGINE_DEBUG_
#define _DEBUG_FILE_AND_LINE_PARAMS_ , const char *const &&_file_in, const unsigned int &&_line_in
#define _DEBUG_FILE_AND_LINE_ARGS_   , std::move(__FILE__), std::move(__LINE__)
#define _DEBUG_FILE_AND_LINE_PARAMS_NO_COMMA_ \
    const char *const &&_file_in, const unsigned int &&_line_in
#define _DEBUG_FILE_AND_LINE_ARGS_NO_COMMA_ std::move(__FILE__), std::move(__LINE__)
#define _DEBUG_QUERY_ERRORS_ARGS            std::move(_file_in), std::move(_line_in)
#else
#define _DEBUG_FILE_AND_LINE_PARAMS_          /* NO OP */
#define _DEBUG_FILE_AND_LINE_ARGS_            /* NO OP */
#define _DEBUG_FILE_AND_LINE_PARAMS_NO_COMMA_ /* NO OP */
#define _DEBUG_FILE_AND_LINE_ARGS_NO_COMMA_   /* NO OP */
#define _DEBUG_QUERY_ERRORS_ARGS              /* NO OP */
#endif

//
// General Note:
//
// It is recommended to use the macros in rather than the Impl_<glFunction> definitions,
// unless you particularly enjoy typing "std::move()" and "Impl_". In which case I ask
// that you please excuse this interruption.
//
#define ActiveTexture(_texture_in) \
    Impl_ActiveTexture(std::move(_texture_in) _DEBUG_FILE_AND_LINE_ARGS_)
#define AttachShader(_program_in, _shader_in) \
    Impl_AttachShader(std::move(_program_in), std::move(_shader_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define BindBuffer(_buffer_target_in, _buffer_in) \
    Impl_BindBuffer(std::move(_buffer_target_in), std::move(_buffer_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define BindTexture(_target_in, _texture_in) \
    Impl_BindTexture(std::move(_target_in), std::move(_texture_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define BindVertexArray(_vertex_array_in) \
    Impl_BindVertexArray(std::move(_vertex_array_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define BufferData(_target_in, _size_in, _data_in, _usage_in) \
    Impl_BufferData(std::move(_target_in),                    \
                    std::move(_size_in),                      \
                    std::move(_data_in),                      \
                    std::move(_usage_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define ClearColor(r_in, g_in, b_in, a_in) \
    Impl_ClearColor(std::move(r_in),       \
                    std::move(g_in),       \
                    std::move(b_in),       \
                    std::move(a_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define Clear(_bit_mask_in) Impl_Clear(std::move(_bit_mask_in) _DEBUG_FILE_AND_LINE_ARGS_)
#define CullFace(_facet_in) Impl_CullFace(std::move(_facet_in) _DEBUG_FILE_AND_LINE_ARGS_)
#define CompileShader(_shader_in) \
    Impl_CompileShader(std::move(_shader_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define CreateProgram() Impl_CreateProgram(_DEBUG_FILE_AND_LINE_ARGS_NO_COMMA_)

#define CreateShader(_shader_type_in) \
    Impl_CreateShader(std::move(_shader_type_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define DebugMessageCallback(_callback_in, _user_params_in) \
    Impl_DebugMessageCallback(std::move(_callback_in),      \
                              std::move(_user_params_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define DeleteTextures(_texture_count_in, _textures_in) \
    Impl_DeleteTextures(std::move(_texture_count_in),   \
                        std::move(_textures_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define DepthFunc(_depth_comparison_mode_in) \
    Impl_DepthFunc(std::move(_depth_comparison_mode_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define DrawElements(_drawing_mode_in,                \
                     _element_count_in,               \
                     _index_data_type_in,             \
                     _data_byte_offset_in)            \
    Impl_DrawElements(std::move(_drawing_mode_in),    \
                      std::move(_element_count_in),   \
                      std::move(_index_data_type_in), \
                      std::move(_data_byte_offset_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define Enable(_capability_in) Impl_Enable(std::move(_capability_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define EnableVertexAttribArray(_index_in) \
    Impl_EnableVertexAttribArray(std::move(_index_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define FrontFace(_winding_order_in) \
    Impl_FrontFace(std::move(_winding_order_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define GenBuffers(_buffer_count_in, _buffers_out) \
    Impl_GenBuffers(std::move(_buffer_count_in), std::move(_buffers_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GenerateMipmap(_target_in) \
    Impl_GenerateMipmap(std::move(_target_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define GenTextures(_texture_count_in, _textures_out) \
    Impl_GenTextures(std::move(_texture_count_in),    \
                     std::move(_textures_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GenVertexArrays(_generate_count_in, _vertex_arrays_out) \
    Impl_GenVertexArrays(std::move(_generate_count_in),         \
                         std::move(_vertex_arrays_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetFloatv(_paramter_name_in, _data_out) \
    Impl_GetFloatv(std::move(_paramter_name_in), std::move(_data_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetProgramInfoLog(_program_in, _max_length_in, _length_out, _info_log_out) \
    Impl_GetProgramInfoLog(std::move(_program_in),                                 \
                           std::move(_max_length_in),                              \
                           std::move(_length_out),                                 \
                           std::move(_info_log_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetProgramiv(_program_in, _object_parameter_in, _parameters_out) \
    Impl_GetProgramiv(std::move(_program_in),                            \
                      std::move(_object_parameter_in),                   \
                      std::move(_parameters_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetShaderInfoLog(_shader_in, _max_length_in, _length_out, _info_log_out) \
    Impl_GetShaderInfoLog(std::move(_shader_in),                                 \
                          std::move(_max_length_in),                             \
                          std::move(_length_out),                                \
                          std::move(_info_log_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetShaderiv(_shader_in, _parameter_type_in, _parameter_out) \
    Impl_GetShaderiv(std::move(_shader_in),                         \
                     std::move(_parameter_type_in),                 \
                     std::move(_parameter_out) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetString(_name_in) Impl_GetString(std::move(_name_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetStringi(_name_in, _index_in) \
    Impl_GetStringi(std::move(_name_in), std::move(_index_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define GetUniformLocation(_program_in, _name_in) \
    Impl_GetUniformLocation(std::move(_program_in), std::move(_name_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define IsShader(_shader_in)   Impl_IsShader(std::move(_shader_in) _DEBUG_FILE_AND_LINE_ARGS_)
#define IsProgram(_program_in) Impl_IsProgram(std::move(_program_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define LinkProgram(_program_in) Impl_LinkProgram(std::move(_program_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define PixelStorei(_parameter_name_in, _parameter_value_in) \
    Impl_PixelStorei(std::move(_parameter_name_in),          \
                     std::move(_parameter_value_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define ShaderSource(_shader_in, _count_in, _string_in, _length_in) \
    Impl_ShaderSource(std::move(_shader_in),                        \
                      std::move(_count_in),                         \
                      std::move(_string_in),                        \
                      std::move(_length_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define TexImage2D(_target_in,                      \
                   _level_in,                       \
                   _internal_format_in,             \
                   _width_in,                       \
                   _height_in,                      \
                   _border_in,                      \
                   _format_in,                      \
                   _type_in,                        \
                   _data_in)                        \
    Impl_TexImage2D(std::move(_target_in),          \
                    std::move(_level_in),           \
                    std::move(_internal_format_in), \
                    std::move(_width_in),           \
                    std::move(_height_in),          \
                    std::move(_border_in),          \
                    std::move(_format_in),          \
                    std::move(_type_in),            \
                    std::move(_data_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define TexParameter(_target_in, _parameter_name_in, _parameter_in) \
    Impl_TexParameter(std::move(_target_in),                        \
                      std::move(_parameter_name_in),                \
                      std::move(_parameter_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define Uniform1f(_location_in, _float_in) \
    Impl_Uniform1f(std::move(_location_in), std::move(_float_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define UniformMatrix4fv(_location_in, _count_in, _should_transpose_in, _value_in) \
    Impl_UniformMatrix4fv(std::move(_location_in),                                 \
                          std::move(_count_in),                                    \
                          std::move(_should_transpose_in),                         \
                          std::move(_value_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define UseProgram(_program_in) Impl_UseProgram(std::move(_program_in) _DEBUG_FILE_AND_LINE_ARGS_)

#define VertexAttribPointer(_vertex_attribute_index_in,                   \
                            _component_count_in,                          \
                            _data_type_in,                                \
                            _should_be_normalized_by_gpu_in,              \
                            _byte_stride_between_elements_in,             \
                            _first_component_byte_offset_in)              \
    Impl_VertexAttribPointer(std::move(_vertex_attribute_index_in),       \
                             std::move(_component_count_in),              \
                             std::move(_data_type_in),                    \
                             std::move(_should_be_normalized_by_gpu_in),  \
                             std::move(_byte_stride_between_elements_in), \
                             std::move(_first_component_byte_offset_in)   \
                               _DEBUG_FILE_AND_LINE_ARGS_)

#define Viewport(_lower_left_corner_x_in, _lower_left_corner_y_in, _width_in, _height_in) \
    Impl_Viewport(std::move(_lower_left_corner_x_in),                                     \
                  std::move(_lower_left_corner_y_in),                                     \
                  std::move(_width_in),                                                   \
                  std::move(_height_in) _DEBUG_FILE_AND_LINE_ARGS_)

constexpr static const char* const ogl_dll_c_str = "opengl32.dll";
static const HINSTANCE             ogl_dll       = LoadLibraryA(ogl_dll_c_str);

// Forward declarations from "gl_tools.h".
struct glt::GLState::ClearColor_t;
struct glt::GLState::Viewport_t;

#ifdef _ENGINE_DEBUG_
static inline void
// [ cfarvin::REMOVE ] Remove the default parameter arguments.
s_QueryErrors(const char* const&& _file_in = nullptr, const unsigned int&& _line_in = 0) noexcept
{
    std::stringstream outer_ss;
    std::stringstream inner_ss;

    std::string gl_error_string;
    auto        error_enum_to_str = [&](const GLenum& _enum_in)
    {
        switch (_enum_in)
        {
            CASE_ENUM_TO_STR_VAR(GL_INVALID_ENUM, gl_error_string);
            CASE_ENUM_TO_STR_VAR(GL_INVALID_VALUE, gl_error_string);
            CASE_ENUM_TO_STR_VAR(GL_INVALID_OPERATION, gl_error_string);
            CASE_ENUM_TO_STR_VAR(GL_INVALID_FRAMEBUFFER_OPERATION, gl_error_string);
            CASE_ENUM_TO_STR_VAR(GL_OUT_OF_MEMORY, gl_error_string);
            CASE_ENUM_TO_STR_VAR(GL_STACK_UNDERFLOW, gl_error_string);
            CASE_ENUM_TO_STR_VAR(GL_STACK_OVERFLOW, gl_error_string);
            default:
            {
                gl_error_string = "Unrecognized error enumeration.";
                Log_e(gl_error_string);
            }
        }
    };

    static const GLenum (*local_glGetError)() = (const GLenum (*)())GetProcAddress(ogl_dll,
                                                                                   "glGetError");
    assert(nullptr != local_glGetError);

    bool                    errors_found    = false;
    GLenum                  gl_error        = local_glGetError();
    constexpr unsigned char max_error_count = 10;
    unsigned char           error_count     = 0;
    while (gl_error != GL_NO_ERROR)
    {
        if (max_error_count <= error_count)
        {
            inner_ss << std::endl
                     << "   Note: max_error_count reached. This can sometimes happen when OpenGL "
                        "errors occur after the OpenGL context has been destroyed.";
            break;
        }

        errors_found = true;
        error_enum_to_str(gl_error);
        inner_ss << std::endl << "   " << gl_error_string;
        gl_error = local_glGetError();

        error_count++;
    }

    if (true == errors_found)
    {
        outer_ss << "[ error ]: The following OpenGL error enumerations were reported:"
                 << inner_ss.str() << std::endl
                 << "   File: " << _file_in << ":" << _line_in << std::endl
                 << "   Exiting..." << std::endl
                 << std::endl;
        std::fprintf(stderr, "%s", outer_ss.str().c_str());
        std::fflush(stderr);
        std::exit(EXIT_FAILURE);
    }
}

static inline void
sLog_w(const std::string& _log_message_in) noexcept
{
    Log_w(_log_message_in.c_str());
}
#else
#define s_QueryErrors() /* NO OP */
#define sLog_w(str)     /* NO OP */
#endif

namespace glfn
{
    inline void
    Impl_ActiveTexture(const GLenum&& _texture_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glActiveTexture)(
          GLenum) = (const void (*)(GLenum))wglGetProcAddress("glActiveTexture");

        assert(nullptr != local_glActiveTexture);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLenum* active_texture = &(state_cache->opengl_state->active_texture);
        if (_texture_in == *active_texture && (true == state_initialized))
        {
            return;
        }

        local_glActiveTexture(_texture_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *active_texture   = _texture_in;
        state_initialized = true;
    }

    inline void
    Impl_AttachShader(const GLuint&&            _program_in,
                      const GLuint&& _shader_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glAttachShader)(
          GLuint,
          GLuint) = (const void (*)(GLuint, GLuint))wglGetProcAddress("glAttachShader");
        assert(nullptr != local_glAttachShader);

        local_glAttachShader(_program_in, _shader_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_BindBuffer(const GLenum&&            _buffer_target_in,
                    const GLuint&& _buffer_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glBindBuffer)(
          GLenum,
          GLuint) = (const void (*)(GLenum, GLuint))wglGetProcAddress("glBindBuffer");

        assert(nullptr != local_glBindBuffer);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        // [ cfarvin::TODO ] Performance check. Does commenting out these checks make
        //                   a meaningful difference under high-stress workloads? If
        //                   so, these are prevalent in this file. The basic assumption
        //                   is that comparisons on simple types will be faster than
        //                   cpu->gpu function calls w/ driver overhead in cases where
        //                   the values do not actually need to be changed.
        const unsigned char buffer_target_array_index = static_cast<unsigned char>(
          GetBufferTargetArrayIndex(std::move(_buffer_target_in)));
        assert(buffer_target_array_length != buffer_target_array_index); // Equivalent to COUNT.

        GLuint* buffer_id = &(
          state_cache->opengl_state->bound_buffers_by_target_type[buffer_target_array_index]);
        if ((_buffer_in == *buffer_id) && (true == state_initialized))
        {
            return;
        }

        local_glBindBuffer(_buffer_target_in, _buffer_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *buffer_id        = _buffer_in;
        state_initialized = true;
    }

    inline void
    Impl_BindTexture(const GLenum&&             _target_in,
                     const GLuint&& _texture_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized                            = false;
        static const void (*local_glBindTexture)(GLenum, GLuint) = (const void (*)(
          GLenum target,
          GLuint texture))GetProcAddress(ogl_dll, "glBindTexture");

        assert(nullptr != local_glBindTexture);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        const unsigned char texture_target_array_index = static_cast<unsigned char>(
          GetTextureTargetArrayIndex(std::move(_target_in)));
        assert(texture_target_array_length != texture_target_array_index); // Equivalent to COUNT.

        GLuint* texture_id = &(
          state_cache->opengl_state->bound_textures_by_target_type[texture_target_array_index]);
        if (_texture_in == *texture_id && (true == state_initialized))
        {
            return;
        }

        local_glBindTexture(_target_in, _texture_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *texture_id       = _texture_in;
        state_initialized = true;
    }

    inline void
    Impl_BindVertexArray(const GLuint&& _vertex_array_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glBindVertexArray)(
          GLuint) = (const void (*)(GLuint))wglGetProcAddress("glBindVertexArray");

        assert(nullptr != local_glBindVertexArray);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLuint* currently_bound_vertex_array = &(
          state_cache->opengl_state->currently_bound_vertex_array);
        if ((_vertex_array_in == *currently_bound_vertex_array) && (true == state_initialized))
        {
            return;
        }

        local_glBindVertexArray(_vertex_array_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *currently_bound_vertex_array = _vertex_array_in;
        state_initialized             = true;
    }

    inline void
    Impl_BufferData(const GLenum&&           _target_in,
                    const GLsizeiptr&&       _size_in,
                    const GLvoid* const&&    _data_in,
                    const GLenum&& _usage_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glBufferData)(GLenum, GLsizeiptr, const void*, GLenum) =
          (const void (*)(GLenum, GLsizeiptr, const void*, GLenum))wglGetProcAddress(
            "glBufferData");
        assert(nullptr != local_glBufferData);

        local_glBufferData(_target_in, _size_in, _data_in, _usage_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_Clear(const GLbitfield&& _bit_mask_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glClear)(
          GLbitfield) = (const void (*)(GLbitfield))GetProcAddress(ogl_dll, "glClear");
        assert(nullptr != local_glClear);

        local_glClear(_bit_mask_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_ClearColor(const float&&      r_in,
                    const float&&      g_in,
                    const float&&      b_in,
                    const float&& a_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) =
          (const void (*)(GLfloat, GLfloat, GLfloat, GLfloat))GetProcAddress(ogl_dll,
                                                                             "glClearColor");

        assert(nullptr != local_glClearColor);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        glt::GLState::ClearColor_t* clear_color = &(state_cache->opengl_state->ClearColor);
        if ((r_in == clear_color->r) && (g_in == clear_color->g) && (b_in == clear_color->b) &&
            (a_in == clear_color->a) && (true == state_initialized))
        {
            return;
        }

        local_glClearColor(r_in, g_in, b_in, a_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *clear_color      = { r_in, g_in, b_in, a_in };
        state_initialized = true;
    }

    inline void
    Impl_CompileShader(const GLuint&& _shader_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glCompileShader)(
          GLuint) = (const void (*)(GLuint))wglGetProcAddress("glCompileShader");
        assert(nullptr != local_glCompileShader);

        local_glCompileShader(_shader_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline GLuint
    Impl_CreateProgram(_DEBUG_FILE_AND_LINE_PARAMS_NO_COMMA_)
    {
        static const GLuint (*local_glCreateProgram)(void) = (const GLuint (*)())wglGetProcAddress(
          "glCreateProgram");
        assert(nullptr != local_glCreateProgram);

        GLuint p = local_glCreateProgram();
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        return p;
    }

    inline GLuint
    Impl_CreateShader(const GLenum&& _shader_type_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const GLuint (*local_glCreateShader)(
          GLenum) = (const GLuint (*)(GLenum))wglGetProcAddress("glCreateShader");
        assert(nullptr != local_glCreateShader);

        GLuint s = local_glCreateShader(_shader_type_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        return s;
    }

    inline void
    Impl_CullFace(const GLenum&& _facet_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glCullFace)(
          GLenum) = (const void (*)(GLenum))GetProcAddress(ogl_dll, "glCullFace");

        assert(nullptr != local_glCullFace);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLenum* cull_face_facet = &(state_cache->opengl_state->cull_face_facet);
        if (_facet_in == *cull_face_facet && (true == state_initialized))
        {
            return;
        }

        local_glCullFace(_facet_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *cull_face_facet  = _facet_in;
        state_initialized = true;
    }

    inline void
    Impl_DebugMessageCallback(
      void (*&&_callback_in)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, void*),
      void*&& _user_params_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        assert(nullptr != _callback_in);

        static const void (*local_DebugMessageCallback)(
          void (*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, void*),
          void*) =
          (const void (*)(void (*)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, void*),
                          void*))wglGetProcAddress("glDebugMessageCallback");

        assert(nullptr != local_DebugMessageCallback);

        local_DebugMessageCallback(_callback_in, _user_params_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_DeleteTextures(const GLsizei&&              _texture_count_in,
                        const GLuint*&& _textures_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glDeleteTextures)(GLsizei, const GLuint*) = (const void (*)(
          GLsizei,
          const GLuint*))GetProcAddress(ogl_dll, "glDeleteTextures");
        assert(nullptr != local_glDeleteTextures);

        local_glDeleteTextures(_texture_count_in, _textures_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_DepthFunc(const GLenum&& _depth_comparison_mode_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glDepthFunc)(
          GLenum) = (const void (*)(GLenum))GetProcAddress(ogl_dll, "glDepthFunc");

        assert(nullptr != local_glDepthFunc);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLenum* depth_comparison_mode = &(state_cache->opengl_state->depth_comparison_mode);
        if (_depth_comparison_mode_in == *depth_comparison_mode && (true == state_initialized))
        {
            return;
        }

        local_glDepthFunc(_depth_comparison_mode_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        state_initialized      = true;
        *depth_comparison_mode = _depth_comparison_mode_in;
    }

    inline void
    Impl_DrawElements(const GLenum&&                     _drawing_mode_in,
                      const GLsizei&&                    _element_count_in,
                      const GLenum&&                     _index_data_type_in,
                      const void*&& _data_byte_offset_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glDrawElements)(GLenum, GLsizei, GLenum, const void*) =
          (const void (*)(GLenum, GLsizei, GLenum, const void*))GetProcAddress(ogl_dll,
                                                                               "glDrawElements");

        assert(nullptr != local_glDrawElements);

        local_glDrawElements(_drawing_mode_in,
                             _element_count_in,
                             _index_data_type_in,
                             _data_byte_offset_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_Enable(const GLenum&& _capability_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glEnable)(
          GLenum) = (const void (*)(GLenum))GetProcAddress(ogl_dll, "glEnable");

        assert(nullptr != local_glEnable);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        std::unordered_set<GLenum>* enabled_server_side_capabilities = &(
          state_cache->opengl_state->enabled_server_side_capabilities);
        if (true == enabled_server_side_capabilities->contains(_capability_in) &&
            (true == state_initialized))
        {
            return;
        }

        local_glEnable(_capability_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        enabled_server_side_capabilities->emplace(_capability_in);
        state_initialized = true;
    }

    inline void
    Impl_EnableVertexAttribArray(const GLuint&& _index_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glEnableVertexAttribArray)(
          GLuint) = (const void (*)(GLuint))wglGetProcAddress("glEnableVertexAttribArray");

        assert(nullptr != local_glEnableVertexAttribArray);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLuint* currently_enabled_vertex_attribute_array = &(
          state_cache->opengl_state->currently_enabled_vertex_attribute_array);
        if (_index_in == *currently_enabled_vertex_attribute_array && (true == state_initialized))
        {
            return;
        }

        local_glEnableVertexAttribArray(_index_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *currently_enabled_vertex_attribute_array = _index_in;
        state_initialized                         = true;
    }

    inline void
    Impl_FrontFace(const GLenum&& _winding_order_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*lobal_glFrontFace)(
          GLenum) = (const void (*)(GLenum))GetProcAddress(ogl_dll, "glFrontFace");

        assert(nullptr != lobal_glFrontFace);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLenum* winding_order = &(state_cache->opengl_state->front_face);
        if (_winding_order_in == *winding_order && (true == state_initialized))
        {
            return;
        }

        lobal_glFrontFace(_winding_order_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
        *winding_order    = _winding_order_in;
        state_initialized = true;
    }

    inline void
    Impl_GenBuffers(const GLsizei&&        _buffer_count_in,
                    GLuint*&& _buffers_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGenBuffers)(
          GLsizei,
          GLuint*) = (const void (*)(GLsizei, GLuint*))wglGetProcAddress("glGenBuffers");

        assert(nullptr != local_glGenBuffers);

        local_glGenBuffers(_buffer_count_in, _buffers_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GenerateMipmap(const GLenum&& _target_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGenerateMipmap)(
          GLenum) = (const void (*)(GLenum))wglGetProcAddress("glGenerateMipmap");

        assert(nullptr != local_glGenerateMipmap);

        local_glGenerateMipmap(_target_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GenTextures(const GLsizei&&         _texture_count_in,
                     GLuint*&& _textures_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGenTextures)(
          GLsizei,
          GLuint*) = (const void (*)(GLsizei, GLuint*))GetProcAddress(ogl_dll, "glGenTextures");

        assert(nullptr != local_glGenTextures);

        local_glGenTextures(_texture_count_in, _textures_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GenVertexArrays(const GLsizei&&              _generate_count_in,
                         GLuint*&& _vertex_arrays_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGenVertexArrays)(
          GLsizei,
          GLuint*) = (const void (*)(GLsizei, GLuint*))wglGetProcAddress("glGenVertexArrays");

        assert(nullptr != local_glGenVertexArrays);

        local_glGenVertexArrays(_generate_count_in, _vertex_arrays_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GetFloatv(const GLenum&&     _parameter_name_in,
                   GLfloat* _data_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGetFloatv)(
          GLenum,
          GLfloat*) = (const void (*)(GLenum, GLfloat*))GetProcAddress(ogl_dll, "glGetFloatv");

        assert(nullptr != local_glGetFloatv);

        local_glGetFloatv(_parameter_name_in, _data_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GetProgramInfoLog(const GLuint&&          _program_in,
                           const GLsizei&&         _max_length_in,
                           GLsizei*&&              _length_out,
                           GLchar*&& _info_log_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*) =
          (const void (*)(GLuint, GLsizei, GLsizei*, GLchar*))wglGetProcAddress(
            "glGetProgramInfoLog");

        assert(nullptr != local_glGetProgramInfoLog);

        local_glGetProgramInfoLog(_program_in, _max_length_in, _length_out, _info_log_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GetProgramiv(const GLuint&&           _program_in,
                      const GLenum             _object_parameters_in,
                      GLint*&& _parameters_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGetProgramiv)(
          GLuint,
          GLenum,
          GLint*) = (const void (*)(GLuint, GLenum, GLint*))wglGetProcAddress("glGetProgramiv");

        assert(nullptr != local_glGetProgramiv);

        local_glGetProgramiv(_program_in, _object_parameters_in, _parameters_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GetShaderiv(const GLuint&&          _shader_in,
                     const GLenum&&          _parameter_type_in,
                     GLint*&& _parameter_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGetShaderiv)(
          GLuint,
          GLenum,
          GLint*) = (const void (*)(GLuint, GLenum, GLint*))wglGetProcAddress("glGetShaderiv");

        assert(nullptr != local_glGetShaderiv);

        local_glGetShaderiv(_shader_in, _parameter_type_in, _parameter_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_GetShaderInfoLog(const GLuint&&          _shader_in,
                          const GLsizei&&         _max_length_in,
                          GLsizei*&&              _length_out,
                          GLchar*&& _info_log_out _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*) =
          (const void (*)(GLuint, GLsizei, GLsizei*, GLchar*))wglGetProcAddress(
            "glGetShaderInfoLog");

        assert(nullptr != local_glGetShaderInfoLog);

        local_glGetShaderInfoLog(_shader_in, _max_length_in, _length_out, _info_log_out);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline const GLubyte*
    Impl_GetString(const GLenum&& _name_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const GLubyte* (*local_glGetString)(
          GLenum) = (const GLubyte* (*)(GLenum))GetProcAddress(ogl_dll, "glGetString");

        assert(nullptr != local_glGetString);

        const GLubyte* b = local_glGetString(_name_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        return b;
    }

    inline const GLubyte*
    Impl_GetStringi(const GLenum&& _name_in, const GLuint&& _index_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const GLubyte* (*local_glGetStringi)(
          GLenum,
          GLuint) = (const GLubyte* (*)(GLenum, GLuint))GetProcAddress(ogl_dll, "glGetStringi");

        assert(nullptr != local_glGetStringi);

        const GLubyte* b = local_glGetStringi(_name_in, _index_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        return b;
    }

    inline GLint
    Impl_GetUniformLocation(const GLuint&&                 _program_in,
                            const GLchar* const&& _name_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const GLuint (*local_glGetUniformLocation)(GLuint, const GLchar* const) =
          (const GLuint (*)(GLuint, const GLchar* const))wglGetProcAddress("glGetUniformLocation");

        assert(nullptr != local_glGetUniformLocation);

        GLint u = local_glGetUniformLocation(_program_in, _name_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
        return u;
    }

    inline bool
    Impl_IsShader(const GLuint&& _shader_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const bool (*local_glIsShader)(GLuint) = (const bool (*)(GLuint))wglGetProcAddress(
          "glIsShader");

        assert(nullptr != local_glIsShader);

        bool b = local_glIsShader(_shader_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
        return b;
    }

    inline bool
    Impl_IsProgram(const GLuint&& _program_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const bool (*local_glIsProgram)(GLuint) = (const bool (*)(GLuint))wglGetProcAddress(
          "glIsProgram");

        assert(nullptr != local_glIsProgram);

        bool b = local_glIsProgram(_program_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
        return b;
    }

    inline void
    Impl_LinkProgram(const GLuint&& _program_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glLinkProgram)(
          GLuint) = (const void (*)(GLuint))wglGetProcAddress("glLinkProgram");

        assert(nullptr != local_glLinkProgram);

        local_glLinkProgram(_program_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_PixelStorei(const GLenum&&                   _paramter_name_in,
                     const GLint&& _paramter_value_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glPixelStorei)(
          GLenum,
          GLint) = (const void (*)(GLenum, GLint))GetProcAddress(ogl_dll, "glPixelStorei");

        assert(nullptr != local_glPixelStorei);

        local_glPixelStorei(_paramter_name_in, _paramter_value_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_ShaderSource(const GLuint&&            _shader_in,
                      const GLsizei&&           _count_in,
                      const GLchar* const*&&    _string_in,
                      const GLint*&& _length_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glShaderSource)(
          GLuint,
          GLsizei,
          const GLchar* const*,
          const GLint*) = (const void (*)(GLuint, GLsizei, const GLchar* const*, const GLint*))
          wglGetProcAddress("glShaderSource");

        assert(nullptr != local_glShaderSource);

        local_glShaderSource(_shader_in, _count_in, _string_in, _length_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_TexImage2D(const GLenum&&         _target_in,
                    const GLint&&          _level_in,
                    const GLint&&          _internal_format_in,
                    const GLsizei&&        _width_in,
                    const GLsizei&&        _height_in,
                    const GLint&&          _border_in,
                    const GLenum&&         _format_in,
                    const GLenum&&         _type_in,
                    const void*&& _data_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glTexImage2D)(
          GLenum,
          GLint,
          GLint,
          GLsizei,
          GLsizei,
          GLint,
          GLenum,
          GLenum,
          const void*) = (const void (*)(GLenum,
                                         GLint,
                                         GLint,
                                         GLsizei,
                                         GLsizei,
                                         GLint,
                                         GLenum,
                                         GLenum,
                                         const void*))GetProcAddress(ogl_dll, "glTexImage2D");

        assert(nullptr != local_glTexImage2D);

        local_glTexImage2D(_target_in,
                           _level_in,
                           _internal_format_in,
                           _width_in,
                           _height_in,
                           _border_in,
                           _format_in,
                           _type_in,
                           _data_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_TexParameter(const GLenum&&              _target_in,
                      const GLenum&&              _parameter_name_in,
                      const GLint&& _parameter_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glTexParameteri)(GLenum, GLenum, GLuint) =
          (const void (*)(GLenum, GLenum, GLuint))GetProcAddress(ogl_dll, "glTexParameteri");

        assert(nullptr != local_glTexParameteri);

        local_glTexParameteri(_target_in, _parameter_name_in, _parameter_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_TexParameter(const GLenum&&                _target_in,
                      const GLenum&&                _parameter_name_in,
                      const GLfloat&& _parameter_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glTexParameterf)(GLenum, GLenum, GLfloat) =
          (const void (*)(GLenum, GLenum, GLfloat))GetProcAddress(ogl_dll, "glTexParameterf");

        assert(nullptr != local_glTexParameterf);

        local_glTexParameterf(_target_in, _parameter_name_in, _parameter_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_Uniform1f(const GLint&&             _location_in,
                   const GLfloat&& _float_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glUniform1f)(
          GLint,
          GLfloat) = (const void (*)(GLint, GLfloat))wglGetProcAddress("glUniform1f");

        assert(nullptr != local_glUniform1f);

        local_glUniform1f(_location_in, _float_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_UniformMatrix4fv(const GLint&&              _location_in,
                          const GLsizei&&            _count_in,
                          const bool&&               _should_transpose_in,
                          const GLfloat*&& _value_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (*local_glUniformMatrix4fv)(GLint, GLsizei, bool, const GLfloat*) =
          (const void (*)(GLint, GLsizei, bool, const GLfloat*))wglGetProcAddress(
            "glUniformMatrix4fv");

        assert(nullptr != local_glUniformMatrix4fv);

        local_glUniformMatrix4fv(_location_in, _count_in, _should_transpose_in, _value_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_UseProgram(const GLenum&& _program_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized                   = false;
        static const void (*local_glUseProgram)(GLenum) = (const void (*)(GLenum))wglGetProcAddress(
          "glUseProgram");

        assert(nullptr != local_glUseProgram);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        GLenum* program_id = &(state_cache->opengl_state->program_id);
        if (_program_in == *program_id && (true == state_initialized))
        {
            return;
        }

        local_glUseProgram(_program_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *program_id       = _program_in;
        state_initialized = true;
    }

    inline void
    Impl_VertexAttribPointer(const GLuint&&    _vertex_attribute_index_in,
                             const GLint&&     _component_count_in,
                             const GLenum&&    _data_type_in,
                             const GLboolean&& _should_be_normalized_by_gpu_in,
                             const GLsizei&&   _byte_stride_between_elements_in,
                             const GLvoid*&& _first_component_byte_offset_in
                               _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static const void (
          *local_glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*) =
          (const void (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))
            wglGetProcAddress("glVertexAttribPointer");

        assert(nullptr != local_glVertexAttribPointer);

        local_glVertexAttribPointer(_vertex_attribute_index_in,
                                    _component_count_in,
                                    _data_type_in,
                                    _should_be_normalized_by_gpu_in,
                                    _byte_stride_between_elements_in,
                                    _first_component_byte_offset_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);
    }

    inline void
    Impl_Viewport(const GLint&&              _lower_left_corner_x_in,
                  const GLint&&              _lower_left_corner_y_in,
                  const GLsizei&&            _width_in,
                  const GLsizei&& _height_in _DEBUG_FILE_AND_LINE_PARAMS_)
    {
        static bool state_initialized = false;
        static const void (*local_glViewport)(GLint, GLint, GLsizei, GLsizei) =
          (const void (*)(GLint, GLint, GLsizei, GLsizei))GetProcAddress(ogl_dll, "glViewport");

        assert(nullptr != local_glViewport);

        static state::StateCache* state_cache = state::StateCache::GetInstance();
        assert(nullptr != state_cache->opengl_state);

        glt::GLState::Viewport_t* viewport = &(state_cache->opengl_state->Viewport);
        if ((_lower_left_corner_x_in == viewport->lower_left_corner_x) &&
            (_lower_left_corner_y_in == viewport->lower_left_corner_y) &&
            (_width_in == viewport->width) && (_height_in == viewport->height) &&
            (true == state_initialized))
        {
            return;
        }

        local_glViewport(_lower_left_corner_x_in, _lower_left_corner_y_in, _width_in, _height_in);
        s_QueryErrors(_DEBUG_QUERY_ERRORS_ARGS);

        *viewport = { _lower_left_corner_x_in, _lower_left_corner_y_in, _width_in, _height_in };
        state_initialized = true;
    }

} // namespace glfn

#endif
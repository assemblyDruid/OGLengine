#ifndef gl_tools_h
#define gl_tools_h

// clang-format off
#include "pch.h"
// clang-format on

#include "gl_enum_tools.h"

namespace glt
{
    void
    GLDebugCallback(GLenum        _source_in,
                    GLenum        _type_in,
                    GLuint        _id_in,
                    GLenum        _severity_in,
                    GLsizei       _length_in,
                    const GLchar* _message_in,
                    void*         _user_params_in);

    struct GLState
    {
        struct ClearColor_t
        {
            float r;
            float g;
            float b;
            float a;
        } ClearColor;

        struct Version_t
        {
            int major_version;
            int minor_version;
        } Version;

        struct Viewport_t
        {
            GLint   lower_left_corner_x;
            GLint   lower_left_corner_y;
            GLsizei width;
            GLsizei height;
        } Viewport;

        // Note: The enabled_server_side_capabilities variable name is based off of
        //       the Khronos specification documentation for glEnable() and glDisable()
        std::unordered_set<GLenum> enabled_server_side_capabilities;

        GLenum cull_face_facet;
        GLenum depth_comparison_mode;
        GLenum front_face;
        GLuint program_id;

        // [ cfarvin::TODO ] Add items below this comment to DumpState
        GLenum active_texture;
        GLuint bound_buffers_by_target_type[buffer_target_array_length]   = { 0 };
        GLuint bound_textures_by_target_type[texture_target_array_length] = { 0 };
        GLuint currently_bound_vertex_array;
        GLuint currently_enabled_vertex_attribute_array;
    };

    // Note: `COUNT` enum int value determines size of `array_buffers` array in BufferStore.
    //       See declaration of `glt::array_buffer_count` and `glt::BufferStore::array_buffers`
    enum class VertexDataType
    {
        POSITION = 0,
        TEXTURE,
        NORMAL,
        COUNT
    };

    constexpr size_t array_buffer_count = static_cast<size_t>(VertexDataType::COUNT);

    struct VertexAttributeDescriptor
    {
        VertexAttributeDescriptor() = delete;
        VertexAttributeDescriptor(const GLuint&&         _vertex_array_object_id_in,
                                  const GLuint&&         _vertex_attribute_index_in,
                                  const GLint&&          _component_count_in,
                                  const GLenum&&         _data_type_in,
                                  const GLboolean&&      _should_be_normalized_by_gpu_in,
                                  const GLsizei&&        _byte_stride_between_elements_in,
                                  const GLvoid*&&        _first_component_byte_offset_in,
                                  const VertexDataType&& _vertex_data_type_in);

        const GLuint vertex_array_object_id;

        const GLuint    vertex_attribute_index;
        const GLint     component_count;
        const GLenum    data_type;
        const GLboolean should_be_normalized_by_gpu;
        const GLsizei   byte_stride_between_elements;
        const GLvoid*   first_component_byte_offset;

        const VertexDataType vertex_data_type;
    };

    // Binds a VAO in the constructor and binds 0 in the destructor such
    // that a VAO is only bound during the life/scope of this ojbect.
    // Note: This is only appropriate for simple situations where it is certain
    //       that no other binding will occur while this object is in scope.
    struct ScopedVaoBinding
    {
        ScopedVaoBinding() = delete;
        ScopedVaoBinding(const GLuint&& _vertex_array_object_id_in);

        ~ScopedVaoBinding();

      private:
        ScopedVaoBinding(const ScopedVaoBinding& _);

        ScopedVaoBinding
        operator=(const ScopedVaoBinding&);
    };

    // Binds a VBO in the constructor and binds 0 in the destructor such
    // that a VBO is only bound during the life/scope of this ojbect. VAOs are
    // unound before the VBO.
    // Note: This is only appropriate for simple situations where it is certain
    //       that no other binding will occur while this object is in scope.
    struct ScopedVboBinding
    {
        ScopedVboBinding() = delete;
        ScopedVboBinding(const GLenum&& _buffer_target_in, const GLuint&& _buffer_object_id_in);

        ~ScopedVboBinding();

      private:
        GLenum buffer_target;
        ScopedVboBinding(const ScopedVboBinding& _);

        ScopedVboBinding
        operator=(const ScopedVboBinding&);
    };

    // Binds a Texture in the constructor and binds 0 in the destructor such
    // that a texture is only bound during the life/scope of this ojbect.
    // Note: This is only appropriate for simple situations where it is certain
    //       that no other binding will occur while this object is in scope.
    struct ScopedTextureBinding
    {
        ScopedTextureBinding() = delete;
        ScopedTextureBinding(const GLenum&& _texture_target_in,
                             const GLuint&& _texture_object_id_in);

        ~ScopedTextureBinding();

      private:
        GLenum texture_target;
        ScopedTextureBinding(const ScopedTextureBinding& _);

        ScopedTextureBinding
        operator=(const ScopedTextureBinding&);
    };

    struct GLBufferInfo
    {
        GLuint  buffer_id;
        GLsizei target_buffer_element_count;
        GLenum  primitive_drawing_mode;
    };

    struct GLArrayBuffer
    {
        GLArrayBuffer() = delete;

        // Performance Note: Expect few elements, otherwise use unordered_map.
        std::vector<VertexAttributeDescriptor> vertex_attribute_pointers;
        const GLBufferInfo                     buffer_info;

      private:
        friend struct GLBufferStore;

        GLArrayBuffer(
          const GLuint&&                                _buffer_id_in,
          const GLsizei&&                               _target_buffer_element_count_in,
          const GLenum&&                                _primitive_drawing_mode_in,
          const std::vector<VertexAttributeDescriptor>& _glt_vertex_attribute_pointers_in) noexcept;

        GLArrayBuffer(const GLArrayBuffer&) noexcept;

        GLArrayBuffer
        operator=(const GLArrayBuffer&) noexcept;
    };

    struct GLElementBuffer
    {
        GLElementBuffer() = delete;

        const GLBufferInfo buffer_info;

      private:
        friend struct GLBufferStore;

        GLElementBuffer(const GLuint&&  _buffer_id_in,
                        const GLsizei&& _target_buffer_element_count_in,
                        const GLenum&&  _primitive_drawing_mode_in) noexcept;

        GLElementBuffer(const GLElementBuffer&) noexcept;

        GLElementBuffer
        operator=(const GLElementBuffer&) noexcept;
    };

    struct GLBufferStore
    {
        ~GLBufferStore();

        const GLuint
        AddArrayBuffer(
          bool&                                         _success_out,
          const GLuint&&                                _vao_in,
          const GLvoid* const                           _data_in,
          const GLsizeiptr&&                            _data_size_bytes_in,
          const GLsizei&&                               _target_buffer_element_count_in,
          const GLenum&&                                _target_buffer_usage_type_in,
          const GLenum&&                                _primitive_drawing_mode_in,
          const std::vector<VertexAttributeDescriptor>& _glt_vertex_attribute_pointers_in) noexcept;

        const GLuint
        AddElementBuffer(bool&               _success_out,
                         const GLuint&&      _vao_in,
                         const GLvoid* const _data_in,
                         const GLsizeiptr&&  _data_size_bytes_in,
                         const GLsizei&&     _target_buffer_element_count_in,
                         const GLenum&&      _target_buffer_usage_type_in,
                         const GLenum&&      _primitive_drawing_mode_in) noexcept;

        void
        GetBufferByAttributeIndex(const GLuint&&         _attribute_index_in,
                                  const VertexDataType&& _glt_vertex_data_type_in,
                                  GLArrayBuffer*&        _buffer_out) const noexcept;

        GLArrayBuffer*   array_buffer   = nullptr;
        GLElementBuffer* element_buffer = nullptr;
    };

    void
    CreateShaderProgram(bool&               _success_out,
                        const char* const&& _vertex_shader_file_path_in,
                        const char* const&& _fragment_shader_file_path_in,
                        GLuint&             _program_id_out);

    GLuint
    GetTexture2DFromImage(bool&               _success_out,
                          const char* const&& _file_path_in,
                          const bool&&        _attempt_ansiotropic_filtering_in);

    GLuint
    GetTestTextureRGB(
      bool&                _success_out,
      const char* const&&  _output_file_in, // Provide nullptr to skip writing to disk.
      const unsigned int&& _image_width_in  = 600,
      const unsigned int&& _image_height_in = 600) noexcept;

} // namespace glt
#endif
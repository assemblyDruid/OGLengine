#ifndef model_h
#define model_h

// clang-format off
#include "pch.h"
// clang-format on

#include "gl_tools.h"
#include "object3.h"

// [ cfarvin::REVISIT ] Should all of this go in a "model" namespace?

struct Model : protected Object3
{
    using Object3::GetOrientation;
    using Object3::GetPosition;
    using Object3::SetOrientation;
    using Object3::SetPosition;

  protected:
    glm::mat4 model_matrix;
};

// [ cfarvin::REVISIT ] Does this belong in glt? More general than OpenGL?
//                      Currently v_mat and p_mat are being stored in
//                      app_window::state (state_tools.h)
enum class MatrixType
{
    mtMODEL_MATRIX = 0,
    mtVIEW_MATRIX,
    mtPROJECTION_MATRIX,
    mtCOUNT
};
constexpr size_t matrix_type_count = static_cast<size_t>(MatrixType::mtCOUNT);

struct ProgramMatrixInfo
{
    GLuint rendering_program_id                       = 0;
    GLuint shader_layout_locations[matrix_type_count] = {};

    // Note: This member tracks the initialized state for each of the available MatrixType
    //       enum definitions. Default state is 0, indicating that no matrix types have
    //       been initialized. Bits at offsets defined by the integer values of the
    //       MatrixType enumeration are set to 1 when a matrix of that type is initialized.
    std::bitset<matrix_type_count> initialized_matrix_types = { 0 };
};

struct BufferedModel : Model
{
    void
    AddVertexArrayObject(bool& _success_out, GLuint& _vao_id_out);

    void
    AddMatrix(bool&              _success_out,
              const GLuint&&      _rendering_program_id_in,
              const MatrixType&& _matrix_type_in,
              const char* const  _matrix_uniform_name_in);

    GLuint
    AddArrayBuffer(
      bool&                                              _success_out,
      const GLvoid* const                                _data_in,
      const GLsizeiptr&&                                 _data_size_bytes_in,
      const GLsizei&&                                    _target_buffer_element_count_in,
      const GLenum&&                                     _target_buffer_usage_type_in,
      const GLenum&&                                     _primitive_drawing_mode_in,
      const std::vector<glt::VertexAttributeDescriptor>& _glt_vertex_attribute_pointers_in);

    GLuint
    AddElementBuffer(bool&               _success_out,
                     const GLvoid* const _data_in,
                     const GLsizeiptr&&  _data_size_bytes_in,
                     const GLsizei&&     _target_buffer_element_count_in,
                     const GLenum&&      _target_buffer_usage_type_in,
                     const GLenum&&      _primitive_drawing_mode_in);

    void
    EnableElementBuffer(bool& _success_out);

    void
    EnableVertexAttribute(bool& _success_out, const GLuint&& _attribute_index_in);

    // Note: Marked `const` and does not modify anything in the BufferedModel class.
    //       Though, it may modify values in the state cache.
    void
    ModifyMatrix(const GLuint&&     _rendering_program_id_in,
                 const MatrixType&& _matrix_type_in,
                 const glm::mat4&   _matrix_modifier_in) const noexcept;

    void
    Draw() const noexcept;

  protected:
    ProgramMatrixInfo*
    GetProgramMatrixInfoByRenderingProgramID(
      const GLuint&& _rendering_program_id_in) const noexcept;

    GLuint*            vertex_array_object = nullptr;
    glt::GLBufferStore buffer_store;

    std::vector<ProgramMatrixInfo> program_matrix_infos;

    ~BufferedModel();
};

struct TextureInfo
{
    TextureInfo() = delete;
    TextureInfo(const GLuint&& _texture_id_in,
                const GLuint&& _texture_coordinates_vbo_id_in,
                const GLuint&& _texture_coordinates_vertex_attribute_index_in,
                const GLenum&& _texture_target_in,
                const GLenum&& _texture_unit_in) noexcept;

    const GLuint texture_id;
    const GLuint texture_coordinates_vbo_id;
    const GLuint texture_coordinates_vertex_attribute_index;
    const GLenum texture_target;
    const GLenum texture_unit;
};

struct TexturedModel : BufferedModel
{
    void
    AddTexture(bool&             _success_out,
               const char* const _file_path_in,
               const GLuint&&    _vbo_id_in,
               const GLuint&&    _texture_coordinates_vertex_attribute_index_in,
               const GLenum&&    _texture_unit_in,
               const GLenum&&    _texture_target_buffer_type_in,
               GLuint&           _texture_id_out) noexcept;

    void
    EnableTexture(bool& _success_out, const GLuint&& _texture_id_in);

    void
    Draw() const noexcept;

  protected:
    std::vector<TextureInfo> added_textures;
    TextureInfo*             currently_enabled_texture;

  private:
    void
    GetTextureInfoByAttributeIndex(bool&          _success_out,
                                   const GLuint&& _attribute_index_in,
                                   TextureInfo*&  _texture_info_out) const;
};

// void
// LoadModelFromObjFile(bool&             _success_out,
//                      const char* const _obj_file_location);

void
LoadModelFromAsciiPlyFile(
  bool&                                        _success_out,
  const GLuint&&                               _vertex_array_object_id_in,
  const char* const                            _ascii_ply_file_path_in,
  const GLuint&&                               _position_attribute_index_in,
  const GLuint&&                               _normal_attribute_index_in,
  const GLuint&&                               _texture_coordinates_attribute_index_in,
  size_t&                                      _vertex_count_out,
  std::vector<float>&                          _buffer_data_out,
  std::vector<GLuint>&                         _buffer_elements_array_out,
  std::vector<glt::VertexDataType>&            _vertex_data_types_out,
  std::vector<glt::VertexAttributeDescriptor>& _vertex_attribute_pointers_out);

#endif
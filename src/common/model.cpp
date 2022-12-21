// clang-format off
#include "pch.h"
// clang-format on

#include "model.h"

#include "fileio.h"
#include "gl_function_wrappers.h"
#include "gl_tools.h"
#include "logging.h"

BufferedModel::~BufferedModel()
{
    if (nullptr != vertex_array_object)
    {
        delete vertex_array_object;
    }
}

void
BufferedModel::AddVertexArrayObject(bool& _success_out, GLuint& _vao_id_out)
{
    // Note: May expand functionality to enable multiple VAOs per object.
    if (nullptr != vertex_array_object)
    {
        _success_out = false;
        Log_e("A VAO was already bound for this object.");
        return;
    }

    glfn::GenVertexArrays(1, &_vao_id_out);
    vertex_array_object = new GLuint(_vao_id_out);
    _success_out        = true;
}

void
BufferedModel::AddModelMatrix(bool&             _success_out,
                              const glm::mat4&  _model_matrix_in,
                              const GLuint&     _rendering_program_id_in,
                              const char* const _matrix_uniform_name_in)
{
    if (false == glfn::IsProgram(_rendering_program_id_in))
    {
        _success_out = false;
        Log_e("Invalid rendering program; cannot add model matrix.");
        return;
    }

    glfn::UseProgram(_rendering_program_id_in);
    GLint layout_location = glfn::GetUniformLocation(_rendering_program_id_in,
                                                     _matrix_uniform_name_in);
    if (-1 == layout_location)
    {
        _success_out = false;
        std::stringstream ss;
        ss << "A uniform with name " << _matrix_uniform_name_in
           << " could not be found in the rendering program with id: " << _rendering_program_id_in
           << ". Could not add model matrix.";
        Log_e(ss);
        return;
    }

    model_matrix_info.model_matrix                 = _model_matrix_in;
    model_matrix_info.model_matrix_layout_location = layout_location;
    model_matrix_info.rendering_program_id         = _rendering_program_id_in;
}

GLuint
BufferedModel::AddArrayBuffer(
  bool&                                              _success_out,
  const GLvoid* const                                _data_in,
  const GLsizeiptr&&                                 _data_size_bytes_in,
  const GLsizei&&                                    _target_buffer_element_count_in,
  const GLenum&&                                     _target_buffer_usage_type_in,
  const GLenum&&                                     _primitive_drawing_mode_in,
  const std::vector<glt::VertexAttributeDescriptor>& _glt_vertex_attribute_pointers_in)
{
    if (nullptr == vertex_array_object)
    {
        _success_out = false;
        Log_e("No VAO was bound; cannot add array buffer.");
        return 0;
    }

    glt::ScopedVaoBinding scoped_vao_binding = glt::ScopedVaoBinding(
      std::move(*vertex_array_object));
    return buffer_store.AddArrayBuffer(_success_out,
                                       std::move(*vertex_array_object),
                                       _data_in,
                                       std::move(_data_size_bytes_in),
                                       std::move(_target_buffer_element_count_in),
                                       std::move(_target_buffer_usage_type_in),
                                       std::move(_primitive_drawing_mode_in),
                                       _glt_vertex_attribute_pointers_in);
}

GLuint
BufferedModel::AddElementBuffer(bool&               _success_out,
                                const GLvoid* const _data_in,
                                const GLsizeiptr&&  _data_size_bytes_in,
                                const GLsizei&&     _target_buffer_element_count_in,
                                const GLenum&&      _target_buffer_usage_type_in,
                                const GLenum&&      _primitive_drawing_mode_in)
{
    if (nullptr == vertex_array_object)
    {
        _success_out = false;
        Log_e("No VAO was bound; cannot add element buffer.");
        return 0;
    }

    glt::ScopedVaoBinding scoped_vao_binding = glt::ScopedVaoBinding(
      std::move(*vertex_array_object));
    return buffer_store.AddElementBuffer(_success_out,
                                         std::move(*vertex_array_object),
                                         _data_in,
                                         std::move(_data_size_bytes_in),
                                         std::move(_target_buffer_element_count_in),
                                         std::move(_target_buffer_usage_type_in),
                                         std::move(_primitive_drawing_mode_in));
}

void
BufferedModel::EnableElementBuffer(bool& _success_out)
{
    if (nullptr == vertex_array_object)
    {
        _success_out = false;
        Log_e("No VAO was bound; cannot enable element buffer.");
        return;
    }
    else if (nullptr == buffer_store.element_buffer)
    {
        _success_out = false;
        Log_e("No element buffer was added; cannot enable.");
        return;
    }

    glt::ScopedVaoBinding scoped_vao_binding = glt::ScopedVaoBinding(
      std::move(*vertex_array_object));
    glt::ScopedVboBinding scoped_element_buffer_binding = glt::ScopedVboBinding(
      std::move(GL_ELEMENT_ARRAY_BUFFER),
      std::move(buffer_store.element_buffer->buffer_info.buffer_id));

    _success_out = true;
}

void
BufferedModel::EnableVertexAttribute(bool& _success_out, const GLuint&& _attribute_index_in)
{
    if (nullptr == vertex_array_object)
    {
        _success_out = false;
        Log_e("No VAO was bound; cannot enable vertex attribute.");
        return;
    }

    glt::ScopedVaoBinding scoped_vao_binding = glt::ScopedVaoBinding(
      std::move(*vertex_array_object));

    bool vertex_attribute_enabled = false;
    for (auto& buffer_store_vert_attrib_desc : buffer_store.array_buffer->vertex_attribute_pointers)
    {
        if ((_attribute_index_in == buffer_store_vert_attrib_desc.vertex_attribute_index) &&
            (*vertex_array_object == buffer_store_vert_attrib_desc.vertex_array_object_id))
        {
            glfn::BindBuffer(GL_ARRAY_BUFFER, buffer_store.array_buffer->buffer_info.buffer_id);
            glfn::VertexAttribPointer(buffer_store_vert_attrib_desc.vertex_attribute_index,
                                      buffer_store_vert_attrib_desc.component_count,
                                      buffer_store_vert_attrib_desc.data_type,
                                      buffer_store_vert_attrib_desc.should_be_normalized_by_gpu,
                                      buffer_store_vert_attrib_desc.byte_stride_between_elements,
                                      buffer_store_vert_attrib_desc.first_component_byte_offset);
            glfn::EnableVertexAttribArray(buffer_store_vert_attrib_desc.vertex_attribute_index);

            vertex_attribute_enabled = true;
            break;
        }
    }

    if (false == vertex_attribute_enabled)
    {
        std::stringstream ss;
        ss << "Could not find VertexAttributeDescriptor with vertex_attribute_index: "
           << _attribute_index_in << " and vertex_array_object_id: " << *vertex_array_object
           << " in the buffer store.";
        Log_e(ss);
        _success_out = false;
        return;
    }

    _success_out = true;
}

void
BufferedModel::ModifyModelMatrix(const glm::mat4& _matrix_modifier_in)
{
    // [ cfarvin::TODO ] You are not confident in this.
    static const state::StateCache* const state_cache = state::StateCache::GetInstance();
    assert(nullptr != state_cache->opengl_state);
    glfn::UseProgram(state_cache->opengl_state->program_id);

    glfn::UniformMatrix4fv(model_matrix_info.model_matrix_layout_location,
                           1,
                           false,
                           glm::value_ptr(_matrix_modifier_in));

    // [ cfarvin::TODO ]
    // Should we go through all of these functions an do things like:
    // glfn::UseProgram(0); ??
}

void
BufferedModel::Draw() const noexcept
{
    assert(nullptr != buffer_store.array_buffer);
    assert(nullptr != buffer_store.element_buffer);
    assert(nullptr != vertex_array_object);

    // [ cfarvin::TODO ]
    // Right now we're not binding a gl program. Das good?

    // Note: Avoiding scoped binding helper objects in draw loop.
    glfn::BindVertexArray(*vertex_array_object);
    glfn::DrawElements(buffer_store.element_buffer->buffer_info.primitive_drawing_mode,
                       buffer_store.element_buffer->buffer_info.target_buffer_element_count,
                       GL_UNSIGNED_INT,
                       (void*)0);
    glfn::BindVertexArray(0);
}

TextureInfo::TextureInfo(const GLuint&& _texture_id_in,
                         const GLuint&& _texture_coordinates_vbo_id_in,
                         const GLuint&& _texture_coordinates_vertex_attribute_index_in,
                         const GLenum&& _texture_target_buffer_type_in,
                         const GLenum&& _texture_unit_in) noexcept
  : texture_id(_texture_id_in)
  , texture_coordinates_vbo_id(_texture_coordinates_vbo_id_in)
  , texture_coordinates_vertex_attribute_index(_texture_coordinates_vertex_attribute_index_in)
  , texture_target(_texture_target_buffer_type_in)
  , texture_unit(_texture_unit_in)
{
}

void
TexturedModel::AddTexture(bool&             _success_out,
                          const char* const _file_path_in,
                          const GLuint&&    _vbo_id_in,
                          const GLuint&&    _texture_coordinates_vertex_attribute_index_in,
                          const GLenum&&    _texture_unit_in,
                          const GLenum&&    _texture_target_buffer_type_in,
                          GLuint&           _texture_id_out) noexcept
{
    _success_out = false;

    // Note: glt::GetTexture2DFromImage handles nullptr file path.
    _texture_id_out = glt::GetTexture2DFromImage(_success_out, std::move(_file_path_in), true);
    if (false == _success_out)
    {
        std::stringstream ss;
        ss << "Unable to load texture at: " << _file_path_in;
        Log_e(ss);
        return;
    }

    bool is_unique = true;
    for (size_t index = 0; index < added_textures.size(); index++)
    {
        if (added_textures[index].texture_id == _texture_id_out) is_unique = false;
    }

    if (false == is_unique)
    {
        Log_e("A non-unique texture ID was generated.");
        glfn::DeleteTextures(1, &_texture_id_out);
        return;
    }

    added_textures.push_back(TextureInfo(std::move(_texture_id_out),
                                         std::move(_vbo_id_in),
                                         std::move(_texture_coordinates_vertex_attribute_index_in),
                                         std::move(_texture_target_buffer_type_in),
                                         std::move(_texture_unit_in)));
    _success_out = true;
}

void
TexturedModel::EnableTexture(bool& _success_out, const GLuint&& _texture_id_in)
{
    // Ensure texture exists in added textures.
    TextureInfo* texture_info = nullptr;
    {
        for (auto& texture : added_textures)
        {
            if (_texture_id_in == texture.texture_id)
            {
                texture_info = &texture;
                break;
            }
        }

        if (nullptr == texture_info)
        {
            std::stringstream ss;
            ss << "The texture with id " << _texture_id_in << " was not found.";
            Log_e(ss);
            _success_out = false;
            return;
        }
    }

    currently_enabled_texture = texture_info;
    _success_out              = true;
}

void
TexturedModel::Draw() const noexcept
{
    assert(nullptr != buffer_store.array_buffer);
    assert(nullptr != buffer_store.element_buffer);
    assert(nullptr != vertex_array_object);

    // Note: Avoiding scoped binding helper objects in draw loop.
    glfn::BindVertexArray(*vertex_array_object);

    static const state::StateCache* const state_cache = state::StateCache::GetInstance();
    assert(nullptr != state_cache->opengl_state);
    glfn::UseProgram(state_cache->opengl_state->program_id);

    if (nullptr != currently_enabled_texture)
    {
        glfn::ActiveTexture(currently_enabled_texture->texture_unit);

        // Note: Texture does not get unbound in this loop.
        glfn::BindTexture(currently_enabled_texture->texture_target,
                          currently_enabled_texture->texture_id);
    }

    glfn::DrawElements(buffer_store.element_buffer->buffer_info.primitive_drawing_mode,
                       buffer_store.element_buffer->buffer_info.target_buffer_element_count,
                       GL_UNSIGNED_INT,
                       (void*)0);

    glfn::BindVertexArray(0);
}

// void
// LoadModelFromObjFile(bool&             _success_out,
//                      const char* const _obj_file_path_in,
//                      ImportedModel&    _model_in)
// {
//     Log_w("LoadModelFromObjFile(...) Does not account for duplicate vertices, texture coordinates, "
//           "or normal data in faces. If possible, try using another 3D mesh file format which"
//           " accounts for this issue. The Stanford Triangle Format, \".ply\" is currently"
//           " favored in this codebase.");

//     _success_out = true;
//     std::stringstream ss;
//     std::stringstream sub_ss;

//     size_t      file_data_size_bytes;
//     std::string file_data;
//     LoadFileToStdString(_success_out, _obj_file_path_in, file_data_size_bytes, file_data);
//     if ((false == _success_out) || (0 == file_data_size_bytes))
//     {
//         Log_e("Could not load object file.");
//         return;
//     }

//     // Object File Prefixes:
//     // -------------------------------------
//     // 'v':  Geometric vertex location.
//     // 'vt': Texture Coordinates.
//     // 'vn': Vertex normals.
//     // 'f':  Face.

//     const char* const      delimeter         = "\n";
//     const char* const      sub_delimeters    = "/ ";
//     char*                  token             = std::strtok(file_data.data(), delimeter);
//     char*                  sub_token         = nullptr;
//     size_t                 char_line_offset  = 0;
//     size_t                 char_total_offset = 0;
//     size_t                 line_number       = 1;
//     std::vector<glm::vec2> texture_coordinates;
//     std::vector<glm::vec3> vertices;
//     std::vector<glm::vec3> normals;
//     std::vector<glm::vec3> faces;

//     auto report_parse_error = [&](const char* const _note = nullptr)
//     {
//         ss << "Failed to parse model object file." << std::endl
//            << "   Object File: " << _obj_file_path_in << std::endl
//            << "   Line Number: " << line_number << std::endl
//            << "   Line Data:   " << token;

//         if (nullptr != _note)
//         {
//             ss << std::endl << "   Note:        " << _note;
//         }

//         Log_e(ss);
//         _success_out = false;
//     };

//     // Note: Incrememnts the `char_line_offset` variable by +1.
//     auto ensure_next_space = [&]()
//     {
//         char previous_char = token[char_line_offset];
//         if (' ' != token[char_line_offset++])
//         {
//             sub_ss << "Expected a space after `" << previous_char << "` at line offset "
//                    << char_line_offset - 1;
//             report_parse_error(sub_ss.str().c_str());
//         }
//     };

//     // Note: Using float vector as general container, will cast to unsigned int
//     //       for faces/element indexes. Verticies, normals, and texture coordinates
//     //       remain floats. Container reserved 12 spaces as the largest data group per
//     //       line in the .obj file are the "faces" lines, having 4 groups of 3.
//     std::vector<float> elements;
//     elements.reserve(12); // Largest data group is "faces", with 12 elements.
//     elements.shrink_to_fit();

//     float temp_float;
//     char* strtof_ending = nullptr;

//     auto deserialize_core_impl = [&]()
//     {
//         elements.clear();
//         sub_token = std::strtok(&(token[char_line_offset]), sub_delimeters);
//         while (nullptr != sub_token)
//         {
//             temp_float = std::strtof(sub_token, &strtof_ending);
//             if (0 == strtof_ending)
//             {
//                 sub_ss << "Could not parse as float: " << sub_token;
//                 report_parse_error(sub_ss.str().c_str());
//             }

//             elements.push_back(temp_float);
//             sub_token = std::strtok(nullptr, sub_delimeters);
//         }
//     };

//     auto deserialize_to_vec2 = [&](std::vector<glm::vec2>& _target_buffer_type_in)
//     {
//         deserialize_core_impl();
//         if (2 != elements.size())
//         {
//             sub_ss << "Expected 2 elements, got " << elements.size();
//             report_parse_error(sub_ss.str().c_str());
//         }
//         else
//         {
//             _target_buffer_type_in.push_back(glm::vec2(elements[0], elements[1]));
//         }
//     };

//     auto deserialize_to_vec3 = [&](std::vector<glm::vec3>& _target_buffer_type_in)
//     {
//         deserialize_core_impl();
//         if (&_target_buffer_type_in == &faces)
//         {
//             if (9 != elements.size())
//             {
//                 sub_ss << "Expected 9 elements, got " << elements.size()
//                        << ". Was \"Triangulated Mesh\" selected on object export?";
//                 report_parse_error(sub_ss.str().c_str());
//             }
//             else
//             {
//                 _target_buffer_type_in.push_back(glm::vec3(elements[0], elements[1], elements[2]));
//                 _target_buffer_type_in.push_back(glm::vec3(elements[3], elements[4], elements[5]));
//                 _target_buffer_type_in.push_back(glm::vec3(elements[6], elements[7], elements[8]));
//             }
//         }
//         else
//         {
//             if (3 != elements.size())
//             {
//                 sub_ss << "Expected 3 elements, got " << elements.size();
//                 report_parse_error(sub_ss.str().c_str());
//             }
//             else
//             {
//                 _target_buffer_type_in.push_back(glm::vec3(elements[0], elements[1], elements[2]));
//             }
//         }
//     };

//     while (nullptr != token)
//     {
//         if (false == _success_out) return;

//         char_line_offset = 0;
//         char_total_offset += strlen(token) + 1;

//         switch (token[char_line_offset++])
//         {
//             case '#': // Skip comments.
//             case 's': // Skip smoothing for now.
//             case 'o': // Skip object name for now.
//             {
//                 break;
//             }
//             case 'v':
//             {
//                 switch (token[char_line_offset++])
//                 {
//                     case ' ': // Geometric vertex location. ("v ")
//                     {
//                         deserialize_to_vec3(vertices);
//                         break;
//                     }
//                     case 't': // Texture coordinates. ("vt  ")
//                     {
//                         ensure_next_space();
//                         deserialize_to_vec2(texture_coordinates);
//                         break;
//                     }
//                     case 'n': // Normal coordinates. ("vn ")
//                     {
//                         ensure_next_space();
//                         deserialize_to_vec3(normals);
//                         break;
//                     }
//                     default:
//                     {
//                         report_parse_error();
//                     }
//                 }
//                 break;
//             }
//             case 'f': // Face. ("f ")
//             {
//                 ensure_next_space();
//                 deserialize_to_vec3(faces);
//                 break;
//             }
//             default:
//             {
//                 sub_ss << "Unexpected start-of-line: \"" << token[char_line_offset - 1] << "\"";
//                 report_parse_error(sub_ss.str().c_str());
//             }
//         }

//         token = std::strtok((file_data.data() + char_total_offset), delimeter);
//         line_number++;
//     }

//     _model_in.AddBuffers(_success_out, texture_coordinates, vertices, normals, faces);
//     if (false == _success_out)
//     {
//         Log_e("Model load failed due to errors in buffer addition.");
//         return;
//     }
// }

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
  std::vector<glt::VertexAttributeDescriptor>& _vertex_attribute_pointers_out)
{
    _success_out = true;

    size_t      file_data_size_bytes;
    std::string file_data;
    LoadFileToStdString(_success_out, _ascii_ply_file_path_in, file_data_size_bytes, file_data);
    if ((false == _success_out) || (0 == file_data_size_bytes))
    {
        Log_e("Could not load .ply object file.");
        return;
    }

    const std::string ply_tag = "ply";
    const std::string test    = file_data.substr(0, 3);
    if (test != ply_tag)
    {
        Log_e("Invalid .ply file.");
        _success_out = false;
        return;
    }

    const std::string format_tag                    = "format";
    const std::string ascii_tag                     = "ascii";
    const std::string version_tag                   = "1.0";
    const std::string comment_tag                   = "comment";
    const std::string element_tag                   = "element";
    const std::string end_header_tag                = "end_header";
    const std::string property_tag                  = "property";
    const std::string vertex_tag                    = "vertex";
    const std::string x_tag                         = "x";
    const std::string y_tag                         = "y";
    const std::string z_tag                         = "z";
    const std::string nx_tag                        = "nx";
    const std::string ny_tag                        = "ny";
    const std::string nz_tag                        = "nz";
    const std::string s_tag                         = "s";
    const std::string t_tag                         = "t";
    const std::string float_tag                     = "float";
    const std::string face_tag                      = "face";
    const std::string list_tag                      = "list";
    const std::string uchar_uint_vertex_indices_tag = "uchar uint vertex_indices";
    const char* const strtok_delim                  = " ";

    // Note: If more types are added, ensure to cover additions in the
    //       switch for that data type as well as the stringify lambda switch below.
    enum class ElementTypes
    {
        NONE = 0,
        VERTEX,
        FACE
    };

    auto stringify_element_type = [](ElementTypes _type_in, std::string& _string_out)
    {
        switch (_type_in)
        {
            case ElementTypes::VERTEX:
            {
                _string_out = "ElementTypes::VERTEX";
                break;
            }
            case ElementTypes::FACE:
            {
                _string_out = "ElementTypes::FACE";
                break;
            }
            case ElementTypes::NONE:
            {
                _string_out = "ElementTypes::NONE";
                break;
            }
        }
    };

    std::istringstream       file_data_stream(file_data);
    std::string              file_data_line;
    std::vector<std::string> file_data_line_string_elements;
    std::vector<float>       file_data_line_float_elements;
    std::stringstream        ss;
    size_t                   file_line_number = 0;

    auto split_line_to_strings = [](const std::string&        _line_in,
                                    const char* const         _delimeter_in,
                                    std::vector<std::string>& _target_buffer_type_in_out)
    {
        _target_buffer_type_in_out.clear();
        std::string line_copy = _line_in;
        const char* token     = std::strtok(line_copy.data(), _delimeter_in);

        while (nullptr != token)
        {
            _target_buffer_type_in_out.push_back(std::string(token));
            token = std::strtok(nullptr, _delimeter_in);
        }
    };

    // Note: sets _success_out to false, exiting loops in implementations below.
    auto report_parse_error = [&](const char* const _note = nullptr)
    {
        std::stringstream parse_error_ss;
        parse_error_ss << "Failed to parse model PLY file." << std::endl
                       << "   PLY File:    " << _ascii_ply_file_path_in << std::endl
                       << "   Line Number: " << file_line_number << std::endl
                       << "   Line Data:   " << file_data_line;

        if (nullptr != _note)
        {
            parse_error_ss << std::endl << "   Note:        " << _note;
        }

        Log_e(parse_error_ss);
        _success_out = false;
    };

    // Note: sets _success_out to false, exiting loops in implementations below.
    auto report_invalid_string_sequence = [&]()
    {
        ss.clear();
        ss << "Invalid string sequence: " + file_data_line_string_elements[0];
        report_parse_error(ss.str().c_str());
    };

    auto convert_string_to_data_size_bytes_in_t =
      [](const char* const _string_in, size_t& _data_size_bytes_in_t_out)
    {
        char*         strtoull_ending = nullptr;
        constexpr int base            = 10;
        _data_size_bytes_in_t_out     = std::strtoull(_string_in, &strtoull_ending, base);

        if (0 == strtoull_ending)
        {
            _data_size_bytes_in_t_out = 0; // Indicates error state to caller.
        }
    };

    auto convert_strings_to_floats =
      [](const std::vector<std::string>& _strings_in, std::vector<float>& _floats_out)
    {
        _floats_out.clear();
        char* strtof_ending = nullptr;
        float temp_float;
        for (auto& str : _strings_in)
        {
            temp_float = std::strtof(str.c_str(), &strtof_ending);

            if (0 == strtof_ending)
            {
                _floats_out.clear(); // Indicates error state to caller.
                return;
            }

            _floats_out.push_back(temp_float);
        }
    };

    auto update_element_count_and_layout_order =
      [](char&                             _element_count_in,
         bool&                             _vertex_type_bool_in,
         std::vector<glt::VertexDataType>& _vertex_data_type_target_buffer_in,
         glt::VertexDataType&&             _vertex_data_type_in)
    {
        _element_count_in++;

        if (false == _vertex_type_bool_in)
        {
            _vertex_data_type_target_buffer_in.push_back(_vertex_data_type_in);
        }
        _vertex_type_bool_in = true;
    };

    size_t vertex_count                       = 0;
    char   vertex_component_count             = 0;
    char   normal_component_count             = 0;
    char   texture_coordinate_component_count = 0;
    size_t face_count                         = 0;

    std::vector<ElementTypes>        file_element_layout_order;
    std::vector<glt::VertexDataType> vertex_data_type_layout_order;
    std::vector<float>               model_data_in;
    std::vector<GLuint>              model_elements;

    // Parse Header
    {
        bool         vertices_header_processed                 = false;
        bool         faces_header_processed                    = false;
        bool         header_complete                           = false;
        bool         face_properties_set                       = false;
        bool         vertex_position_data_type_added           = false;
        bool         normal_vertex_data_type_added             = false;
        bool         texture_coordinate_vertex_data_type_added = false;
        ElementTypes current_element_type                      = ElementTypes::NONE;
        while ((false == header_complete) && (true == _success_out) &&
               (std::getline(file_data_stream, file_data_line)))
        {
            file_line_number++;
            split_line_to_strings(file_data_line, strtok_delim, file_data_line_string_elements);

            switch (file_data_line_string_elements[0][0]) // Switch by 1st char of 1st string.
            {
                case 'c':
                {
                    if (comment_tag != file_data_line_string_elements[0]) // COMMENT TAG
                    {
                        report_invalid_string_sequence();
                    }

                    break;
                }
                case 'p':
                {
                    if (ply_tag == file_data_line_string_elements[0]) // PLY TAG
                    {
                        if (1 != file_data_line_string_elements.size() || 1 != file_line_number)
                        {
                            ss.clear();
                            ss << "Invalid line containing: " + ply_tag;
                            report_parse_error(ss.str().c_str());
                        }
                    }
                    else if (property_tag == file_data_line_string_elements[0]) // PROPERTY TAG
                    {
                        switch (current_element_type)
                        {
                            case ElementTypes::VERTEX: // VERTEX PROPERTIES
                            {
                                if ((3 != file_data_line_string_elements.size()) ||
                                    (float_tag != file_data_line_string_elements[1]))
                                {
                                    report_parse_error(
                                      "Vertex property descriptions must be floats and defined on "
                                      "lines containing exactly three words.");
                                    break;
                                }

                                if ((x_tag == file_data_line_string_elements[2]) ||
                                    (y_tag == file_data_line_string_elements[2]) ||
                                    (z_tag == file_data_line_string_elements[2]))
                                {
                                    update_element_count_and_layout_order(
                                      vertex_component_count,
                                      vertex_position_data_type_added,
                                      vertex_data_type_layout_order,
                                      glt::VertexDataType::POSITION);
                                }
                                else if ((nx_tag == file_data_line_string_elements[2]) ||
                                         (ny_tag == file_data_line_string_elements[2]) ||
                                         (nz_tag == file_data_line_string_elements[2]))
                                {
                                    update_element_count_and_layout_order(
                                      normal_component_count,
                                      normal_vertex_data_type_added,
                                      vertex_data_type_layout_order,
                                      glt::VertexDataType::NORMAL);
                                }
                                else if ((s_tag == file_data_line_string_elements[2]) ||
                                         (t_tag == file_data_line_string_elements[2]))
                                {
                                    update_element_count_and_layout_order(
                                      texture_coordinate_component_count,
                                      texture_coordinate_vertex_data_type_added,
                                      vertex_data_type_layout_order,
                                      glt::VertexDataType::TEXTURE);
                                }
                                else
                                {
                                    report_parse_error("Invalid vertex property description.");
                                }

                                break;
                            }
                            case ElementTypes::FACE: // FACE PROPERTIES
                            {
                                if (true == face_properties_set)
                                {
                                    report_parse_error(
                                      "Face properties can be defined exactly once.");
                                    break;
                                }
                                face_properties_set = true;

                                const std::string comparison_test_a =
                                  file_data_line_string_elements[1] + // No space is intentional.
                                  file_data_line_string_elements[2] + " " +
                                  file_data_line_string_elements[3] + " " +
                                  file_data_line_string_elements[4];

                                const std::string comparison_test_b = list_tag +
                                                                      uchar_uint_vertex_indices_tag;

                                if ((5 != file_data_line_string_elements.size()) ||
                                    (comparison_test_a != comparison_test_b))
                                {
                                    report_parse_error("Invalid face property description.");
                                }

                                // Note: Above logic enforces a single element face property description with
                                //       exactly the following text:
                                //
                                //       "property list uchar uint vertex_indices"

                                break;
                            }
                            default:
                            {
                                report_parse_error("Property tags were found without first listing "
                                                   "an element type.");
                            }
                        }
                    }
                    else
                    {
                        report_invalid_string_sequence();
                    }

                    break;
                }
                case 'f':
                {
                    if ((3 == file_data_line_string_elements.size()) &&
                        (format_tag == file_data_line_string_elements[0])) // FORMAT TAG
                    {
                        if ((ascii_tag != file_data_line_string_elements[1]) &&
                            (version_tag != file_data_line_string_elements[2]))
                        {
                            report_parse_error(
                              "Unsupported file type or version. Must be ascii version 1.0");
                        }
                    }
                    else
                    {
                        report_invalid_string_sequence();
                    }

                    break;
                }
                case 'e':
                {
                    if ((3 == file_data_line_string_elements.size()) &&
                        (element_tag == file_data_line_string_elements[0])) // ELEMENT TAG
                    {
                        if (vertex_tag == file_data_line_string_elements[1]) // VERTEX TAG
                        {
                            if (true == vertices_header_processed)
                            {
                                report_parse_error(
                                  "Vertex header information can be processed exactly once.");
                                break;
                            }

                            convert_string_to_data_size_bytes_in_t(
                              file_data_line_string_elements[2].c_str(),
                              vertex_count);

                            if (0 == vertex_count)
                            {
                                report_parse_error("Unable to determine vertex count.");
                                break;
                            }

                            current_element_type = ElementTypes::VERTEX;
                            file_element_layout_order.push_back(ElementTypes::VERTEX);
                            vertices_header_processed = true;
                        }
                        else if (face_tag == file_data_line_string_elements[1]) // FACE TAG
                        {
                            if (true == faces_header_processed)
                            {
                                report_parse_error(
                                  "Faces header information can be processed exactly once.");
                                break;
                            }

                            convert_string_to_data_size_bytes_in_t(
                              file_data_line_string_elements[2].c_str(),
                              face_count);

                            if (0 == face_count)
                            {
                                report_parse_error("Unable to determine face count.");
                                break;
                            }

                            current_element_type = ElementTypes::FACE;
                            file_element_layout_order.push_back(ElementTypes::FACE);
                            faces_header_processed = true;
                        }
                    }
                    else if (end_header_tag == file_data_line_string_elements[0]) // END HEADER TAG
                    {
                        if (3 != vertex_component_count)
                        {
                            report_parse_error(
                              "Vertex coordinates require exactly three positional components.");
                            break;
                        }

                        if (3 != normal_component_count)
                        {
                            report_parse_error("Normal vectors require exactly three components.");
                            break;
                        }

                        if (2 != texture_coordinate_component_count)
                        {
                            report_parse_error(
                              "Texture coordinates require exactly two positional components.");
                            break;
                        }

                        if (0 == vertex_count)
                        {
                            report_parse_error("At least one vertex is required.");
                            break;
                        }

                        if (0 == face_count)
                        {
                            report_parse_error("At least one face is required.");
                            break;
                        }

                        model_data_in.resize(
                          vertex_count * (vertex_component_count + normal_component_count +
                                          texture_coordinate_component_count),
                          0.0f);

                        //
                        // VISIBILITY: Reservation size for the element array is the face count multiplied
                        //             by three since this importer requires that exported meshes are
                        //             triangulated. There are checks in the data import section which will
                        //             throw errors for non-triangulated meshes (containing quads or N-gons).
                        //
                        model_elements.resize(face_count * 3, 0);

                        header_complete = true;
                    }
                    else
                    {
                        report_invalid_string_sequence();
                    }

                    break;
                }
            }
        }
    }

    //
    // [ cfarvin::TODO ] Move to a function, to be seen in the header later if needed.
    // Debug: Neatly prints pivotal data from the .ply header.
    //
    {
        // std::stringstream header_debug_ss;
        // header_debug_ss << "Successfully loaded PLY header" << std::endl
        //                 << "   File:              " << _ascii_ply_file_path_in << std::endl
        //                 << "   Vertices:          " << vertex_count << std::endl
        //                 << "   Position elements: " << static_cast<size_t>(vertex_component_count)
        //                 << std::endl
        //                 << "   Normal elements:   " << static_cast<size_t>(normal_component_count)
        //                 << std::endl
        //                 << "   Texture elements:  "
        //                 << static_cast<size_t>(texture_coordinate_component_count) << std::endl
        //                 << "   Face count:        " << face_count;

        // Log_i(header_debug_ss);
    }

    // Note: Pops one line off of the input stream and increments the line number.
    auto read_next_data_in_line =
      [&](bool& _should_break_calling_loop, const std::string& _data_in_type)
    {
        _should_break_calling_loop = false;

        //
        // Visibility: GETLINE
        //
        std::getline(file_data_stream, file_data_line);
        file_line_number++;

        if (0 == file_data_line.size())
        {
            ss.clear();
            ss << "Expected more " << _data_in_type << " information.";
            report_parse_error(ss.str().c_str());
            _should_break_calling_loop = true;
            return;
        }

        split_line_to_strings(file_data_line, strtok_delim, file_data_line_string_elements);
        convert_strings_to_floats(file_data_line_string_elements, file_data_line_float_elements);
        if (0 == file_data_line_float_elements.size())
        {
            ss.clear();
            ss << "Unable to read or convert " << _data_in_type << " information.";
            report_parse_error(ss.str().c_str());
            _should_break_calling_loop = true;
        }
    };

    const size_t expected_vertex_attribute_components = vertex_component_count +
                                                        normal_component_count +
                                                        texture_coordinate_component_count;
    // Parse Data
    {
        if (end_header_tag != file_data_line)
        {
            Log_e("Could not move on to parsing data; 'end_header' was not the last line read.");
            _success_out = false;
            return;
        }

        bool vertices_data_in_processed = false;
        bool faces_data_in_processed    = false;
        for (auto& element_type : file_element_layout_order)
        {
            if (false == _success_out) break;

            switch (element_type)
            {
                case (ElementTypes::VERTEX):
                {
                    if (true == vertices_data_in_processed)
                    {
                        report_parse_error("Vertices can be processed exactly once.");
                        break;
                    }

                    size_t vertex_insertion_index = 0;
                    for (size_t vertex_index = 0; vertex_index < vertex_count; vertex_index++)
                    {
                        bool break_loop;
                        read_next_data_in_line(break_loop, "vertex");
                        if (true == break_loop)
                        {
                            _success_out = false;
                            break;
                        }

                        if (expected_vertex_attribute_components !=
                            file_data_line_float_elements.size())
                        {
                            ss.clear();
                            ss
                              << "Unexpected quantity of vertex attribute elements found. Expected "
                              << expected_vertex_attribute_components << ", got "
                              << file_data_line_float_elements.size();
                            report_parse_error(ss.str().c_str());
                            break;
                        }

                        for (auto& vertex_attribute_element : file_data_line_float_elements)
                        {
                            model_data_in[vertex_insertion_index++] = vertex_attribute_element;
                        }
                    }

                    vertices_data_in_processed = true;
                    break;
                }
                case (ElementTypes::FACE):
                {
                    if (true == faces_data_in_processed)
                    {
                        report_parse_error("Faces can be processed exactly once.");
                        break;
                    }

                    size_t face_insertion_index = 0;
                    while (face_insertion_index < model_elements.size())
                    {
                        bool break_loop;
                        read_next_data_in_line(break_loop, "face");
                        if (true == break_loop)
                        {
                            _success_out = false;
                            break;
                        }

                        if ((4 != file_data_line_float_elements.size()) &&
                            (3 != static_cast<size_t>(file_data_line_float_elements[0])))
                        {
                            report_parse_error(
                              "Faces must have exactly 3 elements. Ensure that meshes are "
                              "triangulated before export.");
                            break;
                        }

                        // Note: Ignore first element, which specifies count of elements to follow.
                        for (size_t face_element_index = 1;
                             face_element_index < file_data_line_float_elements.size();
                             face_element_index++)
                        {
                            model_elements[face_insertion_index++] = static_cast<GLuint>(
                              file_data_line_float_elements[face_element_index]);
                        }
                    }

                    faces_data_in_processed = true;
                    break;
                }
                default:
                {
                    ss.clear();
                    std::string stringified_element_type;
                    stringify_element_type(element_type, stringified_element_type);
                    ss << "Unidentified element type specified for current data line. Received: "
                       << stringified_element_type;
                    report_parse_error(ss.str().c_str());
                }
            }
        }

        if ((true != faces_data_in_processed) && (true != vertices_data_in_processed))
        {
            report_parse_error("Both vertices and faces must be imported from model files.");
            _success_out = false;
            return;
        }

        //
        // [ cfarvin::TODO ] Move to a function, to be seen in the header later if needed.
        // Debug: Prints the vertices and face data as would be
        //        seen in the data section object file on disk.
        //
        {
            // size_t c               = 0;
            // size_t newline_counter = 1;

            // std::stringstream model_data_in_test;
            // model_data_in_test << std::endl;

            // size_t newline_on = 9;
            // for (size_t i = 0; i < model_data_in.size(); i++)
            // {
            //     model_data_in_test << model_data_in[c++] << " ";
            //     newline_counter++;
            //     if (newline_counter == newline_on)
            //     {
            //         newline_counter = 1;
            //         model_data_in_test << std::endl;
            //     }
            // }

            // c               = 0;
            // newline_on      = 4;
            // newline_counter = 1;
            // for (size_t i = 0; i < model_elements.size(); i++)
            // {
            //     if (newline_counter == 1)
            //     {
            //         model_data_in_test << "3 ";
            //     }

            //     model_data_in_test << model_elements[c++] << " ";
            //     newline_counter++;

            //     if (newline_counter == newline_on)
            //     {
            //         newline_counter = 1;
            //         model_data_in_test << std::endl;
            //     }
            // }

            // Log_i(model_data_in_test);
        }
    }

    // Write data to output buffers.
    {
        // Create vertex attributes
        {
            //
            // VISIBILITY: This calculation assumes that all the data is of type float.
            //
            //             This works exclusively for layouts like:
            //             float | float | float | float | float | float | float | float
            //             p.x,  | p.y,  | p.z,  | n.x,  | n.y,  | n.z,  | tc.s, | tc.t
            //
            //
            //             This does ___NOT___ work for layouts like:
            //             float | float | float | double | double | double | uchar | uchar
            //             p.x,  | p.y,  | p.z,  | n.x,   | n.y,   | n.z,   | tc.s, | tc.t
            //             Though the example above is unlikely, it illustrates the point.
            //

            // Default values.
            const GLboolean default_should_be_normalized_by_gpu = false;
            const GLenum    default_data_type                   = GL_FLOAT;

            // Variable values derived from input model file to be filled in.
            GLuint              tmp_vertex_attribute_index      = 0;
            GLint               tmp_component_count             = 0;
            GLvoid*             tmp_first_component_byte_offset = nullptr;
            glt::VertexDataType tmp_vertex_data_type            = glt::VertexDataType::COUNT;

            // Constant values derived from input model file.
            const GLsizei byte_stride_between_elements = static_cast<GLsizei>(
                                                           expected_vertex_attribute_components) *
                                                         sizeof(GL_FLOAT);

            size_t first_component_offset_accumulator = 0;
            for (auto& vertex_type : vertex_data_type_layout_order)
            {
                if (false == _success_out) break;
                tmp_vertex_data_type = vertex_type;

                switch (vertex_type)
                {
                    case glt::VertexDataType::POSITION:
                    {
                        tmp_component_count        = vertex_component_count;
                        tmp_vertex_attribute_index = _position_attribute_index_in;
                        break;
                    }
                    case glt::VertexDataType::NORMAL:
                    {
                        tmp_component_count        = normal_component_count;
                        tmp_vertex_attribute_index = _normal_attribute_index_in;
                        break;
                    }
                    case glt::VertexDataType::TEXTURE:
                    {
                        tmp_component_count        = texture_coordinate_component_count;
                        tmp_vertex_attribute_index = _texture_coordinates_attribute_index_in;
                        break;
                    }
                    default:
                    {
                        Log_e("Unsupported buffer vertex_type.");
                        _success_out = false;
                        break;
                    }
                }

                tmp_first_component_byte_offset = (GLvoid*)(first_component_offset_accumulator *
                                                            sizeof(GL_FLOAT));
                first_component_offset_accumulator += tmp_component_count;

                _vertex_attribute_pointers_out.push_back(
                  glt::VertexAttributeDescriptor(std::move(_vertex_array_object_id_in),
                                                 std::move(tmp_vertex_attribute_index),
                                                 std::move(tmp_component_count),
                                                 std::move(default_data_type),
                                                 std::move(default_should_be_normalized_by_gpu),
                                                 std::move(byte_stride_between_elements),
                                                 std::move(tmp_first_component_byte_offset),
                                                 std::move(tmp_vertex_data_type)));
            }
        }

        _buffer_data_out.reserve(model_data_in.size());
        _buffer_elements_array_out.reserve(model_elements.size());

        _buffer_data_out           = model_data_in;
        _buffer_elements_array_out = model_elements;
        _vertex_data_types_out     = vertex_data_type_layout_order;

        _vertex_count_out = vertex_count;
    }
}
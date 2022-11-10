/*
 *  Note: This entire file was written with the assumption that cases in which these functions
 *        are called are cases in which an error has occurred or when the application is otherwise
 *        slated to shut down and top rendering. These functions were not written with performance
 *        in mind, but rather for ease of use and ease of implementation.
 */

// clang-format off
#include "pch.h"
// clang-format on

#include "state_tools.h"

#include "fileio.h"
#include "logging.h"

const std::string str_graphics_state = "graphics_state";
const std::string str_window_state   = "window_state";
const std::string str_opengl_state   = "opengl_state";

static std::string
GraphicsApiToString(const graphics::API& _graphics_api_in)
{
    switch (_graphics_api_in)
    {
        case graphics::API::OPENGL:
        {
            return "OpenGL";
        }
        default:
        {
            std::string error = "Unrecognized graphics API.";
            Log_e(error);
            return error;
        }
    }
}

static inline void
SplitLineToWords(const std::string&        _file_data_line_in,
                 std::vector<std::string>& _file_data_line_words_out) noexcept
{
    _file_data_line_words_out.clear();
    if (0 == _file_data_line_in.size())
    {
        return;
    }

    const char* const space_delimeter           = " ";
    std::string       local_file_data_line_copy = _file_data_line_in;
    const char*       token = std::strtok(local_file_data_line_copy.data(), space_delimeter);

    while (nullptr != token)
    {
        _file_data_line_words_out.push_back(std::string(token));
        token = std::strtok(nullptr, space_delimeter);
    }
}

static inline void
WordToFloat(bool&              _success_out,
            std::string        _error_string_out,
            const std::string& _word_in,
            float&             _float_out)
{
    _success_out = true;
    assert(0 < _word_in.size());

    char* strtof_ending = nullptr;
    float temp_float    = std::strtof(_word_in.c_str(), &strtof_ending);
    if (0 == strtof_ending)
    {
        _success_out      = false;
        _error_string_out = "Unable to convert " + _word_in + " to float.";
        return;
    }

    _float_out = temp_float;
}

static inline void
ErrorStringUnrecognizedMember(bool&              _success_out,
                              std::string&       _error_string_out,
                              const std::string& _structure_name_in,
                              const std::string& _unrecognized_member_in)
{
    _success_out = false;

    std::stringstream ss;
    ss << "No such member for " << _structure_name_in << ": " << _unrecognized_member_in
       << std::endl;

    _error_string_out = ss.str();
}

static inline void
ErrorStringUnrecognizedStructure(bool&              _success_out,
                                 std::string&       _error_string_out,
                                 const std::string& _structure_name_in)
{
    _success_out = false;

    std::stringstream ss;
    ss << "No such structure handled by parser: " << _structure_name_in << std::endl;

    _error_string_out = ss.str();
}

static inline void
HandleGraphicsState(bool&                           _success_out,
                    std::string&                    _error_string_out,
                    graphics::State&                _graphics_state_out,
                    const std::vector<std::string>& _file_data_line_words_in) noexcept
{
    assert(str_graphics_state == _file_data_line_words_in[0]);
    if (3 != _file_data_line_words_in.size())
    {
        _success_out = false;
        std::stringstream ss;
        ss << "Lines defining " << str_graphics_state << " members require exactly 3 words; found "
           << _file_data_line_words_in.size();
        _error_string_out = ss.str();
        return;
    }

    _success_out = true;

    const std::string str_graphics_state_opengl        = "OPENGL";
    const std::string str_field_of_view_radians        = "field_of_view_radians";
    const std::string str_far_clipping_plane_distance  = "far_clipping_plane_distance";
    const std::string str_near_clipping_plane_distance = "near_clipping_plane_distance";

    float temp_float = 0;

    // Note: non-zero starting index.
    for (size_t word_index = 1; word_index < _file_data_line_words_in.size(); word_index++)
    {
        // Note: Loop iterator modified.
        const std::string& member_var       = _file_data_line_words_in[word_index++];
        const std::string& member_var_value = _file_data_line_words_in[word_index++];

        WordToFloat(_success_out, _error_string_out, member_var_value, temp_float);
        if (false == _success_out) return;

        switch (member_var[0])
        {
            case 'a':
            {
                auto no_such_enumeration = [&](const std::string& _member_var_value_in)
                {
                    _success_out = false;
                    std::stringstream ss;
                    ss << "No such enumeration for " << str_graphics_state << "." << member_var
                       << ": " << _member_var_value_in << std::endl;
                    _error_string_out = ss.str();
                };

                if (str_graphics_state_opengl == member_var_value) // API
                {
                    _graphics_state_out.api = graphics::API::OPENGL;
                }
                else
                {
                    no_such_enumeration(member_var_value);
                    return;
                }

                break;
            }
            case 'f':
            {
                if (str_field_of_view_radians == member_var) // Field of view.
                {
                    _graphics_state_out.field_of_view_radians = temp_float;
                }
                else if (str_far_clipping_plane_distance ==
                         member_var) // Far clipping plane distance.
                {
                    _graphics_state_out.far_clipping_plane_distance = temp_float;
                }
                else
                {
                    ErrorStringUnrecognizedMember(_success_out,
                                                  _error_string_out,
                                                  str_graphics_state,
                                                  member_var);
                    return;
                }

                break;
            }
            case 'n':
            {
                if (str_near_clipping_plane_distance == member_var) // Near clipping plane distance.
                {
                    _graphics_state_out.near_clipping_plane_distance = temp_float;
                }
                else
                {
                    ErrorStringUnrecognizedMember(_success_out,
                                                  _error_string_out,
                                                  str_graphics_state,
                                                  member_var);
                    return;
                }
                break;
            }
            default:
            {
                ErrorStringUnrecognizedMember(_success_out,
                                              _error_string_out,
                                              str_graphics_state,
                                              member_var);
            }
        }
    }
}

static inline void
HandleWindowState(bool&                           _success_out,
                  std::string&                    _error_string_out,
                  app_window::State&              _window_state_out,
                  const std::vector<std::string>& _file_data_line_words_in) noexcept
{
    assert(str_window_state == _file_data_line_words_in[0]);
    if (3 != _file_data_line_words_in.size())
    {
        _success_out = false;
        std::stringstream ss;
        ss << "Lines defining " << str_window_state << " members require exactly 3 words; found "
           << _file_data_line_words_in.size();
        _error_string_out = ss.str();
        return;
    }

    _success_out = true;

    const std::string str_window_width  = "window_width";
    const std::string str_window_height = "window_height";

    float temp_float = 0;

    // Note: non-zero starting index.
    for (size_t word_index = 1; word_index < _file_data_line_words_in.size(); word_index++)
    {
        // Note: Loop iterator modified.
        const std::string& member_var       = _file_data_line_words_in[word_index++];
        const std::string& member_var_value = _file_data_line_words_in[word_index++];

        WordToFloat(_success_out, _error_string_out, member_var_value, temp_float);
        if (false == _success_out) return;

        // Note: Switch based on 7th index, for example after "Window_" in the words
        //       "window_height" ('h' in this case) or "window_width" ('w' in this case).
        switch (member_var[7])
        {
            case 'h':
            {
                if (str_window_height == member_var) // AppWindow height.
                {
                    _window_state_out.window_height = static_cast<unsigned int>(temp_float);
                }
                else
                {
                    ErrorStringUnrecognizedMember(_success_out,
                                                  _error_string_out,
                                                  str_window_state,
                                                  member_var);
                    return;
                }

                break;
            }
            case 'w':
            {
                if (str_window_width == member_var) // AppWindow width.
                {
                    _window_state_out.window_width = static_cast<unsigned int>(temp_float);
                }
                else
                {
                    ErrorStringUnrecognizedMember(_success_out,
                                                  _error_string_out,
                                                  str_window_state,
                                                  member_var);
                    return;
                }
                break;
            }
            default:
            {
                ErrorStringUnrecognizedMember(_success_out,
                                              _error_string_out,
                                              str_window_state,
                                              member_var);
            }
        }
    }
}

static inline void
HandleGLState(bool&                           _success_out,
              std::string&                    _error_string_out,
              glt::GLState*&                  _gl_state_out,
              const std::vector<std::string>& _file_data_line_words_in) noexcept
{
    assert(str_opengl_state == _file_data_line_words_in[0]);
    if (3 != _file_data_line_words_in.size())
    {
        _success_out = false;
        std::stringstream ss;
        ss << "Lines defining " << str_opengl_state << " members require exactly 3 words; found "
           << _file_data_line_words_in.size();
        _error_string_out = ss.str();
        return;
    }

    _success_out = true;

    const std::string str_major_version = "major_version";
    const std::string str_minor_version = "minor_version";

    float temp_float = 0;

    // Note: non-zero starting index.
    for (size_t word_index = 1; word_index < _file_data_line_words_in.size(); word_index++)
    {
        // Note: Loop iterator modified.
        const std::string& member_var       = _file_data_line_words_in[word_index++];
        const std::string& member_var_value = _file_data_line_words_in[word_index++];

        WordToFloat(_success_out, _error_string_out, member_var_value, temp_float);
        if (false == _success_out) return;

        switch (member_var[0])
        {
            case 'm':
            {
                if (str_major_version == member_var) // Major Version.
                {
                    _gl_state_out->Version.major_version = static_cast<int>(temp_float);
                }
                else if (str_minor_version == member_var) // Minor Version.
                {
                    _gl_state_out->Version.minor_version = static_cast<int>(temp_float);
                }
                else
                {
                    ErrorStringUnrecognizedMember(_success_out,
                                                  _error_string_out,
                                                  str_opengl_state,
                                                  member_var);
                    return;
                }

                break;
            }
            default:
            {
                ErrorStringUnrecognizedMember(_success_out,
                                              _error_string_out,
                                              str_opengl_state,
                                              member_var);
            }
        }
    }
}

state::StateCache::~StateCache()
{
    if (nullptr != opengl_state)
    {
        delete opengl_state;
    }
}

state::StateCache*
state::StateCache::GetInstance() noexcept
{
    static StateCache INSTANCE;
    return &INSTANCE;
}

// Note: This function is implemented with pretty, readable output in mind.
//       Performance was not a concern. It is intended as a debugging tool only.
void
state::StateCache::DumpState()
{
#define MEMBER_TO_STRING(x) #x
#define TO_SS_LONG(_member_str_in, _member_value_in)                                             \
    get_member_space_offset(MEMBER_TO_STRING(_member_str_in));                                   \
    ss << std::endl                                                                              \
       << left_pad << left_pad << MEMBER_TO_STRING(_member_str_in) << ":" << member_space_offset \
       << _member_value_in
#define TO_SS(_stucture_member_in) TO_SS_LONG(_stucture_member_in, _stucture_member_in)

    const std::string left_pad(4, ' ');
    std::stringstream ss;
    ss << "State dump:";

    // Note: The pretty print alignment can be adjusted here, should this prove
    //       not to be enough spaces in the future.
    const char        max_space_offset_length = 50;
    const std::string max_space_offset(max_space_offset_length, ' ');

    std::string member_space_offset;
    auto        get_member_space_offset = [&](const std::string& member_string)
    {
        const size_t member_string_size = member_string.size();
        member_space_offset.resize(max_space_offset_length - member_string_size, ' ');

        // Note: See max_space_offset_length.
        assert(max_space_offset_length > member_string_size);
    };

    // API Agnostic State
    {
        ss << std::endl << left_pad << str_graphics_state;
        TO_SS_LONG(graphics_state.api, GraphicsApiToString(graphics_state.api));
        TO_SS(graphics_state.field_of_view_radians);
        TO_SS(graphics_state.near_clipping_plane_distance);
        TO_SS(graphics_state.far_clipping_plane_distance);
    }

    // OpenGL State
    if (nullptr == opengl_state)
    {
        TO_SS_LONG(opengl_state, "Not set");
    }
    else
    {
        ss << std::endl;
        TO_SS(opengl_state->Version.major_version);
        TO_SS(opengl_state->Version.minor_version);

        ss << std::endl;
        TO_SS(opengl_state->Viewport.lower_left_corner_x);
        TO_SS(opengl_state->Viewport.lower_left_corner_y);
        TO_SS(opengl_state->Viewport.width);
        TO_SS(opengl_state->Viewport.height);

        ss << std::endl;
        TO_SS(opengl_state->ClearColor.r);
        TO_SS(opengl_state->ClearColor.g);
        TO_SS(opengl_state->ClearColor.b);
        TO_SS(opengl_state->ClearColor.a);

        ss << std::endl;
        for (auto& enabled_capability : opengl_state->enabled_server_side_capabilities)
        {
            TO_SS_LONG(enabled_capability, GLSeverSideCapabilitiesToString(enabled_capability));
        }

        ss << std::endl;
        TO_SS_LONG(opengl_state->cull_face_faceet,
                   GLCullFaceToString(opengl_state->cull_face_facet));
        TO_SS_LONG(opengl_state->depth_comparison_mode,
                   GLDepthComparisonModeToString(opengl_state->depth_comparison_mode));

        ss << std::endl;
        TO_SS(opengl_state->program_id);
    }

    // AppWindow State
    {
        ss << std::endl;
        TO_SS(window_state.window_height);
        TO_SS(window_state.window_width);
        TO_SS(window_state.aspect_ratio);
        TO_SS(window_state.is_minimized);
    }

    Log_i(ss);

#undef MEMBER_TO_STRING
#undef TO_SS_LONG
#undef TO_SS
}

void
state::StateCache::Initialize(bool&              _success_out,
                              const std::string& _start_up_file_state_in) noexcept
{
    _success_out = true;

    std::string file_data;
    size_t      file_size;
    LoadFileToStdString(_success_out, _start_up_file_state_in.c_str(), file_size, file_data);
    if ((false == _success_out) || (0 == file_size) || (0 == file_data.size()))
    {
        Log_e("Unable to read file; cannot import startup state.");
        return;
    }

    std::istringstream       file_data_stream(file_data);
    std::string              file_data_line;
    std::vector<std::string> file_data_line_words;
    size_t                   file_line_number = 0;

    std::string error_string = "";

    auto report_parse_error = [&]()
    {
        _success_out = false;

        std::stringstream parse_error_ss;
        parse_error_ss << "Failed to parse start-up-scrip (SUS) file." << std::endl
                       << "   SUS File:    " << _start_up_file_state_in << std::endl
                       << "   Line Number: " << file_line_number << std::endl
                       << "   Line Data:   " << std::quoted(file_data_line) << std::endl
                       << "   Details:     " << error_string;

        const std::string test = parse_error_ss.str();
        Log_e(test);
    };

    auto on_failure_report_parse_error = [&]()
    {
        if (false == _success_out)
        {
            report_parse_error();
        }
    };

    // Parse Start-Up-State (SUS) File Contents.
    // -----------------------------------------
    while ((true == _success_out) && (std::getline(file_data_stream, file_data_line)))
    {
        file_line_number++;

        // Skip empty lines.
        if (('\0' == file_data_line[0]) || ('\n' == file_data_line[0]))
        {
            continue;
        }

        SplitLineToWords(file_data_line, file_data_line_words);

        // Avoid some strcmp work by switching on 1st letter of 1st word.
        switch (file_data_line_words[0][0])
        {
            case 'g':
            {
                if (str_graphics_state == file_data_line_words[0]) // API State
                {
                    HandleGraphicsState(_success_out,
                                        error_string,
                                        graphics_state,
                                        file_data_line_words);
                    on_failure_report_parse_error();
                }
                else
                {
                    ErrorStringUnrecognizedStructure(_success_out,
                                                     error_string,
                                                     file_data_line_words[0]);
                    report_parse_error();
                }

                break;
            }
            case 'o':
            {
                if (str_opengl_state == file_data_line_words[0]) // GL State
                {
                    // Note: Expect invalid parsing of GL state (or any other state)
                    //       to cause app exit. In that case, GLState obj is freed when
                    //       the local reference to StateCache goes out of scope via
                    //       StateCache::~StateCache()
                    if (nullptr == opengl_state)
                    {
                        opengl_state = new glt::GLState();
                    }

                    HandleGLState(_success_out, error_string, opengl_state, file_data_line_words);
                    on_failure_report_parse_error();
                }
                else
                {
                    ErrorStringUnrecognizedStructure(_success_out,
                                                     error_string,
                                                     file_data_line_words[0]);
                    report_parse_error();
                }

                break;
            }
            case 'w':
            {
                if (str_window_state == file_data_line_words[0]) // AppWindow State
                {
                    HandleWindowState(_success_out,
                                      error_string,
                                      window_state,
                                      file_data_line_words);
                    on_failure_report_parse_error();
                }
                else
                {
                    ErrorStringUnrecognizedStructure(_success_out,
                                                     error_string,
                                                     file_data_line_words[0]);
                    report_parse_error();
                }

                break;
            }
            default:
            {
                ErrorStringUnrecognizedStructure(_success_out,
                                                 error_string,
                                                 file_data_line_words[0]);
                report_parse_error();
            }
        }
    }

    if (false == _success_out)
    {
        Log_e("Unable to parse startup state file during state cache initialization.");
        return;
    }

    // Initialize Platform Layers.
    // ---------------------------
#ifdef _ENGINE_PLATFORM_WINDOWS_
    // [ cfarvin::TODO ] Delete on bye-bye.
    win32_state = new platform::win32::State();
#endif
}
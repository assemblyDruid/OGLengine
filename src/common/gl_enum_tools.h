/*
 *  Note: This entire file was written with the assumption that cases in which these functions
 *        are called are cases in which an error has occurred or when the application is otherwise
 *        slated to shut down and top rendering. These functions were not written with performance
 *        in mind, but rather for ease of use and ease of implementation.
 * 
 *        File contains local_<> versions of GLenums used as indexes into arrays, functions which
 *        convert from local_<> versions of GLenums to their formal OpenGL equivalents, and ToString
 *        functions which facilitate logging and state dumps.
 */

#ifndef gl_enum_tools_h
#define gl_enum_tools_h

// clang-format off
#include "pch.h"
// clang-format on

#include "logging.h"

// Note: This enum allows for the use of an array of COUNT elements to track
//       the currently bound buffer target and buffer id. The target is the
//       index corresponding to the uint defined by these enumerations. The
//       buffer id bound to that target is defined by the GLuint stored at
//       that index in the array.
enum class GLBufferTargetToArrayIndex
{
    local_GL_ARRAY_BUFFER = 0,
    local_GL_ATOMIC_COUNTER_BUFFER,
    local_GL_COPY_READ_BUFFER,
    local_GL_COPY_WRITE_BUFFER,
    local_GL_DISPATCH_INDIRECT_BUFFER,
    local_GL_DRAW_INDIRECT_BUFFER,
    local_GL_ELEMENT_ARRAY_BUFFER,
    local_GL_PIXEL_PACK_BUFFER,
    local_GL_PIXEL_UNPACK_BUFFER,
    local_GL_QUERY_BUFFER,
    local_GL_SHADER_STORAGE_BUFFER,
    local_GL_TEXTURE_BUFFER,
    local_GL_TRANSFORM_FEEDBACK_BUFFER,
    local_GL_UNIFORM_BUFFER,
    COUNT // VISIBILITY: Count.
};

constexpr unsigned char buffer_target_array_length = static_cast<unsigned char>(
  GLBufferTargetToArrayIndex::COUNT);

enum class GLTextureTargetToArrayIndex
{
    local_GL_TEXTURE_1D = 0,
    local_GL_TEXTURE_2D,
    local_GL_TEXTURE_3D,
    local_GL_TEXTURE_1D_ARRAY,
    local_GL_TEXTURE_2D_ARRAY,
    local_GL_TEXTURE_RECTANGLE,
    local_GL_TEXTURE_CUBE_MAP,
    local_GL_TEXTURE_CUBE_MAP_ARRAY,
    local_GL_TEXTURE_BUFFER,
    local_GL_TEXTURE_2D_MULTISAMPLE,
    local_GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    COUNT // VISIBILITY: Count.
};

constexpr unsigned char texture_target_array_length = static_cast<unsigned char>(
  GLTextureTargetToArrayIndex::COUNT);

inline GLBufferTargetToArrayIndex
GetBufferTargetArrayIndex(const GLenum&& _target_type_in)
{
    GLBufferTargetToArrayIndex array_index = GLBufferTargetToArrayIndex::COUNT;
    switch (_target_type_in)
    {
        case (GL_ARRAY_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_ARRAY_BUFFER;
            break;
        }
        case (GL_ATOMIC_COUNTER_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_ATOMIC_COUNTER_BUFFER;
            break;
        }
        case (GL_COPY_READ_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_COPY_READ_BUFFER;
            break;
        }
        case (GL_COPY_WRITE_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_COPY_WRITE_BUFFER;
            break;
        }
        case (GL_DISPATCH_INDIRECT_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_DISPATCH_INDIRECT_BUFFER;
            break;
        }
        case (GL_DRAW_INDIRECT_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_DRAW_INDIRECT_BUFFER;
            break;
        }
        case (GL_ELEMENT_ARRAY_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_ELEMENT_ARRAY_BUFFER;
            break;
        }
        case (GL_PIXEL_PACK_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_PIXEL_PACK_BUFFER;
            break;
        }
        case (GL_PIXEL_UNPACK_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_PIXEL_UNPACK_BUFFER;
            break;
        }
        case (GL_QUERY_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_QUERY_BUFFER;
            break;
        }
        case (GL_SHADER_STORAGE_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_SHADER_STORAGE_BUFFER;
            break;
        }
        case (GL_TEXTURE_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_TEXTURE_BUFFER;
            break;
        }
        case (GL_TRANSFORM_FEEDBACK_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_TRANSFORM_FEEDBACK_BUFFER;
            break;
        }
        case (GL_UNIFORM_BUFFER):
        {
            array_index = GLBufferTargetToArrayIndex::local_GL_UNIFORM_BUFFER;
            break;
        }
        default:
        {
            Log_e("Unrecognized buffer target type.");
        }
    }

    return array_index;
};

inline GLTextureTargetToArrayIndex
GetTextureTargetArrayIndex(const GLenum&& _target_type_in)
{
    GLTextureTargetToArrayIndex array_index = GLTextureTargetToArrayIndex::COUNT;
    switch (_target_type_in)
    {
        case (GL_TEXTURE_1D):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_1D;
            break;
        }
        case (GL_TEXTURE_2D):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_2D;
            break;
        }
        case (GL_TEXTURE_3D):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_3D;
            break;
        }
        case (GL_TEXTURE_1D_ARRAY):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_1D_ARRAY;
            break;
        }
        case (GL_TEXTURE_2D_ARRAY):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_2D_ARRAY;
            break;
        }
        case (GL_TEXTURE_RECTANGLE):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_RECTANGLE;
            break;
        }
        case (GL_TEXTURE_CUBE_MAP):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_CUBE_MAP;
            break;
        }
        case (GL_TEXTURE_CUBE_MAP_ARRAY):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_CUBE_MAP_ARRAY;
            break;
        }
        case (GL_TEXTURE_BUFFER):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_BUFFER;
            break;
        }
        case (GL_TEXTURE_2D_MULTISAMPLE):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_2D_MULTISAMPLE;
            break;
        }
        case (GL_TEXTURE_2D_MULTISAMPLE_ARRAY):
        {
            array_index = GLTextureTargetToArrayIndex::local_GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
            break;
        }
        default:
        {
            Log_e("Unrecognized buffer texture type.");
        }
    }

    return array_index;
};

inline const char* const
GLSeverSideCapabilitiesToString(const GLenum& _server_side_capability_in)
{
    switch (_server_side_capability_in)
    {
        // Note: Provided 4 definitions for GL_CLIP_DISTANCE.
        //       May need to provide more in the future.
        CASE_ENUM_RETURN_STR(GL_CLIP_DISTANCE0);
        CASE_ENUM_RETURN_STR(GL_CLIP_DISTANCE1);
        CASE_ENUM_RETURN_STR(GL_CLIP_DISTANCE2);
        CASE_ENUM_RETURN_STR(GL_CLIP_DISTANCE3);

        CASE_ENUM_RETURN_STR(GL_BLEND);
        CASE_ENUM_RETURN_STR(GL_COLOR_LOGIC_OP);
        CASE_ENUM_RETURN_STR(GL_CULL_FACE);
        CASE_ENUM_RETURN_STR(GL_DEBUG_OUTPUT);
        CASE_ENUM_RETURN_STR(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        CASE_ENUM_RETURN_STR(GL_DEPTH_CLAMP);
        CASE_ENUM_RETURN_STR(GL_DEPTH_TEST);
        CASE_ENUM_RETURN_STR(GL_DITHER);
        CASE_ENUM_RETURN_STR(GL_FRAMEBUFFER_SRGB);
        CASE_ENUM_RETURN_STR(GL_LINE_SMOOTH);
        CASE_ENUM_RETURN_STR(GL_MULTISAMPLE);
        CASE_ENUM_RETURN_STR(GL_POLYGON_OFFSET_FILL);
        CASE_ENUM_RETURN_STR(GL_POLYGON_OFFSET_LINE);
        CASE_ENUM_RETURN_STR(GL_POLYGON_OFFSET_POINT);
        CASE_ENUM_RETURN_STR(GL_POLYGON_SMOOTH);
        CASE_ENUM_RETURN_STR(GL_PRIMITIVE_RESTART);
        CASE_ENUM_RETURN_STR(GL_PRIMITIVE_RESTART_FIXED_INDEX);
        CASE_ENUM_RETURN_STR(GL_RASTERIZER_DISCARD);
        CASE_ENUM_RETURN_STR(GL_SAMPLE_ALPHA_TO_COVERAGE);
        CASE_ENUM_RETURN_STR(GL_SAMPLE_ALPHA_TO_ONE);
        CASE_ENUM_RETURN_STR(GL_SAMPLE_COVERAGE);
        CASE_ENUM_RETURN_STR(GL_SAMPLE_SHADING);
        CASE_ENUM_RETURN_STR(GL_SAMPLE_MASK);
        CASE_ENUM_RETURN_STR(GL_SCISSOR_TEST);
        CASE_ENUM_RETURN_STR(GL_STENCIL_TEST);
        CASE_ENUM_RETURN_STR(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        CASE_ENUM_RETURN_STR(GL_PROGRAM_POINT_SIZE);
        default:
        {
            const char* const error_string = "Unrecognized server side capability.";
            Log_e(error_string);
            return error_string;
        }
    }
}

inline const char* const
GLCullFaceToString(const GLenum& _cull_mode_in)
{
    switch (_cull_mode_in)
    {
        CASE_ENUM_RETURN_STR(GL_FRONT);
        CASE_ENUM_RETURN_STR(GL_BACK);
        CASE_ENUM_RETURN_STR(GL_FRONT_AND_BACK);
        default:
        {
            const char* const error_string = "Unrecognized culling mode.";
            Log_e(error_string);
            return error_string;
        }
    }
}

inline const char* const
GLDepthComparisonModeToString(const GLenum& _depth_comparison_mode_in)
{
    switch (_depth_comparison_mode_in)
    {
        CASE_ENUM_RETURN_STR(GL_NEVER);
        CASE_ENUM_RETURN_STR(GL_LESS);
        CASE_ENUM_RETURN_STR(GL_EQUAL);
        CASE_ENUM_RETURN_STR(GL_LEQUAL);
        CASE_ENUM_RETURN_STR(GL_GREATER);
        CASE_ENUM_RETURN_STR(GL_NOTEQUAL);
        CASE_ENUM_RETURN_STR(GL_GEQUAL);
        CASE_ENUM_RETURN_STR(GL_ALWAYS);
        default:
        {
            const char* const error_string = "Unrecognized depth comparison mode.";
            Log_e(error_string);
            return error_string;
        }
    }
}

inline const char* const
GLBufferTargetToString(const GLenum& _depth_comparison_mode_in)
{
    switch (_depth_comparison_mode_in)
    {
        CASE_ENUM_RETURN_STR(GL_ARRAY_BUFFER);
        CASE_ENUM_RETURN_STR(GL_ATOMIC_COUNTER_BUFFER);
        CASE_ENUM_RETURN_STR(GL_COPY_READ_BUFFER);
        CASE_ENUM_RETURN_STR(GL_COPY_WRITE_BUFFER);
        CASE_ENUM_RETURN_STR(GL_DISPATCH_INDIRECT_BUFFER);
        CASE_ENUM_RETURN_STR(GL_DRAW_INDIRECT_BUFFER);
        CASE_ENUM_RETURN_STR(GL_ELEMENT_ARRAY_BUFFER);
        CASE_ENUM_RETURN_STR(GL_PIXEL_PACK_BUFFER);
        CASE_ENUM_RETURN_STR(GL_PIXEL_UNPACK_BUFFER);
        CASE_ENUM_RETURN_STR(GL_QUERY_BUFFER);
        CASE_ENUM_RETURN_STR(GL_SHADER_STORAGE_BUFFER);
        CASE_ENUM_RETURN_STR(GL_TEXTURE_BUFFER);
        CASE_ENUM_RETURN_STR(GL_TRANSFORM_FEEDBACK_BUFFER);
        CASE_ENUM_RETURN_STR(GL_UNIFORM_BUFFER);
        default:
        {
            const char* const error_string = "Unrecognized buffer target.";
            Log_e(error_string);
            return error_string;
        }
    }
}

#endif
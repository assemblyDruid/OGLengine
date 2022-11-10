#ifndef pch_in_h
#define pch_in_h
#define _CRT_SECURE_NO_WARNINGS

#define CASE_ENUM_RETURN_STR(_enum_in) \
    case _enum_in:                     \
    {                                  \
        return #_enum_in;              \
    }

#define CASE_ENUM_TO_STR_VAR(_enum_in, _str_var_out) \
    case _enum_in:                                   \
    {                                                \
        _str_var_out = #_enum_in;                    \
        break;                                       \
    }

#if defined(_WIN32) || defined(_WIN64)
#define _ENGINE_PLATFORM_WINDOWS_ 1
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define _ENGINE_PLATFORM_LINUX 1
#elif defined(__APPLE__) || defined(__MACH__)
#define _ENGINE_PLATFORM_APPLE_ 1
#elif defined(__ANDRIOD__)
#define _ENGINE_PLATFORM_ANDRIOD_ 1
#endif

// clang-format off
#define WIN32_LEAN_AND_MEAN 1
#define WIN32_EXTRA_LEAN 1
#include "Windows.h"
#include "wingdi.h"

#include "KHR/khrplatform.h"
#include "GL/glcorearb.h"
#include "GL/wglext.h"
#include "GL/glext.h"

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "glm/glm.hpp"
#pragma warning(disable : 4201)
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr and glm::translate
#pragma warning(default : 4201)
// clang-format on

#include "assert.h"
#include "stdint.h"
#include "time.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#undef _CRT_SECURE_NO_WARNINGS
#endif // pch_in_h
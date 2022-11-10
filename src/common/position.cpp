// clang-format off
#include "pch.h"
// clang-format on

#include "position.h"

// All non-constructor functions must have 'void' return type.
// No allocation within any non-constructor function.

Position3::Position3() noexcept
{
    x = 0;
    y = 0;
    z = 0;
}
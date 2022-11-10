// clang-format off
#include "pch.h"
// clang-format on

#include "input_tools.h"

void
input::SetPressed(const uint64_t&& key, uint64_t*&& const input_state)
{
    *input_state |= key;
}

void
input::SetReleased(const uint64_t&& key, uint64_t*&& const input_state)
{
    *input_state &= ~key;
}

uint64_t
input::GetPressed(const uint64_t&& key, uint64_t*&& const input_state)
{
    return *input_state & key;
}
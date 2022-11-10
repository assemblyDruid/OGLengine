#ifndef input_tools_h
#define input_tools_h

// clang-format off
#include "pch.h"
// clang-format on

#define KEY_A             (1ULL << 0)
#define KEY_B             (1ULL << 1)
#define KEY_C             (1ULL << 2)
#define KEY_D             (1ULL << 3)
#define KEY_E             (1ULL << 4)
#define KEY_F             (1ULL << 5)
#define KEY_G             (1ULL << 6)
#define KEY_H             (1ULL << 7)
#define KEY_I             (1ULL << 8)
#define KEY_J             (1ULL << 9)
#define KEY_K             (1ULL << 10)
#define KEY_L             (1ULL << 11)
#define KEY_M             (1ULL << 12)
#define KEY_N             (1ULL << 13)
#define KEY_O             (1ULL << 14)
#define KEY_P             (1ULL << 15)
#define KEY_Q             (1ULL << 16)
#define KEY_R             (1ULL << 17)
#define KEY_S             (1ULL << 18)
#define KEY_T             (1ULL << 19)
#define KEY_U             (1ULL << 20)
#define KEY_V             (1ULL << 21)
#define KEY_W             (1ULL << 22)
#define KEY_X             (1ULL << 23)
#define KEY_Y             (1ULL << 24)
#define KEY_Z             (1ULL << 25)
#define KEY_0             (1ULL << 26)
#define KEY_1             (1ULL << 27)
#define KEY_2             (1ULL << 28)
#define KEY_3             (1ULL << 29)
#define KEY_4             (1ULL << 30)
#define KEY_5             (1ULL << 31)
#define KEY_6             (1ULL << 32)
#define KEY_7             (1ULL << 33)
#define KEY_8             (1ULL << 34)
#define KEY_9             (1ULL << 35)
#define KEY_arrow_up      (1ULL << 36)
#define KEY_arrow_down    (1ULL << 37)
#define KEY_arrow_left    (1ULL << 38)
#define KEY_arrow_right   (1ULL << 39)
#define KEY_shift_left    (1ULL << 40)
#define KEY_shift_right   (1ULL << 41)
#define KEY_control_left  (1ULL << 42)
#define KEY_control_right (1ULL << 43)
#define MOUSE_right       (1ULL << 44)
#define MOUSE_left        (1ULL << 45)
#define MOUSE_middle      (1ULL << 46)

namespace input
{

    enum class SystemEvent
    {
        EventNone,
        EventClose,
        EventResize,
    };

    struct MousePosition
    {
        uint16_t x;
        uint16_t y;
    };

    void
    SetPressed(const uint64_t&& key, uint64_t*&& const input_state);

    void
    SetReleased(const uint64_t&& key, uint64_t*&& const input_state);

    uint64_t
    GetPressed(const uint64_t&& key, uint64_t*&& const input_state);

} // namespace input
#endif
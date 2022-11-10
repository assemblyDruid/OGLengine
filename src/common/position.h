#ifndef position_h
#define position_h

// All non-constructor functions must have 'void' return type.
// No allocation within any non-constructor function.

struct Position3
{
    const Position3() noexcept;

    float x;
    float y;
    float z;
};

#endif
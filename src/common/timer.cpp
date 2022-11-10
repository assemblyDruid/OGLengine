// clang-format off
#include "pch.h"
// clang-format on

#include "timer.h"

void
Timer::StartTimer() noexcept
{
    timer_start = std::chrono::steady_clock::now();
    return;
}

void
Timer::StopTimer() noexcept
{
    timer_end = std::chrono::steady_clock::now();
    return;
}

void const
Timer::TimerElapsedMs(float& _elapsed_miliseconds_out) const noexcept
{
    _elapsed_miliseconds_out = std::chrono::duration<float, std::milli>(timer_end - timer_start)
                                 .count();
    return;
}

// Provides elapsed time at the resolution of one quarter of a millisecond.
const double&&
Timer::ElapsedMs() noexcept
{
    if (0.0f == delta_t) // Considered initialized when delta_t != 0.0f
    {
        t0 = std::chrono::steady_clock::now();
    }

    t1                   = std::chrono::steady_clock::now();
    elapsed_t += delta_t = std::chrono::duration<float, std::milli>(t1 - t0).count();
    t0                   = t1;

    return std::move(elapsed_t);
}
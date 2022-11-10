#ifndef timer_h
#define timer_h

// clang-format off
#include "pch.h"
// clang-format on

struct Timer
{
    void
    StartTimer() noexcept;

    void
    StopTimer() noexcept;

    void const
    TimerElapsedMs(float& _elapsed_miliseconds_out) const noexcept;

    const double&&
    ElapsedMs() noexcept;

  private:
    // [ cfarvin::TODO ] This timer doesn't know what to do if the StartTimer
    //                   function is called befor the timer is started, or if
    //                   the EndTimer function is called before it is started.
    //                   In the former case, we should probably call EndTime
    //                   and throw a warning, and in the latter case, we should
    //                   probably throw a warning and do nothing. Both cases will
    //                   probably need an internal member determining if the timer
    //                   has been started. To avoid putting bools in this struct,
    //                   we could be sure to set time_start to zero when EndTimer
    //                   is called.
    std::chrono::time_point<std::chrono::steady_clock> timer_start;
    std::chrono::time_point<std::chrono::steady_clock> timer_end;

    // For use with ElapsedMs()
    std::chrono::time_point<std::chrono::steady_clock> t0;
    std::chrono::time_point<std::chrono::steady_clock> t1;
    float                                              delta_t   = 0.0f;
    double                                             elapsed_t = 0.0f;
};

#endif
#ifndef windows_platform_h
#define windows_platform_h

// clang-format off
#include "pch.h"
// clang-format on

#include "app_window.h"

namespace platform
{
    namespace win32
    {
        void
        Initialize(bool& _success_out);

        // [ cfarvin::TODO ] This does not yet destroy the instance. Nor
        //                   is it called when we're done with Win32 on exit.
        // Note: If optional parameters are omitted, the corresponding
        //       members will be deleted from the StateCache.
        void
        Destroy(bool&        _success_out,
                HWND         _window_in               = nullptr,
                HDC          _context_in              = nullptr,
                HGLRC        _gl_rendering_context_in = nullptr,
                const LPCSTR _class_name_in           = nullptr);

        void
        ProcessPlatformEvents();

        void
        SwapBuffers();

    } // namespace win32
} // namespace platform
#endif

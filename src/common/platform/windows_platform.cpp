// clang-format off
#include "pch.h"
// clang-format on

#include "windows_platform.h"

#include "app_window.h"
#include "gl_function_wrappers.h"
#include "gl_tools.h"
#include "logging.h"
#include "state_tools.h"

// [ cfarvin::TODO ] Put this in an object, deallocate when finished with Win32 setup.
std::function<HGLRC(HDC, HGLRC, const int*)> wglCreateContextAttribsARB;
std::function<bool(GLint)>                   wglSwapIntervalEXT;

static LRESULT CALLBACK
WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static state::StateCache* state_cache = state::StateCache::GetInstance();
    switch (uMsg)
    {
        case WM_CLOSE:
        {
            // [ cfarvin::REVISIT ]
            state_cache->is_running = false;
            break;
        }
        case WM_CREATE:
        {
            break;
        }
        case WM_DESTROY:
        {
            // sys_event = uEventClose; // [ cfarvin::REVISIT ]
            // [ cfarvin::TODO ] Deallocate Win32 in close or destroy event.
            ::PostQuitMessage(0);
            break;
        }

            // case WM_LBUTTONDOWN:
            // {
            //     SetInputPressed(uMouse_left, &input_state);
            //     break;
            // }

            // case WM_RBUTTONDOWN:
            // {
            //     SetInputPressed(uMouse_right, &input_state);
            //     break;
            // }

            // case WM_LBUTTONUP:
            // {
            //     SetInputReleased(uMouse_left, &input_state);
            //     break;
            // }

            // case WM_RBUTTONUP:
            // {
            //     SetInputReleased(uMouse_right, &input_state);
            //     break;
            // }

        case WM_SIZE:
        {
            bool success = false;
            app_window::OnWindowResize(success, LOWORD(lParam), HIWORD(lParam));
            assert(true == success);

            // window_props.width  = (uint16_t)LOWORD(lParam);
            // window_props.height = (uint16_t)HIWORD(lParam);

            // if (wParam == SIZE_MINIMIZED)
            // {
            //     window_props.is_minimized = true;
            // }
            // else if (wParam == SIZE_RESTORED)
            // {
            //     window_props.is_minimized = false;
            // }

            break;
        }

            // case WM_MOUSEMOVE:
            // {
            //     GetCursorPos(&mouse_point);
            //     ScreenToClient(hwnd, &mouse_point);

            //     // Note: uMousePos has origin @ lower left == (0, 0, 0)
            //     mouse_coordinates.x = (u16)mouse_point.x;
            //     mouse_coordinates.y = (u16)(window_props.height - mouse_point.y);
            //     break;
            // }
    }

    // Default AppWindow Procedure
    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static const std::string
GetLastWin32SystemErrorMessage()
{
    constexpr DWORD error_message_buffer_length                       = 256;
    char            error_message_buffer[error_message_buffer_length] = { 0 };
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                     nullptr,
                     ::GetLastError(),
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                     error_message_buffer,
                     error_message_buffer_length,
                     nullptr);

    return std::string(error_message_buffer);
}

static void
DestroyWin32Objects(bool&        _success_out,
                    HWND         _window_in,
                    HDC          _context_in,
                    HGLRC        _gl_rendering_context_in,
                    const LPCSTR _class_name_in)
{
    std::stringstream       ss;
    state::StateCache*      state_cache = state::StateCache::GetInstance();
    platform::win32::State* win32_state = state_cache->win32_state;
    if (nullptr == win32_state)
    {
        _success_out = false;
        Log_e("Win32 state cache has not been initialized.");
        return;
    }

    // Used to destroy multiple handles.
    HWND      window_to_destroy = (nullptr == _window_in) ? win32_state->window : _window_in;
    HINSTANCE instance          = win32_state->instance;

    // Device Context
    {
        _success_out = wglMakeCurrent(nullptr, nullptr);
        if (false == _success_out)
        {
            Log_e("[ win32 ] Unable to release the current rendering context.");
            return;
        }

        HDC context_to_destroy = (nullptr == _context_in) ? win32_state->device_context
                                                          : _context_in;

        int release_dc_success = ::ReleaseDC(window_to_destroy, context_to_destroy);
        if (0 == release_dc_success)
        {
            _success_out = false;
            ss << "[ win32 ] Unable to release the current device context. Win32 details: "
               << GetLastWin32SystemErrorMessage();
            Log_e(ss);
            return;
        }
    }

    // GL Rendering Context
    {
        HGLRC rendering_context_to_destroy = (nullptr == _gl_rendering_context_in)
                                               ? win32_state->gl_rendering_context
                                               : _gl_rendering_context_in;

        _success_out = wglDeleteContext(rendering_context_to_destroy);
        if (false == _success_out)
        {
            Log_e("[ win32 ] Unable to release the delete rendering context.");
            return;
        }
    }

    // Window
    {
        _success_out = ::DestroyWindow(window_to_destroy);
        if (false == _success_out)
        {
            ss << "[ win32 ] Unable to destroy window. Win32 details: "
               << GetLastWin32SystemErrorMessage();
            Log_e(ss);
            return;
        }
    }

    // Window Class
    {
        LPCSTR class_to_destroy = (nullptr == _class_name_in) ? win32_state->class_name
                                                              : _class_name_in;
        _success_out            = ::UnregisterClassA(class_to_destroy, instance);
        if (false == _success_out)
        {
            ss << "[ win32 ] Unable to unregister window class with name: " << _class_name_in
               << ". Win32 details: " << GetLastWin32SystemErrorMessage();
            Log_e(ss);
            return;
        }
    }
}

static void
CreateWin32Window(bool& _success_out, HWND& _window_out, const LPCSTR& _class_name_in)
{
    state::StateCache*      state_cache = state::StateCache::GetInstance();
    platform::win32::State* win32_state = state_cache->win32_state;
    if (nullptr == win32_state)
    {
        _success_out = false;
        Log_e("Win32 state cache has not been initialized.");
        return;
    }

    std::stringstream ss;

    WNDCLASSEXA window_class = { 0 };
    window_class.style       = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = WindowProcedure;
    window_class.hInstance = win32_state->instance = GetModuleHandle(NULL);
    window_class.hCursor                           = LoadCursor(NULL, IDC_WAIT);
    window_class.lpszClassName = win32_state->class_name = _class_name_in;
    window_class.cbSize                                  = sizeof(window_class);

    if (false == ::RegisterClassExA(&window_class))
    {
        _success_out = false;

        ss << "[ win32 ] Could not register window class. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // [ cfarvin::TODO ] Use state_cache (if defined) for _FINAL_ window position and dimensions.
    _window_out = ::CreateWindowExA(
      0,                          // Extended window style flags.
      window_class.lpszClassName, // Registered class name.
      "",                         // Window name.
      WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window style flags.
      0,                      // Initial ULHC horizontal window position.
      0,                      // Initial ULHC vertical window position.
      600,                    // Initial width in device units.
      600,                    // Initial height in device units.
      0,                      // Parent window, if any.
      0,                      // Handle to a menu. Uses class menu if null.
      window_class.hInstance, // Window instance handle.
      0);                     // Pointer to CREATESTRUCT value as lParam in WM_CREATE events.

    if (nullptr == _window_out)
    {
        _success_out = false;
        ss << "[ win32 ] Windows returned null handle to client window. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    if (false == ::IsWindow(_window_out))
    {
        _success_out = false;
        ss << "[ win32 ] Windows reports that the window is invalid. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }
}

// Acquires the "best" pixel format using WGL methods.
//
// This is done by first creating a "dummy" win32 window, pixel format,
// and rendering context. Pixel formats are queried and set with legacy
// win32 methods (not WGL).
//
// This is necessary because in win32, you cannot query for the WGL
// extensions which are necessary for pixel format acquisition without
// a valid rendering context. Thus, a dummy context must be created,
// extensions queried, pixel formats chosen with wgl extensions, the
// dummy context destroyed, and a new context created which utilizes
// the obtained pixel format.
static void
GetWin32PixelFormat(bool&               _success_out,
                    const unsigned int& _buffer_format_length_in,
                    int*                _format_buffer_out)
{
    std::stringstream ss;

    // ---------------------------------
    //
    // Dummy rendering context creation.
    //
    // ---------------------------------

    // Dummy variables.
    HWND             dummy_window;
    HDC              dummy_context;
    HGLRC            dummy_gl_rendering_context;
    constexpr LPCSTR dummy_class_name = "__TEMPORARY";

    // Dummy PFD.
    PIXELFORMATDESCRIPTOR pixel_format_descriptor = { 0 };
    {
        pixel_format_descriptor.nSize    = sizeof(pixel_format_descriptor);
        pixel_format_descriptor.nVersion = 1;
        pixel_format_descriptor.dwFlags  = PFD_GENERIC_ACCELERATED | PFD_DRAW_TO_WINDOW |
                                          PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
        pixel_format_descriptor.cColorBits = 24;
        pixel_format_descriptor.cRedBits   = 8;
        pixel_format_descriptor.cGreenBits = 8;
        pixel_format_descriptor.cBlueBits  = 8;
        pixel_format_descriptor.cDepthBits = 32;
    }

    // Create dummy window.
    CreateWin32Window(_success_out, dummy_window, dummy_class_name);
    if (false == _success_out)
    {
        Log_e("Unable to create dummy contexts; could not create dummy window.");
        return;
    }

    // Obtain dummy device context.
    dummy_context = ::GetDC(dummy_window);
    if (nullptr == dummy_context)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to get a device context. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // Choose a dummy pixel format.
    int best_matching_pixel_format_desc = ChoosePixelFormat(dummy_context,
                                                            &pixel_format_descriptor);
    if (0 == best_matching_pixel_format_desc)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to choose a suitable pixel format. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // Set a dummy pixel format.
    bool pixel_formatting_success = ::SetPixelFormat(dummy_context,
                                                     best_matching_pixel_format_desc,
                                                     &pixel_format_descriptor);
    if (false == pixel_formatting_success)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to set the device context pixel format. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // Create a dummy rendering context.
    // Note: wglCreateContext is the older version of wglCreateContextAttribsARB,
    //       and is only to be used in dummy rendering context creation.
    dummy_gl_rendering_context = wglCreateContext(dummy_context);
    if (nullptr == dummy_gl_rendering_context)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to create an OpenGL rendering context. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // Make the dummy rendering context current.
    bool make_current_success = wglMakeCurrent(dummy_context, dummy_gl_rendering_context);
    if (false == make_current_success)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to make the rendering context current. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // ------------------------------
    //
    // OpenGL WGL extension querying.
    //
    // ------------------------------

    // Obtain pointer to WGL extension query function.
    const char* (*wglGetExtensionsStringARB)(HDC) = (const char* (*)(HDC))wglGetProcAddress(
      "wglGetExtensionsStringARB");
    if (nullptr == wglGetExtensionsStringARB)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a function pointer for wglGetExtensionsStringsARB");
        return;
    }

    std::string extensions(wglGetExtensionsStringARB(dummy_context));
    if (std::string::npos == extensions.find("WGL_ARB_create_context") ||
        std::string::npos == extensions.find("WGL_ARB_pixel_format") ||
        std::string::npos == extensions.find("WGL_EXT_swap_control"))
    {
        _success_out = false;
        Log_e("This system does not support advanced pixel format selection or advanced context "
              "creation.");
        return;
    }

    // [ cfarvin::TODO ] May need to defer the destruction of the dummy objects in case
    //                   function pointers are no longer valid for use in the actual
    //                   window and context creation.

    // ----------------------------------------------------------------------
    //
    // Use the dummy rendering context to retrieve pointers to WGL functions.
    //
    // ----------------------------------------------------------------------

    // Obtain a function pointer to wglCreateContextARB.
    wglCreateContextAttribsARB = (HGLRC(*)(HDC, HGLRC, const int*))wglGetProcAddress(
      "wglCreateContextAttribsARB");
    if (nullptr == wglCreateContextAttribsARB)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a function pointer for wglCreateContextAttribsARB");
        return;
    }

    // Obtain a function pointer to wglSwapIntervalEXT.
    wglSwapIntervalEXT = (bool (*)(GLint))wglGetProcAddress("wglSwapIntervalEXT");
    if (nullptr == wglSwapIntervalEXT)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a function pointer for wglSwapIntervalEXT");
        return;
    }

    // ------------------------------------------------------------------------------------
    //
    // Use WGL extensions and dummy rendering context to query for the "best" pixel format.
    //
    // ------------------------------------------------------------------------------------

    // Obtain function pointer for wglChoosePixelFormatARB.
    std::function<bool(HDC, const int*, const float*, UINT, const int*, UINT*)>
      wglChoosePixelFormatARB = (bool (*)(HDC, const int*, const float*, UINT, const int*, UINT*))
        wglGetProcAddress("wglChoosePixelFormatARB");
    if (nullptr == wglChoosePixelFormatARB)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a function pointer for wglChoosePixelFormatARB");
        return;
    }

    // [ cfarvin::TODO ] This needs to go in state, probably. You already have a note to move some of the
    //                   members of Win32 to a separate Win32State structure, and some of these should
    //                   probably go there too.
    // clang-format off
    int pixel_format_int_attribute_list[] =
    {
        WGL_SUPPORT_OPENGL_ARB, 1,                         // Require OpenGL support.
        WGL_DRAW_TO_WINDOW_ARB, 1,                         // Require render-to-window support.
        WGL_RED_BITS_ARB,       8,                         // Require minimum 8bpp red.
        WGL_GREEN_BITS_ARB,     8,                         // Require minimum 8bpp green.
        WGL_BLUE_BITS_ARB,      8,                         // Require minimum 8bpp blue.
        WGL_DEPTH_BITS_ARB,     16,                        // Require minimum 16bpp depth buffer.
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB, // Require full HSV acceleration.
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,         // Require RGBA pixel type.
        WGL_DOUBLE_BUFFER_ARB,  1,                         // Require double buffered format.
        0                                                  // Null terminate list semaphore.
    };
    // clang-format on

    // From Khronos specification for wglChoosePixelFormatARB:
    // -------------------------------------------------------
    // <piFormats> points to an array of returned indices of the matching
    // pixel formats. The best pixel formats (i.e., closest match and best
    // format for the hardware) are at the head of the list. The caller
    // must allocate this array and it must have at least <nMaxFormats>
    // entries.
    //
    // Note: This confirms our assumption about the "best" matching pixel
    //       format indices being at the head of the returned buffer.
    unsigned int format_count         = 0;
    bool         pixel_format_success = wglChoosePixelFormatARB(
      dummy_context,                   // HDC for window.
      pixel_format_int_attribute_list, // Desired properties (integer represented).
      nullptr,                         // Desired properties (float represented).
      _buffer_format_length_in,        // Size of results array.
      _format_buffer_out,              // Results array.
      &format_count);                  // Results array element count.

    if (false == pixel_format_success)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a suitable pixel format.");
        return;
    }

    // ---------------------------------------------------------------------
    //
    // Destruction of dummy rendering context and other dummy win32 objects.
    //
    // ---------------------------------------------------------------------

    // Note: Testing shows on all common hardware implementations that the function
    //       pointers for WGL functions remain valid even after the win32 objects
    //       associated with windows and contexts (both device and rendering) are
    //       destroyed. Some sources have warned against this, so in the future if
    //       it is suddenly found that WGL functions are no longer working as
    //       expected, this should be one of the first things to test. In that case,
    //       we would want to defer destruction of the win32 objects until after
    //       the WGL functions have been used in the _actual_, as opposed to the
    //       _dummy_ win32 object creation. To do so, the local CreateWin32Window
    //       function will have to be implemented/moved to account for using dummy
    //       variables for some window class definitions instead of those in the
    //       win32 state cache, which is how it is currently implemented.
    DestroyWin32Objects(_success_out,
                        dummy_window,
                        dummy_context,
                        dummy_gl_rendering_context,
                        dummy_class_name);
    if (false == _success_out)
    {
        Log_e("[ win32 ] Unable to destroy dummy contexts and window.");
        return;
    }
}

void
platform::win32::Destroy(bool&        _success_out,
                         HWND         _window_in,
                         HDC          _context_in,
                         HGLRC        _gl_rendering_context_in,
                         const LPCSTR _class_name_in)
{
    DestroyWin32Objects(_success_out,
                        _window_in,
                        _context_in,
                        _gl_rendering_context_in,
                        _class_name_in);
}

void
platform::win32::Initialize(bool& _success_out)
{
    _success_out = true;
    std::stringstream       ss;
    state::StateCache*      state_cache = state::StateCache::GetInstance();
    platform::win32::State* win32_state = state_cache->win32_state;
    if (nullptr == win32_state)
    {
        _success_out = false;
        Log_e("Win32 state cache has not been initialized.");
        return;
    }

#ifdef _ENGINE_DEBUG_
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#endif

    // Note: the return_buffer_format_length variable below also limits the number of matching pixel
    //       formats returned by wglChoosePixelFormatARB.
    constexpr unsigned int return_buffer_format_length                       = 5;
    int                    return_format_buffer[return_buffer_format_length] = { 0 };
    GetWin32PixelFormat(_success_out, return_buffer_format_length, &(return_format_buffer[0]));
    if (false == _success_out)
    {
        Log_e("Failed to create Win32 dummy contexts.");
        return;
    }

    // --------------------------------------------------------------
    //
    // Actual window, device context, and gl device context creation.
    //
    // --------------------------------------------------------------

    CreateWin32Window(_success_out, win32_state->window, win32_state->class_name);
    if (false == _success_out)
    {
        Log_e("Unable to create dummy contexts; could not create dummy window.");
        return;
    }

    win32_state->device_context = ::GetDC(win32_state->window);
    if (nullptr == win32_state->device_context)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to get a device context. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // Note: Win32 fills in a PFD which best matches the index of the hardware device format
    //       described by the first index of the buffer returned from GetWin32PixelFormat().
    //       According to the most trustworthy source I can find, of which there are few, the
    //       indices in the return buffer from GetWin32PixelFormat() are in order, starting
    //       at index 0, from the closest matching formats to the least closest matching
    //       formats derived from calls to wglChoosePixelFormatARB with our specified
    //       preferences.
    PIXELFORMATDESCRIPTOR final_pixel_format_descriptor;
    DescribePixelFormat(win32_state->device_context,
                        return_format_buffer[0],
                        sizeof(PIXELFORMATDESCRIPTOR),
                        &final_pixel_format_descriptor);
    bool pixel_formatting_success = ::SetPixelFormat(win32_state->device_context,
                                                     return_format_buffer[0],
                                                     &final_pixel_format_descriptor);
    if (false == pixel_formatting_success)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to set the device context pixel format. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    // [ cfarvin::TODO ] This needs to go in state, probably. You already have a note to move some of the
    //                   members of Win32 to a separate Win32State structure, and some of these should
    //                   probably go there too.
    // clang-format off
    int context_attribute_list[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _ENGINE_DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT
#endif
        0
    };
    // clang-format on

    // Ensure we were able to obtain a function pointer to wglCreateContextAttribsARB
    // during the "dummy" phase.
    if (nullptr == wglCreateContextAttribsARB)
    {
        _success_out = false;
        Log_e("[ win32 ] Function pointer for wglCreateContextAttribsARB was not obtained during "
              "\"dummy\" phase.");
        return;
    }

    // Note: Passing nullptr as 2nd argument ensures that no information can be shared between contexts.
    //       If we want to do so in the future, we will have to specify the shared context here.
    win32_state->gl_rendering_context = wglCreateContextAttribsARB(win32_state->device_context,
                                                                   nullptr,
                                                                   &context_attribute_list[0]);

    // Ensure we were able to obtain a function pointer to wglSwapIntervalEXT during
    // the "dummy" phase.
    if (nullptr == wglSwapIntervalEXT)
    {
        _success_out = false;
        Log_e("[ win32 ] Function pointer for wglSwapIntervalEXT was not obtained during "
              "\"dummy\" phase.");
        return;
    }

    // From Khronos Specification for wglSwapIntervalEXT:
    // ---------------------------------------------------------------------
    // glSwapIntervalEXT specifies the minimum number of video frame periods
    // per buffer swap for the window associated with the current context.
    // The interval takes effect when SwapBuffers or wglSwapLayerBuffer
    // is first called subsequent to the wglSwapIntervalEXT call.
    //
    // The parameter <interval> specifies the minimum number of video frames
    // that are displayed before a buffer swap will occur.
    //
    // A video frame period is the time required by the monitor to display a
    // full frame of video data.  In the case of an interlaced monitor,
    // this is typically the time required to display both the even and odd
    // fields of a frame of video data.  An interval set to a value of 2
    // means that the color buffers will be swapped at most every other video
    // frame.
    //
    // If <interval> is set to a value of 0, buffer swaps are not synchron-
    // ized to a video frame.  The <interval> value is silently clamped to
    // the maximum implementation-dependent value supported before being
    // stored.
    //
    // The swap interval is not part of the render context state.  It cannot
    // be pushed or popped.  The current swap interval for the window
    // associated with the current context can be obtained by calling
    // wglGetSwapIntervalEXT.  The default swap interval is 1.
    wglSwapIntervalEXT(1); // [ cfarvin::TOD ] Add to state cache as v-sync option.

    bool make_current_success = wglMakeCurrent(win32_state->device_context,
                                               win32_state->gl_rendering_context);
    if (false == make_current_success)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to make the rendering context current. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    ::ShowWindow(win32_state->window, 10);
    ss << "Device context properties:" << std::endl
       << "Vendor:   " << glfn::GetString(GL_VENDOR) << std::endl
       << "Version:  " << glfn::GetString(GL_VERSION) << std::endl
       << "Renderer: " << glfn::GetString(GL_RENDERER);
    Log_i(ss);

    // [ cfarvin::TODO ] Destroy real context on exit.

    state_cache->window_state.is_minimized = false;
    _success_out                           = true;
}

void
platform::win32::SwapBuffers()
{
    static const state::StateCache* const state_cache = state::StateCache::GetInstance();
    bool success = ::SwapBuffers(state_cache->win32_state->device_context);
    assert(true == success);
}

void
platform::win32::ProcessPlatformEvents()
{
    MSG msg = { 0 };
    // [ cfarvin::TODO ] Is this right? PeekMsg vs GetMsg?
    while (0 != ::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

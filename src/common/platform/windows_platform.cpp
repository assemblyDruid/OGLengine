// clang-format off
#include "pch.h"
// clang-format on

#include "windows_platform.h"

#include "app_window.h"
#include "gl_function_wrappers.h"
#include "gl_tools.h"
#include "logging.h"

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

static void
CreateWin32LegacyDummyContexts(bool&                        _success_out,
                               HWND&                        _dummy_window_out,
                               HDC&                         _dummy_context_out,
                               HGLRC&                       _dummy_gl_rendering_context_out,
                               const PIXELFORMATDESCRIPTOR& _pixel_format_descriptor_in,
                               const LPCSTR&                _dummy_class_name_in)
{
    std::stringstream ss;

    CreateWin32Window(_success_out, _dummy_window_out, _dummy_class_name_in);
    if (false == _success_out)
    {
        Log_e("Unable to create dummy contexts; could not create dummy window.");
        return;
    }

    _dummy_context_out = ::GetDC(_dummy_window_out);
    if (nullptr == _dummy_context_out)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to get a device context. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    int best_matching_pixel_format_desc = ChoosePixelFormat(_dummy_context_out,
                                                            &_pixel_format_descriptor_in);
    if (0 == best_matching_pixel_format_desc)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to choose a suitable pixel format. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    bool pixel_formatting_success = ::SetPixelFormat(_dummy_context_out,
                                                     best_matching_pixel_format_desc,
                                                     &_pixel_format_descriptor_in);
    if (false == pixel_formatting_success)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to set the device context pixel format. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    _dummy_gl_rendering_context_out = wglCreateContext(_dummy_context_out);
    if (nullptr == _dummy_gl_rendering_context_out)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to create an OpenGL rendering context. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
        return;
    }

    bool make_current_success = wglMakeCurrent(_dummy_context_out, _dummy_gl_rendering_context_out);
    if (false == make_current_success)
    {
        _success_out = false;
        ss << "[ win32 ] Unable to make the rendering context current. Win32 details: "
           << GetLastWin32SystemErrorMessage();
        Log_e(ss);
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

    PIXELFORMATDESCRIPTOR pixel_format_descriptor = { 0 };
    pixel_format_descriptor.nSize                 = sizeof(pixel_format_descriptor);
    pixel_format_descriptor.nVersion              = 1;
    pixel_format_descriptor.dwFlags               = PFD_GENERIC_ACCELERATED | PFD_DRAW_TO_WINDOW |
                                      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixel_format_descriptor.iPixelType = PFD_TYPE_RGBA;
    pixel_format_descriptor.cColorBits = 24;
    pixel_format_descriptor.cRedBits   = 8;
    pixel_format_descriptor.cGreenBits = 8;
    pixel_format_descriptor.cBlueBits  = 8;
    pixel_format_descriptor.cDepthBits = 32;

    HWND   dummy_window;
    HDC    dummy_context;
    HGLRC  dummy_gl_rendering_context;
    LPCSTR dummy_class_name = "__TEMPORARY";
    CreateWin32LegacyDummyContexts(_success_out,
                                   dummy_window,
                                   dummy_context,
                                   dummy_gl_rendering_context,
                                   pixel_format_descriptor,
                                   dummy_class_name);

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
        std::string::npos == extensions.find("WGL_ARB_pixel_format"))
    {
        _success_out = false;
        Log_e("This system does not support advanced pixel format selection or advanced context "
              "creation.");
        return;
    }

    const bool (*wglChoosePixelFormatARB)(HDC, const int*, const float*, UINT, const int*, UINT*) =
      (const bool (*)(HDC, const int*, const float*, UINT, const int*, UINT*))wglGetProcAddress(
        "wglChoosePixelFormatARB");
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
    int integer_attribute_list[] =
    {
        WGL_SUPPORT_OPENGL_ARB, 1,                         // Require OpenGL support.
        WGL_DRAW_TO_WINDOW_ARB, 1,                         // Require render-to-window support.
        WGL_RED_BITS_ARB,       8,                         // Require minimum 8bpp red.
        WGL_GREEN_BITS_ARB,     8,                         // Require minimum 8bpp green.
        WGL_BLUE_BITS_ARB,      8,                         // Require minimum 8bpp blue.
        WGL_DEPTH_BITS_ARB,     16,                        // Require minimum 16bpp depth buffer.
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB, // Require full HSV acceleration.
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,         // Require RGBA pixel type.
        0                                                  // Null terminate list semaphore.
    };

    // Note: the return_buffer_format_length variable below also limits the number of matching pixel
    //       formats returned by wglChoosePixelFormatARB.
    constexpr unsigned int return_buffer_format_length                       = 5;
    int                    return_format_buffer[return_buffer_format_length] = { 0 };
    unsigned int           return_format_count                               = 0;

    bool pixel_format_success =
    wglChoosePixelFormatARB(dummy_context,               // HDC for window.
                            integer_attribute_list,      // Desired properties (integer represented).
                            nullptr,                     // Desired properties (float represented).
                            return_buffer_format_length, // Size of results array.
                            &return_format_buffer[0],    // Results array.
                            &return_format_count);       // Results array element count.
    // clang-format on
    if (false == pixel_format_success)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a suitable pixel format.");
        return;
    }

    const HGLRC (*wglCreateContextAttribsARB)(HDC, HGLRC, const int*) =
      (const HGLRC (*)(HDC, HGLRC, const int*))wglGetProcAddress("wglCreateContextAttribsARB");
    if (nullptr == wglCreateContextAttribsARB)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a function pointer for wglCreateContextAttribsARB");
        return;
    }

    const bool (*wglSwapIntervalEXT)(GLint) = (const bool (*)(GLint))wglGetProcAddress(
      "wglSwapIntervalEXT");
    if (nullptr == wglSwapIntervalEXT)
    {
        _success_out = false;
        Log_e("[ win32 ] Unable to obtain a function pointer for wglSwapIntervalEXT");
        return;
    }

    Destroy(_success_out,
            dummy_window,
            dummy_context,
            dummy_gl_rendering_context,
            dummy_class_name);
    if (false == _success_out)
    {
        Log_e("[ win32 ] Unable to destroy dummy contexts and window.");
        return;
    }

    // Actual window, device context, and gl device context creation.
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

    bool pixel_formatting_success = ::SetPixelFormat(win32_state->device_context,
                                                     return_format_buffer[0],
                                                     &pixel_format_descriptor);
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

    // Note: Passing nullptr as 2nd argument ensures that no information can be shared between contexts.
    //       If we want to do so in the future, we will have to specify the shared context here.
    win32_state->gl_rendering_context = wglCreateContextAttribsARB(win32_state->device_context,
                                                                   nullptr,
                                                                   &context_attribute_list[0]);

    wglSwapIntervalEXT(1);

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

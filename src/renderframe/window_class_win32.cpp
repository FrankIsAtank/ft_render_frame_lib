#include "base/platform.h"
#ifdef FT_OS_WINDOWS

#include "window_class_win32.h"

// Define statics
std::recursive_mutex ft::rf::window_class_win32::s_map_mutex;
std::map<::HWND, ft::rf::window_class_win32::t_window_proc> ft::rf::window_class_win32::s_instance_map;


// Constructor
ft::rf::window_class_win32::window_class_win32(t_params && p_params)
{ 
    m_atom = register_class(p_params);
    m_params = std::move(p_params);
}


// Destructor
ft::rf::window_class_win32::~window_class_win32() noexcept
{
    if (m_atom.has_value())
    {
        ::UnregisterClassA(m_params.class_name.c_str(), m_params.module_handle);
    }
}


// Get this class' ATOM (identifier)
::ATOM ft::rf::window_class_win32::get_atom() const
{
    return *m_atom;
}


// Get this class' name
const std::string & ft::rf::window_class_win32::get_name() const
{
    return m_params.class_name;
}


// Register a window instance for message processing
void ft::rf::window_class_win32::register_window(::HWND p_window_handle, t_window_proc p_proc)
{
    std::lock_guard<decltype(s_map_mutex)> lock{ s_map_mutex };
    s_instance_map.emplace(p_window_handle, std::move(p_proc));
}


// Unregister a window instance
void ft::rf::window_class_win32::unregister_window(::HWND p_window_handle)
{
    std::lock_guard<decltype(s_map_mutex)> lock{ s_map_mutex };
    s_instance_map.erase(p_window_handle);
}


// Define a class and register it
::ATOM ft::rf::window_class_win32::register_class(const t_params & p_params)
{
    auto class_info = WNDCLASSEXA
    {
        sizeof(WNDCLASSEXA),            // Size of struct
        make_class_style(p_params),     // Class style
        &WindowProc,                    // WinProc entry point
        0,                              // Don't ask for extra bytes
        0,                              // Again don't ask for extra bytes
        p_params.module_handle,         // Host handle
        make_icon_handle(p_params),     // Icon handle
        make_cursor_handle(p_params),   // Cursor handle
        make_brush(p_params),           // Background brush
        nullptr,                        // No menu ressource item
        p_params.class_name.c_str(),    // Class name
        make_small_icon_handle(p_params)
    };

    const auto result = ::RegisterClassExA( &class_info );

    if (result == 0) {
        // Failed to register
        throw t_except_failed{ "Failed to register window class" };
    }
    return result;
}


// Define a class style
::UINT ft::rf::window_class_win32::make_class_style(const t_params & p_params)
{
    // List of flags available at : https://docs.microsoft.com/en-us/windows/desktop/winmsg/window-class-styles
    return
        CS_OWNDC |               // Window can have it's own device context
        CS_HREDRAW | CS_VREDRAW;    // Redraw on frame size change
}


// Define a class icon
::HICON ft::rf::window_class_win32::make_icon_handle(const t_params & p_params)
{
    // Use some default system icon
    (void)p_params;
    return nullptr;
}


// Define a class small icon
::HICON ft::rf::window_class_win32::make_small_icon_handle(const t_params & p_params)
{
    // Uses make_icon_handle's result to try to find an appropriate small icon
    (void)p_params;
    return nullptr;
}


// Define a class cursor
::HCURSOR ft::rf::window_class_win32::make_cursor_handle(const t_params & p_params)
{
    // Does not automatically set the cursor shape
    (void)p_params;
    return nullptr;
}


// Define the class background handle
::HBRUSH ft::rf::window_class_win32::make_brush(const t_params & p_params)
{
    // Requires that the application paints it's own background
    // Since we intend to display renderings, this is fine
    (void)p_params;
    return nullptr;
}


// WindowProc entry point
::LRESULT ft::rf::window_class_win32::WindowProc(
    ::HWND hwnd,
    ::UINT uMsg,
    ::WPARAM wParam,
    ::LPARAM lParam)
{
    std::lock_guard<decltype(s_map_mutex)> lock{ s_map_mutex };
    auto iter = s_instance_map.find(hwnd);
    if (iter != std::end(s_instance_map)) {
        return iter->second(uMsg, wParam, lParam);
    }
    else {
        return ::DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
}

#endif  // FT_OS_WINDOWS

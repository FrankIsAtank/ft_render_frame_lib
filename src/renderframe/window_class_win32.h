#pragma once

// Handles class registration for Windows windows

// other projects
#include "base/windows_include.h"

// standard headers
#include <functional>
#include <map>
#include <mutex>
#include <optional>
#include <string>

namespace ft {
namespace rf {

class window_class_win32
{
public:
    struct t_params {
        std::string class_name;
        ::HINSTANCE module_handle = nullptr;
    };

    struct t_except_failed : std::runtime_error {
        std::runtime_error::runtime_error;
    };

public:
    // Default constructor
    window_class_win32() = default;

    // Constructor
    explicit window_class_win32(t_params&& p_params);

    // Destructor
    ~window_class_win32() noexcept;

    // Get this class' ATOM (identifier)
    ::ATOM get_atom() const;

    // Get this class' name
    const std::string& get_name() const;

    // Register a window instance for message processing
    using t_window_proc = std::function<::LRESULT(::UINT, ::WPARAM, ::LPARAM)>;
    static void register_window(::HWND p_window_handle, t_window_proc p_proc);

    // Unregister a window instance
    static void unregister_window(::HWND p_window_handle);

private:
    // Define a class and register it
    static ::ATOM register_class(const t_params& p_params);

    // Define a class style
    static ::UINT make_class_style(const t_params& p_params);

    // Define a class icon
    static ::HICON make_icon_handle(const t_params& p_params);

    // Define a class small icon
    static ::HICON make_small_icon_handle(const t_params& p_params);

    // Define a class cursor
    static ::HCURSOR make_cursor_handle(const t_params& p_params);

    // Define the class background handle
    static ::HBRUSH make_brush(const t_params& p_params);

    // WindowProc entry point
    static ::LRESULT WindowProc(
        ::HWND hwnd,
        ::UINT uMsg,
        ::WPARAM wParam,
        ::LPARAM lParam);

private:
    // Initial conditions for this class
    t_params m_params;

    // WINAPI handle referring to this class
    std::optional<::ATOM> m_atom;

    // Registered instances
    static std::recursive_mutex s_map_mutex;
    static std::map<::HWND, t_window_proc> s_instance_map;

};  // class window_class_win32

}   // namespace rf
}   // namespace ft

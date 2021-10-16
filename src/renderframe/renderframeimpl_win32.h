#pragma once

// Platform specific component of a render_frame for Windows

// this project
#include "renderframeparams.h"
#include "window_class_win32.h"

#include "opengl_context/opengl_context.h"

// other projects
#include "handle/ressource_handle.hpp"
#include "base/windows_include.h"

// standard headers
#include <memory>

namespace ft {

// Forward declaration
namespace gl {
namespace basegl {
struct hdc_wrap;
}
}

namespace rf {

class render_frame_impl
{
public:
    struct t_except_failed : std::runtime_error {
        using std::runtime_error::runtime_error;
    };

public:
    // Default constructor
    render_frame_impl() = default;

    // Constructor
    explicit render_frame_impl(const t_render_frame_params& p_params);

    // Prevent copy
    render_frame_impl(const render_frame_impl&) = delete;
    render_frame_impl& operator=(const render_frame_impl&) = delete;

    // Move constructor
    render_frame_impl(render_frame_impl&& p_ref) noexcept = default;

    // Move assignment operator
    render_frame_impl& operator=(render_frame_impl&& p_ref) noexcept = default;


    // Get this render frame's native handle
    ::HWND native_handle() const;


    // Finish the current frame and display it
    // If double buffering is used, display it
    void display_frame();


    // Get the opengl context assigned to this render frame
    context::opengl_context& get_opengl_context();
    const context::opengl_context& get_opengl_context() const;


    // Show or hide the render frame
    void set_visible(const bool p_visible);

    // Is the render frame shown?
    bool is_visible() const;

private:
    // Baes initialization done by all constructors
    void initialize();

    // Set the dummy pixel format
    void set_dummy_pixel_format();

    // Get the device context
    gl::basegl::hdc_wrap get_context() const;

    // Window message processingfunction
    ::LRESULT window_proc(::UINT p_msg, ::WPARAM p_w_params, ::LPARAM p_l_params);

    struct t_dummy_ctor_tag {};

public:
    // Constructor
    // Construct a dummy render frame with a legacy opengl context
    // This constructor is not publically usable
    explicit render_frame_impl(const t_render_frame_params& p_params, t_dummy_ctor_tag);

private:
    // Initialize the window class if it hasn't been initialized yet and get a pointer to it
    static std::shared_ptr<window_class_win32> get_class_obj(const t_render_frame_params& p_params);

    // Get this window's style value
    static ::DWORD make_style(const t_render_frame_params& p_params);

    // Get this window's extended style value
    static ::DWORD make_style_ext(const t_render_frame_params& p_params);

    // Get this window's host object's handle
    static ::HINSTANCE get_host_handle(const t_render_frame_params& p_params);

private:
    // The frame parameters that were used to initialize this frame
    t_render_frame_params m_params;

    // Current class object registration object
    std::shared_ptr<window_class_win32> m_class_obj;

    // This window's handle
    base::handle::ressource_handle<::HWND, nullptr> m_handle;

    // Owns the registration of this object to it's class
    base::handle::ressource_handle<window_class_win32*, nullptr> m_class_obj_registration;

    // The opengl context associated with this window
    std::unique_ptr<context::opengl_context> m_opengl_context;

    // Is the window currently visible?
    bool m_visible = false;


};  // class render_frame

}   // namespace rf
}   // namespace ft

#include "renderframeimpl_win32.h"

// project headers
#include "basegl/hdc_wrap.h"

// other projects
#include "error/ft_assert.h"

// Class name to use
const char g_window_class_name[] = "ft_rf_window_class_name";

// Constructor
ft::rf::render_frame_impl::render_frame_impl(const t_render_frame_params & p_params) :
    m_params(p_params)
{
    // Create the window
    initialize();

    // Create a dummy window with legacy pixel format to allow
    //  a basic opengl context to be created
    // This context will be used to load the real context's function pointers
    auto dummy_window = std::make_unique<render_frame_impl>(p_params, t_dummy_ctor_tag{});

    FT_ASSERT(dummy_window->m_opengl_context != nullptr);
    auto & dummy_context = *dummy_window->m_opengl_context;

    {   // Use the dummy window's opengl context to set this render frame's pixel format
        const auto format_id = dummy_context.assign_pixel_format(p_params.pixel_format);
        
        // Apply the pixel format to the window
        auto context = ::GetDC(m_handle);
        PIXELFORMATDESCRIPTOR pixel_format_descriptor;
        ::DescribePixelFormat(context, format_id, sizeof(pixel_format_descriptor), &pixel_format_descriptor);
        ::SetPixelFormat(context, format_id, &pixel_format_descriptor);
    }

    // Create an opengl context for this render frame
    m_opengl_context = std::make_unique<ft::rf::context::opengl_context>(
        get_context(), dummy_context);
}


// Get the opengl context assigned to this render frame
ft::rf::context::opengl_context & 
ft::rf::render_frame_impl::get_opengl_context()
{
    FT_ASSERT(m_opengl_context != nullptr);
    return *m_opengl_context;
}


// Get this render frame's native handle
::HWND ft::rf::render_frame_impl::native_handle() const
{
    return m_handle;
}


// Finish the current frame and display it
// If double buffering is used, display it
void ft::rf::render_frame_impl::display_frame()
{
    if (m_params.pixel_format.double_buffer)
    {
        SwapBuffers(::GetDC(m_handle));
    }
}


// Get the opengl context assigned to this render frame
const ft::rf::context::opengl_context & 
ft::rf::render_frame_impl::get_opengl_context() const
{
    FT_ASSERT(m_opengl_context != nullptr);
    return *m_opengl_context;
}


// Show or hide the render frame
void ft::rf::render_frame_impl::set_visible(const bool p_visible)
{
    ::ShowWindow(m_handle, p_visible ? SW_SHOW : SW_HIDE);
    m_visible = p_visible;
}


// Is the render frame shown?
bool ft::rf::render_frame_impl::is_visible() const
{
    return m_visible;
}


// Baes initialization done by all constructors
void ft::rf::render_frame_impl::initialize()
{
    m_class_obj = get_class_obj(m_params);

    const auto & position = m_params.position;
    const auto & size = m_params.size;

    auto window_handle = ::CreateWindowExA(
        make_style_ext(m_params),
        m_class_obj->get_name().c_str(),
        m_params.window_name.c_str(),
        make_style(m_params),
        position.x(), position.y(),    // Initial position
        size.x(), size.y(),             // Initial size
        nullptr,                        // No parent window
        nullptr,                        // No default menu
        get_host_handle(m_params),
        nullptr                         // No extra information
    );

    if (window_handle == nullptr)
    {
        throw t_except_failed("Failed to create window object");
    }

    // Save the handle
    m_handle = { window_handle,
        [](::HWND & p_obj) {
            ::DestroyWindow(p_obj);
        } };

    // Register this instance with the class
    m_class_obj->register_window(m_handle, [this](::UINT p_msg, ::WPARAM p_w_params, ::LPARAM p_l_params) -> ::LRESULT {
        return window_proc(p_msg, p_w_params, p_l_params);
    });

    // Setup an object to unregister the instance when the time comes
    m_class_obj_registration = { m_class_obj.get(),
        [window_handle](window_class_win32* & p_obj) {
        p_obj->unregister_window(window_handle);
        p_obj = nullptr;
    } };
}


// Set the dummy pixel format
void ft::rf::render_frame_impl::set_dummy_pixel_format()
{
    // The default pixel format
    constexpr PIXELFORMATDESCRIPTOR dummy_format = { 
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                  // Version
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
        PFD_TYPE_RGBA,      // The kind of framebuffer (RGBA or palette)
        32,                 // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,   // Color bits and shifts
        0, 0,               // Alpha bits and shifts               
        0, 0, 0, 0, 0,      // Accumulator bits
        24,                 // Number of bits for the depthbuffer
        8,                  // Number of bits for the stencilbuffer
        0,                  // Number of Aux buffers in the framebuffer
        PFD_MAIN_PLANE,     // Layer type (deprecated)
        0,                  // Reserved
        0, 0, 0             // Masks (deprecated)
    };

    const auto device_context = ::GetDC(m_handle);

    // Get the associated pixel format ID
    const auto pixel_format_id = ::ChoosePixelFormat(device_context, &dummy_format);
    if (pixel_format_id == 0) 
    {
        throw t_except_failed("failed to choose dummy pixel format");
    }

    // Set the pixel format to use
    auto success = ::SetPixelFormat(device_context, pixel_format_id, &dummy_format);
    if (success == false)
    {
        throw t_except_failed("failed to set dummy pixel format");
    }
}


// Get the device context
ft::gl::basegl::hdc_wrap
ft::rf::render_frame_impl::get_context() const
{
    return { ::GetDC(m_handle) };
}


// Window message processing function
::LRESULT 
ft::rf::render_frame_impl::window_proc(::UINT p_msg, ::WPARAM p_w_params, ::LPARAM p_l_params)
{
    return ::DefWindowProcA(m_handle, p_msg, p_w_params, p_l_params);
}


// Constructor
// Construct a dummy render frame with a legacy opengl context
// This constructor is not publically usable
ft::rf::render_frame_impl::render_frame_impl(const t_render_frame_params & p_params, t_dummy_ctor_tag) :
    m_params(p_params)
{
    // Create the window
    initialize();

    // Set the dummy pixel format
    set_dummy_pixel_format();

    // Create an opengl context for this render frame
    m_opengl_context = std::make_unique<ft::rf::context::opengl_context>(
        get_context(), ft::rf::context::opengl_context::t_legacy_ctor_tag{});
}


// Initialize the window class if it hasn't been initialized yet and get a pointer to it
std::shared_ptr<ft::rf::window_class_win32>
ft::rf::render_frame_impl::get_class_obj(const t_render_frame_params & p_params)
{
    static std::mutex instance_lock;
    static std::weak_ptr<ft::rf::window_class_win32> instance;

    std::lock_guard<decltype(instance_lock)> lock{ instance_lock };

    auto ptr = instance.lock();
    if (!ptr)
    {
        using t_class_param = window_class_win32::t_params;
        auto class_params = t_class_param{
            g_window_class_name,
            get_host_handle(p_params)
        };
        ptr = std::make_shared<window_class_win32>(std::move(class_params));
        instance = ptr;
    }

    return ptr;
}


// Get this window's style value
::DWORD ft::rf::render_frame_impl::make_style(const t_render_frame_params & p_params)
{
    return 0;
}


// Get this window's extended style value
::DWORD ft::rf::render_frame_impl::make_style_ext(const t_render_frame_params & p_params)
{
    return 0;
}


// Get this window's host object's handle
::HINSTANCE ft::rf::render_frame_impl::get_host_handle(const t_render_frame_params & p_params)
{
    (void)p_params;
    // Assume it's always the executable
    return ::GetModuleHandleA(nullptr);
}

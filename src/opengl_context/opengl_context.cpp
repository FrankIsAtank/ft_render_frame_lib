#include "opengl_context.h"
#include "opengl_context_members.h"

// OpenGL headers
#include "basegl/hdc_wrap.h"
#include "basegl/opengl_except.h"
#include "basegl/opengl_headers.h"
#include "basegl/opengl_version.h"

#include "call_opengl_function.h"
#include "opengl_debug.h"
#include "opengl_function.h"

// other projects
#include "error/ft_assert.h"

// standard headers
#include <map>

// Initialize an opengl context for a given render context
ft::rf::context::opengl_context::opengl_context(
    const gl::basegl::hdc_wrap & p_hdc,
    opengl_context & p_reference)
{
    FT_ASSERT(m_opengl_ptr != nullptr);
    m_opengl_ptr->device_context = p_hdc.value;

    // OpenGL version to use
    auto[major, minor] = get_version();

    // Temporarily make the provided context the active one
    //  and use it to initilize this new context
    auto active = make_current{ p_reference };

    // The context attributes to use
    const int context_attributs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    // Create the context
    m_opengl_ptr->render_context = call_opengl_fail_value<err::context_init, nullptr>(
        opengl_function<PFNWGLCREATECONTEXTATTRIBSARBPROC>("wglCreateContextAttribsARB"),
        p_hdc.value,        // Device context
        HGLRC{ 0 },         // No sharing enabled
        context_attributs); // Attributs to use

    // Initialize debugging
    debug::init_debugging(*this);
}


// Initialize a legacy opengl context
// Used to initialize function pointers for a real context
ft::rf::context::opengl_context::opengl_context(
    const gl::basegl::hdc_wrap & p_hdc, t_legacy_ctor_tag)
{
    FT_ASSERT(m_opengl_ptr != nullptr);
    m_opengl_ptr->device_context = p_hdc.value;
    m_opengl_ptr->render_context = wglCreateContext(p_hdc.value);

    // Initialize debugging
    debug::init_debugging(*this);

}


// Initializes a render frame's pixel format
// Must only be called once which is done by the render frame's constructor
// Find a pixel format descriptor but the caller still needs to apply
//  that pixel format to the render frame
// Returns the pixel format identifier
int ft::rf::context::opengl_context::assign_pixel_format(const gl::t_pixel_format & p_format)
{
    // Temporarily make this context the active one
    auto active = make_current{ *this };

    // Create a pixel format description array
    const auto pixel_format = make_pixel_attributs(p_format);

    // Choose the appropriate pixel format
    int pixel_format_id; UINT num_formats;
    call_opengl_fail_value<err::context_bad_pixel_format, false>(
        opengl_function<PFNWGLCHOOSEPIXELFORMATARBPROC>("wglChoosePixelFormatARB"),
        m_opengl_ptr->device_context,   // Device context
        pixel_format.data(),            // Integeral attributes
        nullptr,                        // Floating point attributes
        1,                              // Maximum number of formats to return
        &pixel_format_id,               // Where to write the pixel format ID
        &num_formats);                  // How many formats were generated (limited to the max provided)

    if (num_formats <= 0)
    {
        err::context_bad_pixel_format::raise("wglChoosePixelFormatARB failed");
    }

    return pixel_format_id;
}


// OpenGL version used
// Returns a pair { major, minor }
std::pair<int, int> ft::rf::context::opengl_context::get_version() const
{
    return {
        gl::basegl::g_opengl_version.major,
        gl::basegl::g_opengl_version.minor
    };
}


// Get the opengl handles for this context
const ft::rf::context::opengl_context_members & 
ft::rf::context::opengl_context::get_handles() const
{
    FT_ASSERT(m_opengl_ptr != nullptr);
    return *m_opengl_ptr;
}


// Clear all render buffers and prepare the render a new frame
void ft::rf::context::opengl_context::clear_frame(const gl::color<float> & p_color)
{
    auto active = make_current{ *this };

    call_opengl<err::context_failed_to_clear_frame>(
        glClearColor,
        p_color.red, 
        p_color.green, 
        p_color.blue, 
        1.0f);
    call_opengl<err::context_failed_to_clear_frame>(
        glClear,
        GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

namespace {
    const std::map<ft::rf::context::opengl_context::t_polygon_mode, GLenum> g_polygon_mode_map = {
        {ft::rf::context::opengl_context::t_polygon_mode::point, GL_POINT},
        {ft::rf::context::opengl_context::t_polygon_mode::line, GL_LINE},
        {ft::rf::context::opengl_context::t_polygon_mode::fill, GL_FILL}
    };
};  // anonymous namespace

// Set the polygon render mode
void ft::rf::context::opengl_context::set_polygon_mode(const t_polygon_mode p_mode)
{
    auto active = make_current{ *this };

    auto iter = g_polygon_mode_map.find(p_mode);
    if (iter == std::end(g_polygon_mode_map)) {
        err::context_edit_error::raise("unknown polygon mode enum value");
    }

    call_opengl<err::context_edit_error>(
        glPolygonMode,
        GL_FRONT_AND_BACK,
        iter->second);

    m_polygon_mode = p_mode;
}


// Get the polygon render mode
ft::rf::context::opengl_context::t_polygon_mode
ft::rf::context::opengl_context::get_polygon_mode() const
{
    return m_polygon_mode;
}


namespace {
    const std::map<ft::rf::context::opengl_context::t_culling_mode, GLenum> g_culling_mode_map = {
        {ft::rf::context::opengl_context::t_culling_mode::back_culling, GL_BACK},
        {ft::rf::context::opengl_context::t_culling_mode::front_culling, GL_FRONT},
        {ft::rf::context::opengl_context::t_culling_mode::full_culling, GL_FRONT_AND_BACK}
    };
};  // anonymous namespace

// Set the culling render mode
void ft::rf::context::opengl_context::set_culling_mode(const t_culling_mode p_mode)
{
    auto active = make_current{ *this };

    if (p_mode == t_culling_mode::no_culling)
    {
        // Disable face culling
        call_opengl<err::context_edit_error>(
            glDisable,
            GL_CULL_FACE);
    }
    else
    {
        auto iter = g_culling_mode_map.find(p_mode);
        if (iter == std::end(g_culling_mode_map)) {
            err::context_edit_error::raise("unknown face culling mode enum value");
        }

        // Enable culling
        call_opengl<err::context_edit_error>(
            glEnable,
            GL_CULL_FACE);

        // Set the face to cull
        call_opengl<err::context_edit_error>(
            glCullFace,
            iter->second);
    }

    m_culling_mode = p_mode;
}


// Get the culling render mode
ft::rf::context::opengl_context::t_culling_mode
ft::rf::context::opengl_context::get_culling_mode() const
{
    return m_culling_mode;
}


// Set the depth testing mode
void ft::rf::context::opengl_context::set_depth_test_mode(
    const t_depth_buffering p_mode)
{
    auto active = make_current{ *this };

    if (p_mode == t_depth_buffering::disabled)
    {
        // Disable depth testing
        call_opengl<err::context_edit_error>(
            glDisable,
            GL_DEPTH_TEST);
    }
    else
    {
        // Enable depth testing
        call_opengl<err::context_edit_error>(
            glEnable,
            GL_DEPTH_TEST);

        if (p_mode == t_depth_buffering::read_only)
        {
            // Set depth testing to read only
            call_opengl<err::context_edit_error>(
                glDepthMask,
                GL_FALSE);
        }
        else
        {
            // Set depth testing to normal read/write
            call_opengl<err::context_edit_error>(
                glDepthMask,
                GL_TRUE);
        }

    }

    m_depth_buffering = p_mode;
}


// Get the depth testing mode
ft::rf::context::opengl_context::t_depth_buffering
ft::rf::context::opengl_context::get_depth_test_mode() const
{
    return m_depth_buffering;
}


// Set the alpha blending mode
void ft::rf::context::opengl_context::set_blending_mode(const t_blend_mode p_mode)
{
    if (p_mode == t_blend_mode::disabled)
    {
        // Disable alpha blending
        call_opengl<err::context_edit_error>(
            glDisable,
            GL_BLEND);

    }
    else
    {
        // Enable alpha blending
        call_opengl<err::context_edit_error>(
            glEnable,
            GL_BLEND);

        // Set the blending function
        if (p_mode == t_blend_mode::default_transparency)
        {
            call_opengl<err::context_edit_error>(
                glBlendFunc,
                GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA);
        }
        else
        {
            FT_UNREACHABLE;
        }
    }
    m_blending_mode = p_mode;
}


// Get the alpha blending mode
ft::rf::context::opengl_context::t_blend_mode
ft::rf::context::opengl_context::get_blending_mode() const
{
    return m_blending_mode;
}


// Construct pixel attributes from a pixel format struct
std::vector<int> ft::rf::context::opengl_context::make_pixel_attributs(
    const gl::t_pixel_format & p_format)
{
    std::vector<int> attributs;

    auto add_attribute = [&attributs](const int key, const int value) mutable
    {
        attributs.emplace_back(key);
        attributs.emplace_back(value);
    };
    
    add_attribute(WGL_DRAW_TO_WINDOW_ARB,   GL_TRUE);                   // Renders to a window
    add_attribute(WGL_SUPPORT_OPENGL_ARB,   GL_TRUE);                   // Supports OpenGL
    add_attribute(WGL_DOUBLE_BUFFER_ARB,    p_format.double_buffer);    // Allow double buffering
    add_attribute(WGL_PIXEL_TYPE_ARB,       WGL_TYPE_RGBA_ARB);         // RGBA (as opposed to color pallette)
    add_attribute(WGL_ACCELERATION_ARB,     WGL_FULL_ACCELERATION_ARB); // Must be supported by the GPU
    add_attribute(WGL_COLOR_BITS_ARB,       p_format.color_depth);      // RGBA value bit count
    add_attribute(WGL_ALPHA_BITS_ARB,       p_format.alpha_depth);      // Alpha channel bit count
    add_attribute(WGL_DEPTH_BITS_ARB,       p_format.z_buffer_depth);   // z-axis depth buffer bit count
    add_attribute(WGL_STENCIL_BITS_ARB,     p_format.stencil_depth);    // Sencil buffer pixel bit count
    add_attribute(WGL_SAMPLE_BUFFERS_ARB,   p_format.multisample > 0);  // Enable anti aliasing
    add_attribute(WGL_SAMPLES_ARB,          p_format.multisample);      // Anti aliasing sample count

    // Always end the attributes array with a null
    attributs.emplace_back(0);
    return attributs;
}


// Activate this context by making it the currently active
//  context for the calling thread
// Use an instance of make_current constructed with this instance
//  as argument instead of trying to call this function
void ft::rf::context::opengl_context::activate() const
{
    auto active_thread_lock = m_active_thread.make_lock();

    // Check if this context is already active for another thread
    // A context can only be active for one thread at a given time
    const auto thread_id = std::this_thread::get_id();
    if (active_thread_lock->has_value() == true && active_thread_lock->value() != thread_id)
    {
        // Another thread is already using this context
        err::context_activate_error::raise("context already in use by another thread");
    }

    // Try to make the context active
    // Can't use call_opengl because no context is active yet
    const auto success = wglMakeCurrent(
        m_opengl_ptr->device_context,
        m_opengl_ptr->render_context);

    if(success == false || glGetError() != GL_NO_ERROR)
    {
        err::context_activate_error::raise("wglMakeCurrent failed");
    }
    
    // You are supposed to call glewInit() after every context change
    call_opengl_pass_value<err::context_activate_error, GLEW_OK>(glewInit);

    // Remember which thread is using this context
    (*active_thread_lock) = thread_id;
}


// Make no context currently active
void ft::rf::context::opengl_context::deactivate() const
{
    ::wglMakeCurrent(nullptr, nullptr);
    glGetError();   // Ignore the error...
    m_active_thread.make_lock()->reset();
}

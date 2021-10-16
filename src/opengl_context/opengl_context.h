#pragma once

// Owns an OpenGL context

// project headers
#include "basegl/color.h"
#include "basegl/pixel_format.h"
#include "make_current.h"

//  other headers
#include "thread/lockable.h"

// standard headers
#include <memory>
#include <optional>
#include <stdexcept>
#include <thread>
#include <vector>

namespace ft {

// Forward declaration
namespace gl {
namespace basegl {
struct hdc_wrap;
}
}

namespace rf {
namespace context {

// Forward declaration
struct opengl_context_members;

class opengl_context
{
public:
    enum class t_polygon_mode {
        point,
        line,
        fill    // Default mode
    };

    enum class t_culling_mode {
        no_culling, // Default mode
        back_culling,
        front_culling,
        full_culling // why?
    };

    enum class t_depth_buffering {
        disabled,   // Disable z buffering, Default mode
        enabled,    // Normal depth testing
        read_only   // Test but don't update
    };

    enum class t_blend_mode {
        disabled,
        default_transparency    // Normal alpha blending
    };

public:
    // Initialize an opengl context for a given render context
    // Uses an existing opengl_context to initalize function pointers
    opengl_context(const gl::basegl::hdc_wrap& p_hdc, opengl_context& p_reference);

    // Initialize a legacy opengl context
    // Used to initialize function pointers for a real context
    struct t_legacy_ctor_tag {};
    opengl_context(const gl::basegl::hdc_wrap& p_hdc, t_legacy_ctor_tag);

    // Prevent copy
    opengl_context(const opengl_context&) = delete;
    opengl_context& operator=(const opengl_context&) = delete;

    // Move allowed
    opengl_context(opengl_context&&) = default;
    opengl_context& operator=(opengl_context&&) = default;

    // Initializes a render frame's pixel format
    // Must only be called once which is done by the render frame's constructor
    // Find a pixel format descriptor but the caller still needs to apply
    //  that pixel format to the render frame
    // Returns the pixel format identifier
    int assign_pixel_format(const gl::t_pixel_format& p_format);

    // OpenGL version used
    // Returns a pair { major, minor }
    std::pair<int, int> get_version() const;

    // Get the opengl handles for this context
    const opengl_context_members& get_handles() const;

    // Clear all render buffers and prepare the render a new frame
    void clear_frame(const gl::color<float>& p_color);


    // Set the polygon render mode
    void set_polygon_mode(const t_polygon_mode p_mode);

    // Get the polygon render mode
    t_polygon_mode get_polygon_mode() const;


    // Set the culling render mode
    void set_culling_mode(const t_culling_mode p_mode);

    // Get the culling render mode
    t_culling_mode get_culling_mode() const;


    // Set the depth testing mode
    void set_depth_test_mode(const t_depth_buffering p_mode);

    // Get the depth testing mode
    t_depth_buffering get_depth_test_mode() const;

    // Set the alpha blending mode
    void set_blending_mode(const t_blend_mode p_mode);

    // Get the alpha blending mode
    t_blend_mode get_blending_mode() const;

private:
    // Construct pixel attributes from a pixel format struct
    std::vector<int> make_pixel_attributs(const gl::t_pixel_format& p_format);

    // Activate this context by making it the currently active
    //  context for the calling thread
    // Use an instance of make_current constructed with this instance
    //  as argument instead of trying to call this function
    friend static void make_current<opengl_context>::activate(const opengl_context&);
    void activate() const;

    // Make no context currently active
    friend static void make_current<opengl_context>::deactivate(const opengl_context&);
    void deactivate() const;

private:
    // OpenGL specific members 
    std::shared_ptr<opengl_context_members> m_opengl_ptr =
        std::make_shared<opengl_context_members>();

    // Currently selected polygon rendering mode
    t_polygon_mode m_polygon_mode = t_polygon_mode::fill;

    // Currently selected face culling rendering mode
    t_culling_mode m_culling_mode = t_culling_mode::no_culling;

    // Currently selected buffering
    t_depth_buffering m_depth_buffering = t_depth_buffering::disabled;

    // Current alpha blending mode
    t_blend_mode m_blending_mode = t_blend_mode::disabled;

    // If this context is currently the active context for a thread
    //  then the thread ID of that thread is stored here
    mutable base::thread::lockable<std::optional<std::thread::id>> m_active_thread;

};  // class opengl_context

}   // namespace context
}   // namespace rf
}   // namespace ft

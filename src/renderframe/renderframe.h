#pragma once

// Platform agnostic interface for a window or other render context
#include "renderframeparams.h"

// standard headers
#include <memory>

namespace ft {
namespace rf {

// Forward declaration
namespace context {
    class opengl_context;
}
namespace procloop {
    class process_loop;
}

// Platform-dependent component of this class
class render_frame_impl;

class render_frame
{
public:
    // Constructor
    explicit render_frame(t_render_frame_params p_params);

    // Get the window's initial parameters
    const t_render_frame_params& get_params() const;

    // Get the opengl context assigned to this render frame
    context::opengl_context& get_opengl_context();
    const context::opengl_context& get_opengl_context() const;

    // Clear the current frame and prepare to start drawing to it
    void start_frame();

    // Finish the current frame and display it
    // If double buffering is used, display it
    void end_frame();

    // Show or hide the render frame
    void set_visible(const bool p_visible);
    bool is_visible() const;

private:
    // Get the underlying implementation
    render_frame_impl& get_impl_obj();

    // Allow procloop to access `get_impl_obj`
    friend class ft::rf::procloop::process_loop;

private:
    // Deleters for forward declared types
    template<class T>
    struct t_deleter {
        void operator()(T*);
    };

private:
    // The frame's parameters
    t_render_frame_params m_params;

    // Actual implementation
    std::unique_ptr<render_frame_impl, t_deleter<render_frame_impl>> m_impl;

    // Process loop worker for this frame
    std::unique_ptr<procloop::process_loop, t_deleter<procloop::process_loop>> m_process_loop;

};  // class render_frame

}   // namespace rf
}   // namespace ft

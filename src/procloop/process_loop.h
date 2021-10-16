#pragma once

// standard headers
#include <memory>

namespace ft {
namespace rf {

// Forward declaration
class render_frame_impl;

namespace procloop {

// Platform-dependent component of this class
class process_loop_impl;

class process_loop final
{
public:
    // Constructor
    // Starts the worker thread
    // If a render frame pointer is provided the process loop
    //  instance manages that render frame's messages
    process_loop(render_frame_impl* p_render_frame);

    // Destructor
    // Joins the worker thread
    ~process_loop();

    // Not copiable
    process_loop(const process_loop&) = delete;
    void operator=(const process_loop&) = delete;

    // Moveable
    process_loop(process_loop&&) noexcept = default;
    process_loop& operator=(process_loop&&) noexcept = default;

private:
    // process_loop_impl deleter
    struct impl_deleter {
        void operator()(process_loop_impl*);
    };

private:
    // Holds the actual implementation
    std::unique_ptr<process_loop_impl, impl_deleter> m_impl;

};  // class render_frame

}   // namespace procloop
}   // namespace rf
}   // namespace ft

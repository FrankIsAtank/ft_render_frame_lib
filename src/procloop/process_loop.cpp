#include "process_loop.h"
#include "process_loop_impl.h"

#include "renderframe/renderframe.h"
#include "renderframe/renderframe_impl.h"

// ft_base_lib headers
#include "error/ft_assert.h"

// Destructor
// Joins the worker thread
ft::rf::procloop::process_loop::~process_loop()
{
    if (m_impl != nullptr) {
        m_impl->stop();
    }
}


// The calling thread will run this process loop
void ft::rf::procloop::process_loop::run_loop(
    render_frame_impl& p_render_frame,
    std::promise<void>& p_ready_to_start)
{
    FT_ASSERT(m_impl == nullptr);
    
    // Create the actual implementation object
    m_impl.reset(new process_loop_impl());

    // Start it
    // This only returns when the process loop exits
    m_impl->run_loop(p_render_frame.native_handle(), p_ready_to_start);
}


// Deleter for unique_ptr to forward declared type
void ft::rf::procloop::process_loop::impl_deleter::operator()(process_loop_impl* p_ptr)
{
    delete p_ptr;
}

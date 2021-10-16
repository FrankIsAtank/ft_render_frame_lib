#include "process_loop.h"
#include "process_loop_impl.h"

#include "renderframe/renderframe.h"
#include "renderframe/renderframe_impl.h"

// Constructor
// Starts the worker thread
ft::rf::procloop::process_loop::process_loop(render_frame_impl* p_render_frame) :
    m_impl(new process_loop_impl())
{
    if (p_render_frame != nullptr)
    {
        m_impl->start(p_render_frame->native_handle());
    }
    else
    {
        m_impl->start(nullptr);
    }
}


// Destructor
// Joins the worker thread
ft::rf::procloop::process_loop::~process_loop()
{
    m_impl->stop();
    m_impl->join();
}


// Deleter for unique_ptr to forward declared type
void ft::rf::procloop::process_loop::impl_deleter::operator()(process_loop_impl* p_ptr)
{
    delete p_ptr;
}

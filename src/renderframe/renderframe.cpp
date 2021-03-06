// Implementation for the platform agnostic component of renderframe

// Project headers
#include "procloop/process_loop.h"
#include "renderframe.h"
#include "renderframe_impl.h"

// ft_base_lib headers
#include "error/ft_assert.h"


// Constructor
ft::rf::render_frame::render_frame(t_render_frame_params p_params) :
    m_params{std::move(p_params)}
{
    // Create a promise to know when initialization finishes
    auto ready = std::promise<void>{};
    auto ready_future = ready.get_future();

    // Spawn a worker to create the render frame and to run
    //  it's process loop
    m_worker = std::async(std::launch::async, [this, ready = std::move(ready)]() mutable
    {
        // Create the render frame instance
        this->m_impl.reset(new render_frame_impl(this->m_params));

        // Create and start the process loop
        this->m_process_loop.reset(new procloop::process_loop());
        this->m_process_loop->run_loop(*(this->m_impl), ready);
    });

    // Wait for the process loop to be initialized
    ready_future.get();
}


// Get the window's initial parameters
const ft::rf::t_render_frame_params &
ft::rf::render_frame::get_params() const
{
    return m_params;
}


// Get the opengl context assigned to this render frame
ft::rf::context::opengl_context &
ft::rf::render_frame::get_opengl_context()
{
    return m_impl->get_opengl_context();
}


// Get the opengl context assigned to this render frame
const ft::rf::context::opengl_context & 
ft::rf::render_frame::get_opengl_context() const
{
    return m_impl->get_opengl_context();
}


// Clear the current frame and prepare to start drawing to it
void ft::rf::render_frame::start_frame()
{
    const auto & background = get_params().background;
    m_impl->get_opengl_context().clear_frame(background);
}


// Finish the current frame and display it
// If double buffering is used, display it
void ft::rf::render_frame::end_frame()
{
    m_impl->display_frame();
}


// Show or hide the render frame
void ft::rf::render_frame::set_visible(const bool p_visible)
{
    return m_impl->set_visible(p_visible);
}


// Is the render frame shown?
bool ft::rf::render_frame::is_visible() const
{
    return m_impl->is_visible();
}

// Get the underlying implementation
ft::rf::render_frame_impl&
ft::rf::render_frame::get_impl_obj()
{
    FT_ASSERT(m_impl != nullptr);
    return *m_impl;
}

// Deleters for forward declared render_frame_impl
template<class T>
void ft::rf::render_frame::t_deleter<T>::operator()(T* p_ptr)
{
    delete p_ptr;
};
template ft::rf::render_frame::t_deleter<ft::rf::render_frame_impl>;
template ft::rf::render_frame::t_deleter<ft::rf::procloop::process_loop>;

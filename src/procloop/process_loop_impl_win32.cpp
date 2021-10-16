// ft_base_lib headers
#include "base/platform.h"
#include "error/ft_assert.h"


#ifdef FT_OS_WINDOWS

#include "process_loop_impl_win32.h"

// Start the worker thread
// Call after construction
void ft::rf::procloop::process_loop_impl::start(const ::HWND p_window)
{
    FT_ASSERT(m_worker.joinable() == false);
    FT_ASSERT(m_thread_id.has_value() == false);

    {   // Start the worker
        const auto fptr = &process_loop_impl::worker_function;
        auto function = [this, fptr]() { (this->*fptr)(); };
        m_worker = std::thread{ std::move(function) };
    }

    {   // Save the worker's thread ID
        const auto thread_handle = m_worker.native_handle();
        m_thread_id = ::GetThreadId(thread_handle);
    }
}


// End the worker thread
// Call before joining
void ft::rf::procloop::process_loop_impl::stop() noexcept
{
    FT_ASSERT(m_worker.joinable());
    FT_ASSERT(m_thread_id.has_value());

    const auto thread_id = m_worker.native_handle();

    ::PostThreadMessageA(m_thread_id.value(), WM_QUIT, 0, 0);
}


// Join a stopped or stopping worker
void ft::rf::procloop::process_loop_impl::join() noexcept
{
    FT_ASSERT(m_worker.joinable());
    FT_ASSERT(m_thread_id.has_value());

    m_worker.join();
    m_thread_id.reset();
}

// Worker entry point
void ft::rf::procloop::process_loop_impl::worker_function()
{
    ::MSG msg;
    ::BOOL result;

    // Loop based on the example here :
    //  https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessagea
    while( (result = ::GetMessageA(&msg, nullptr, 0, 0)) != 0)
    {
        if (result < 0)
        {
            // Failed to get the next message
            break;
        }
        else
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }
    }
}

#endif  // FT_OS_WINDOWS

// project headers
#include "process_loop_impl_win32.h"

// ft_base_lib headers
#include "base/platform.h"
#include "error/ft_assert.h"


#ifdef FT_OS_WINDOWS

// The calling thread will run this processing loop
// It will only return when `stop()` is called
// Sets the `p_ready` promise once the worker has started the loop
void ft::rf::procloop::process_loop_impl::run_loop(
    const ::HWND p_window, std::promise<void>& p_ready)
{
    FT_ASSERT(m_worker_handle.has_value() == false);

    // Save the thread's handle
    m_worker_handle = GetCurrentThreadId();

    ::MSG msg;
    ::BOOL result;

    p_ready.set_value();

    // Loop based on the example here :
    //  https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessagea
    while ((result = ::GetMessageA(&msg, nullptr, 0, 0)) != 0)
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


// End the worker thread
    // Does nothing if no worker thread is running
void ft::rf::procloop::process_loop_impl::stop() noexcept
{
    if (m_worker_handle.has_value())
    {
        ::PostThreadMessageA(m_worker_handle.value(), WM_QUIT, 0, 0);
    }
}

#endif  // FT_OS_WINDOWS

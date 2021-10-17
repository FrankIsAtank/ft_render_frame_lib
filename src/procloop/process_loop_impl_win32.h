#pragma once

// ft_base_lib headers
#include "base/platform.h"
#include "base/windows_include.h"

// standard headers
#include <optional>
#include <future>

#ifdef FT_OS_WINDOWS

namespace ft {
namespace rf {
namespace procloop {

class process_loop_impl
{
public:
    // The calling thread will run this processing loop
    // It will only return when `stop()` is called
    // Sets the `p_ready` promise once the worker has started the loop
    void run_loop(const ::HWND p_window, std::promise<void> & p_ready);

    // End the worker thread
    // Does nothing if no worker thread is running
    void stop() noexcept;

private:
    // Native thread handle to the thread currently running this process loop
    std::optional<DWORD> m_worker_handle;

};  // class process_loop_impl

}   // namespace procloop
}   // namespace rf
}   // namespace ft

#endif  // FT_OS_WINDOWS

#pragma once

// ft_base_lib headers
#include "base/platform.h"
#include "base/windows_include.h"

// standard headers
#include <optional>
#include <thread>

#ifdef FT_OS_WINDOWS

namespace ft {
namespace rf {
namespace procloop {

class process_loop_impl
{
public:
    // Start the worker thread
    // Call after construction
    void start(const ::HWND p_window);

    // End the worker thread
    // Call before joining
    void stop() noexcept;

    // Join a stopped or stopping worker
    void join() noexcept;

private:
    // Worker entry point
    void worker_function();

private:
    // Special HWND handle value to indicate no window
    static inline constexpr auto s_no_window = reinterpret_cast<::HWND>(-1);

private:
    // Worker thread
    std::thread m_worker;

    // WINAPI thread ID of the worker thread
    std::optional<DWORD> m_thread_id;

};  // class process_loop_impl

}   // namespace procloop
}   // namespace rf
}   // namespace ft

#endif  // FT_OS_WINDOWS

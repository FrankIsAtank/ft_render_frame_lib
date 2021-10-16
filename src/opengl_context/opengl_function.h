#pragma once

// Loads opengl functions

// project headers
#include "call_opengl_function.h"
#include "basegl/opengl_except.h"

// other headers
#include "error/ft_assert.h"

namespace ft {
namespace rf {
namespace context {

template<class T>
struct opengl_function
{
    // Implicit construction allowed
    opengl_function(const char* const p_name)
    {
        // Load the function pointer
        using t_exception = err::context_opengl_function_not_found;
        auto pointer = call_opengl_fail_value<t_exception, nullptr>(
            wglGetProcAddress,
            p_name);

        // Cast the pointer
        function = reinterpret_cast<T>(pointer);
    }

    // Call the function
    template<class ... Args>
    auto operator()(Args&& ... p_args)
    {
        FT_ASSERT(function != nullptr);

        return function(std::forward<Args>(p_args)...);
    }

    T function;
};

}   // namespace context
}   // namespace rf
}   // namespace ft

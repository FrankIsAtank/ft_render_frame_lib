#pragma once

// Calls an OpenGL function and checks for errors

// project headers
#include "basegl/opengl_headers.h"
#include "opengl_debug.h"

// standard headers
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>

namespace ft {
namespace rf {

// No specific value means error
template<class Exception, class Function, class ... Args>
auto call_opengl(
    Function p_function,
    Args&& ... p_args)
{
    static_assert(std::is_base_of_v<base::error::except_base, Exception>,
        "Expected an exception type that implements `ft::base::error::except_base\n"
        "See ft::base::error::except_impl");

    // Check if there are any undetected errors
    debug::assert_no_pending_errors();

    constexpr auto is_void = std::is_same<void, decltype(p_function(std::forward<Args>(p_args)...))>{};

    if constexpr (is_void == false)
    {
        auto result = p_function(std::forward<Args>(p_args)...);

        // Check the stored error code
        const auto error = glGetError();
        if (error != GL_NO_ERROR)
        {
            // An OpenGL function shouldn't generate more than 1 error
            debug::assert_no_pending_errors();

            // The function generated an error
            constexpr char error_msg[] =
                __FUNCTION__ " failed by generating an error reported by glGetError";
            Exception::raise(error_msg);
        }

        return result;
    }
    else
    {
        p_function(std::forward<Args>(p_args)...);

        // Check the stored error code
        const auto error = glGetError();
        if (error != GL_NO_ERROR)
        {
            // An OpenGL function shouldn't generate more than 1 error
            debug::assert_no_pending_errors();

            // The function generated an error
            constexpr char error_msg[] =
                __FUNCTION__ " failed by generating an error reported by glGetError";
            Exception::raise(error_msg);
        }
    }
}


// Call an opengl function
// Checks glGetError but swallows them
// Is noexcept to be used in destructors
// Only call cleanup functions that never fail when used correctly
template<class Function, class ... Args>
auto call_opengl_skip_errors(
    Function p_function,
    Args&& ... p_args) noexcept
{
    // Check if there are any undetected errors
    debug::assert_no_pending_errors();

    constexpr auto is_void = std::is_same<void, decltype(p_function(std::forward<Args>(p_args)...))>{};

    if constexpr (is_void == false)
    {
        auto result = p_function(std::forward<Args>(p_args)...);

        // Check the stored error code
        const auto error = glGetError();
        if (error != GL_NO_ERROR)
        {
            // An OpenGL function shouldn't generate more than 1 error
            debug::assert_no_pending_errors();

            // TODO : Something with the error
        }

        return result;
    }
    else
    {
        p_function(std::forward<Args>(p_args)...);

        // Check the stored error code
        if (glGetError() != GL_NO_ERROR)
        {
            // An OpenGL function shouldn't generate more than 1 error
            debug::assert_no_pending_errors();

            // TODO : Something with the error
        }
    }
}


// Calls an opengl function
// Fails if the result if `Error`
template<class Exception, auto Error, class Function, class ... Args>
auto call_opengl_fail_value(
    Function p_function,
    Args&& ... p_args)
{
    static_assert(std::is_base_of_v<base::error::except_base, Exception>,
        "Expected an exception type that implements `ft::base::error::except_base\n"
        "See ft::base::error::except_impl");

    constexpr auto is_void = std::is_same<void, decltype(p_function(std::forward<Args>(p_args)...))>{};
    static_assert(is_void == false, "Can't call void function with expected return value");

    auto result = call_opengl<Exception>(p_function, std::forward<Args>(p_args)...);

    // Check return value for error results
    if (Error == result)
    {
        // There is a result value that is considered an error and that
        //  is the value that was generated
        constexpr char error_msg[] =
            __FUNCTION__ " failed by returning an invalid value";
        Exception::raise(error_msg);
    }
    return result;
}


// Calls an opengl function
// Fails if the result is not `Pass`
template<class Exception, auto Pass, class Function, class ... Args>
auto call_opengl_pass_value(
    Function p_function,
    Args&& ... p_args)
{
    static_assert(std::is_base_of_v<base::error::except_base, Exception>,
        "Expected an exception type that implements `ft::base::error::except_base\n"
        "See ft::base::error::except_impl");

    constexpr auto is_void = std::is_same<void, decltype(p_function(std::forward<Args>(p_args)...))>{};
    static_assert(is_void == false, "Can't call void function with expected return value");

    auto result = call_opengl<Exception>(p_function, std::forward<Args>(p_args)...);

    // Check return value for error results
    if (Pass != result)
    {
        // There is a result value that is considered an error and that
        //  is the value that was generated
        constexpr char error_msg[] =
            __FUNCTION__ " failed by returning an invalid value";
        Exception::raise(error_msg);
    }
    return result;
}

}   // namespace rf
}   // namespace ft

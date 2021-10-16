#pragma once

// project headers
#include "basegl/opengl_headers.h"
#include "basegl/opengl_except.h"

namespace ft {
namespace rf {

namespace context {
    class opengl_context;
}   // namespace context

namespace debug {

// If in debug build, causes a break
//  if glGetError is not GL_NO_ERROR
void assert_no_pending_errors();

// Initializes error debuging
// Sets up error callbacks
void init_debugging(context::opengl_context& p_context);

}   // namespace debug
}   // namespace rf
}   // namespace ft

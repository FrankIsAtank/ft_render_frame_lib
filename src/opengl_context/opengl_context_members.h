#pragma once

// Stores members that require OpenGL headers
#include "basegl/opengl_headers.h"

namespace ft {
namespace rf {
namespace context {

struct opengl_context_members
{
    HDC device_context;
    HGLRC render_context;

};  // struct opengl_context_members

}   // namespace context
}   // namespace rf
}   // namespace ft

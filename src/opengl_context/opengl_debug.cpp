// project headers
#include "call_opengl_function.h"
#include "make_current.h"
#include "opengl_context.h"
#include "opengl_context_members.h"
#include "opengl_debug.h"

// other headers
#include "build/build.h"
#include "build/intrinsic.h"

// standard headers
#include <exception>
namespace {

constexpr auto g_is_debug = ft::base::build::g_build_config == ft::base::build::t_build_configs::debug;

// Function called when an opengl error occurs
void debug_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)  // Pointer to the associated opengl_context object
{
    (void)source;
    (void)type;
    (void)id;
    (void)length;
    (void)message;
    (void)userParam;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
    case GL_DEBUG_SEVERITY_MEDIUM:
        FT_BREAKPOINT
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    default:
        break;
    };
}

}   // anonymous namespace


void ft::rf::debug::assert_no_pending_errors()
{
    // Only used in debug builds
    if constexpr (g_is_debug)
    {
        const auto error = glGetError();
        if (error != GL_NO_ERROR)
        {
            FT_BREAKPOINT;
            std::terminate();
        }
    }
}


// Initializes error debuging
// Sets up error callbacks
void ft::rf::debug::init_debugging(context::opengl_context & p_context)
{
    // Only used in debug builds
    if constexpr (g_is_debug)
    {
        // Make the context active
        auto active = context::make_current(p_context);

        assert_no_pending_errors();

        call_opengl<err::context_init>(glEnable, GL_DEBUG_OUTPUT);
        call_opengl<err::context_init>(glDebugMessageCallback, debug_callback, &p_context);
    }
}

#pragma once

// Platform agnostic header for the platform-dependent component
//  of process_loop

// ft_base_lib headers
#include "base/platform.h"

#ifdef FT_OS_WINDOWS
    #include "process_loop_impl_win32.h"
#else
    #error Unsupported platform
#endif

#pragma once

// Platform agnostic header for the platform-dependent component
//  of render_frame

// other headers
#include "base/platform.h"

#ifdef FT_OS_WINDOWS
    #include "renderframeimpl_win32.h"
#else
    #error Unsupported platform
#endif

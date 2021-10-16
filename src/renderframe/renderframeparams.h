#pragma once

// other projects
#include "vector/vector.hpp"
#include "basegl/color.h"
#include "basegl/pixel_format.h"

// standard headers
#include <string>

namespace ft {
namespace rf {

struct t_render_frame_params
{
    std::string window_name;
    math::vector<int, 2> position = { 0, 0 };
    math::vector<int, 2> size = { 0, 0 };
    gl::color<float> background = { 0.2f, 0.2f, 0.2f };

    // Desired pixel format
    gl::t_pixel_format pixel_format;

};  // struct t_render_frame_params

}   // namespace rf
}   // namespace ft

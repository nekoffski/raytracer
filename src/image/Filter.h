#pragma once

#include "Renderer.h"

namespace image {

struct Filter {
    virtual Renderer::Framebuffer apply(const Renderer::Framebuffer& input) = 0;
};

}  // namespace image

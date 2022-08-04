#pragma once

#include <glm/glm.hpp>

namespace texture {

struct Texture {
    virtual glm::vec3 getColor(float u, float v, const glm::vec3& p) const = 0;
};

}  // namespace texture

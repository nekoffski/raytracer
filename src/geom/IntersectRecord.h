#pragma once

#include <glm/glm.hpp>

#include "mat/Material.h"

namespace geom {

struct IntersectRecord {
    glm::vec3 hitPoint;
    glm::vec3 normal;

    float t;
    float u;
    float v;

    mat::Material* material;
};

}  // namespace geom
#pragma once

#include <kc/math/Ray.h>
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

inline glm::vec3 calculateNormal(const kc::math::Ray& r, const glm::vec3& normal) {
    return glm::dot(r.getDirection(), normal) < 0 ? normal : -normal;
}

}  // namespace geom
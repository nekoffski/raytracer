#pragma once

#include <vector>

#include "Intersectable.h"
#include "Triangle.h"

namespace geom {

struct TriangleMesh : Intersectable {
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;

    glm::vec3 color;

    const glm::vec3 &getColor() const override;
    std::optional<HitRecord> intersect(const kc::math::Ray &ray) const override;
    glm::vec3 getNormal(const glm::vec3 &hitPoint) const override;
};

}  // namespace geom
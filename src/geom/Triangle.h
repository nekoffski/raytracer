#pragma once

#include <vector>

#include "Intersectable.h"

namespace geom {

struct Triangle : Intersectable {
    std::vector<glm::vec3> vertices;

    glm::vec3 color;

    const glm::vec3 &getColor() const override;
    std::optional<HitRecord> intersect(const kc::math::Ray &ray) const;

    glm::vec3 getNormal(const glm::vec3 &hitPoint) const;

    static std::optional<HitRecord> intersectTriangle(
        const std::vector<glm::vec3> &vertices, const kc::math::Ray &ray
    );

    static std::optional<HitRecord> intersectTriangle(
        const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
        const kc::math::Ray &ray
    );
};

}  // namespace geom
#pragma once

#include <optional>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

namespace geom {

struct Intersectable {
    struct HitRecord {
        float t;
    };

    virtual glm::vec2 getUV(const glm::vec3 &p) const { return glm::vec2{1.0f}; }
    virtual std::optional<HitRecord> intersect(const kc::math::Ray &ray) const = 0;
    virtual const glm::vec3 &getColor() const                                  = 0;
    virtual glm::vec3 getNormal(const glm::vec3 &hitPoint) const               = 0;
};

}  // namespace geom
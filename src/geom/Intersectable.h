#pragma once

#include <optional>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

#include "Util.h"

namespace geom {

struct Intersectable {
    struct HitRecord {
        float t;
    };

    virtual std::optional<HitRecord> intersect(const kc::math::Ray &ray) const ABSTRACT;
    virtual const glm::vec3 &getColor() const ABSTRACT;
    virtual glm::vec3 getNormal(const glm::vec3 &hitPoint) const ABSTRACT;
};

}  // namespace geom
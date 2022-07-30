#pragma once

#include <optional>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

#include "IntersectRecord.h"
#include "bvh/AABB.h"

namespace geom {

struct Intersectable {
    virtual ~Intersectable() = default;

    virtual std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) = 0;

    virtual bvh::AABB getBoundingBox() const = 0;
};

}  // namespace geom

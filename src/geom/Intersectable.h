#pragma once

#include <optional>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

#include "IntersectRecord.h"

namespace geom {

struct Intersectable {
    virtual std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) = 0;
};

}  // namespace geom

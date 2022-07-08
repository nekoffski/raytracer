#pragma once

#include <optional>

#include <kc/math/Ray.h>
#include <glm/glm.hpp>

#include "geom/fwd.h"

namespace mat {

struct ScatterRecord {
    kc::math::Ray ray;
    glm::vec3 attenuation;
};

struct Material {
    virtual std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    ) = 0;
};

}  // namespace mat
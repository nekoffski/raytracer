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

    virtual glm::vec3 emit(
        [[maybe_unused]] float u, [[maybe_unused]] float v,
        [[maybe_unused]] const glm::vec3& p
    ) const {
        return glm::vec3{0.0f};
    }
};

}  // namespace mat
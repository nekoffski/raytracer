#include "Lambertian.h"

#include <kc/math/Utils.hpp>

namespace mat {

Lambertian::Lambertian(const glm::vec3& albedo) : m_albedo(albedo) {}

std::optional<ScatterRecord> Lambertian::scatter(
    const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
) {
    // TODO: scatter with some probability p
    auto rayDirection = hitRecord.normal + kc::math::randomNormalVec3();

    static constexpr float delta = 0.0001f;

    if (glm::length(rayDirection) < delta) rayDirection = hitRecord.normal;

    kc::math::Ray scatteredRay{hitRecord.hitPoint, rayDirection};
    return ScatterRecord{.ray = scatteredRay, .attenuation = m_albedo};
}

}  // namespace mat

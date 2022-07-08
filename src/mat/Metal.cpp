#include "Metal.h"

#include <kc/math/Utils.hpp>

namespace mat {

Metal::Metal(const glm::vec3& albedo, float fuzziness)
    : m_albedo(albedo), m_fuzziness(std::min(1.0f, std::max(0.0f, fuzziness))) {}

std::optional<ScatterRecord> Metal::scatter(
    const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
) {
    auto reflectedRay =
        glm::reflect(glm::normalize(ray.getDirection()), hitRecord.normal);
    auto fuzzyVector = m_fuzziness * kc::math::randomUnitSphereVec3();

    kc::math::Ray scatteredRay{hitRecord.hitPoint, reflectedRay + fuzzyVector};

    if (glm::dot(reflectedRay, hitRecord.normal) <= 0) return {};

    return ScatterRecord{.ray = scatteredRay, .attenuation = m_albedo};
}

}  // namespace mat
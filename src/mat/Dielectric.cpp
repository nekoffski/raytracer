#include "Dielectric.h"

#include <kc/math/Utils.hpp>

namespace {

float reflectance(float cosine, float refIndex) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1.0f - refIndex) / (1.0f + refIndex);
    r0      = r0 * r0;
    return r0 + (1.0f - r0) * std::pow((1 - cosine), 5);
}

}  // namespace

namespace mat {

Dielectric::Dielectric(float indexOfRefraction)
    : m_indexOfRefraction(indexOfRefraction) {}

std::optional<ScatterRecord> Dielectric::scatter(
    const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
) {
    const auto refractionRatio = 1.0f / m_indexOfRefraction;

    auto unitDirection = glm::normalize(ray.getDirection());

    float cosTheta = std::min(1.0f, glm::dot(-unitDirection, hitRecord.normal));
    float sinTheta = std::sqrt(1 - cosTheta * cosTheta);

    auto newDirection =
        (refractionRatio * sinTheta > 1.0f ||
         reflectance(cosTheta, refractionRatio) > kc::math::random<float>())
            ? glm::reflect(unitDirection, hitRecord.normal)
            : glm::refract(unitDirection, hitRecord.normal, refractionRatio);

    kc::math::Ray scaterredRay{hitRecord.hitPoint, newDirection};
    return ScatterRecord{scaterredRay, glm::vec3{1.0f}};
}

}  // namespace mat

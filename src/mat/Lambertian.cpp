#include "Lambertian.h"

#include <kc/math/Utils.hpp>

namespace mat {

Lambertian::Lambertian(const glm::vec3& color)
    : m_solidAlbedo(texture::Solid{color}), m_albedo(std::addressof(*m_solidAlbedo)) {}

Lambertian::Lambertian(texture::Texture* albedo) : m_albedo(albedo) {}

std::optional<ScatterRecord> Lambertian::scatter(
    const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
) {
    // TODO: scatter with some probability p
    auto rayDirection =
        hitRecord.normal + kc::math::randomUnitHemisphereVec3(hitRecord.normal);

    static constexpr float delta = 0.0001f;

    if (glm::length(rayDirection) < delta) rayDirection = hitRecord.normal;

    kc::math::Ray scatteredRay{hitRecord.hitPoint, rayDirection};

    const auto attenuation =
        m_albedo->getColor(hitRecord.u, hitRecord.v, hitRecord.hitPoint);

    return ScatterRecord{
        .ray         = scatteredRay,
        .attenuation = attenuation,
    };
}

}  // namespace mat

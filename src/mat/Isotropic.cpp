#include "Isotropic.h"

#include <kc/math/Utils.hpp>

#include "geom/IntersectRecord.h"

namespace mat {

Isotropic::Isotropic(texture::Texture* albedo) : m_albedo(albedo) {}

std::optional<ScatterRecord> Isotropic::scatter(
    const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
) {
    return ScatterRecord{
        .ray = kc::math::Ray{hitRecord.hitPoint, kc::math::randomUnitSphereVec3()},
        .attenuation = m_albedo->getColor(hitRecord.u, hitRecord.v, hitRecord.hitPoint)
    };
}

}  // namespace mat
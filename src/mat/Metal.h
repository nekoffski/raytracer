#pragma once

#include <kc/math/Utils.hpp>

#include "geom/Hittable.h"
#include "Material.h"

namespace mat {

class Metal : public Material {
   public:
    explicit Metal(const glm::vec3& albedo, float fuzziness)
        : m_albedo(albedo), m_fuzziness(std::min(1.0f, std::max(0.0f, fuzziness))) {}

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::HitRecord& hitRecord
    ) override {
        auto reflectedRay =
            glm::reflect(glm::normalize(ray.getDirection()), hitRecord.normal);
        auto fuzzyVector = m_fuzziness * kc::math::randomUnitSphereVec3();

        kc::math::Ray scatteredRay{hitRecord.hitPoint, reflectedRay + fuzzyVector};

        if (glm::dot(reflectedRay, hitRecord.normal) <= 0) return {};

        return ScatterRecord{.ray = scatteredRay, .attenuation = m_albedo};
    }

   private:
    float m_fuzziness;
    glm::vec3 m_albedo;
};

}  // namespace mat
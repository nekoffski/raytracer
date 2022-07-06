#pragma once

#include <kc/math/Utils.hpp>

#include "geom/Hittable.h"
#include "Material.h"

namespace mat {

// diffuse
class Lambertian : public Material {
   public:
    explicit Lambertian(const glm::vec3& albedo) : m_albedo(albedo) {}

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::HitRecord& hitRecord
    ) override {
        // TODO: scatter with some probability p
        auto rayDirection = hitRecord.normal + kc::math::randomNormalVec3();

        static constexpr float delta = 0.0001f;

        if (glm::length(rayDirection) < delta) rayDirection = hitRecord.normal;

        kc::math::Ray scatteredRay{hitRecord.hitPoint, rayDirection};
        return ScatterRecord{.ray = scatteredRay, .attenuation = m_albedo};
    }

   private:
    glm::vec3 m_albedo;
};

}  // namespace mat

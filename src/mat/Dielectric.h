#pragma once

#include <kc/math/Utils.hpp>

#include "geom/Hittable.h"
#include "Material.h"

namespace mat {

class Dielectric : public Material {
   public:
    explicit Dielectric(float indexOfRefraction)
        : m_indexOfRefraction(indexOfRefraction) {}

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::HitRecord& hitRecord
    ) override {
        auto unitDirection = glm::normalize(ray.getDirection());
        auto refracted =
            glm::refract(unitDirection, hitRecord.normal, 1.0f / m_indexOfRefraction);

        kc::math::Ray scaterredRay{hitRecord.hitPoint, refracted};

        return ScatterRecord{scaterredRay, glm::vec3{1.0f}};
    }

   private:
    float m_indexOfRefraction;
};

}  // namespace mat

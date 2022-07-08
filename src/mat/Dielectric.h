#pragma once

#include "geom/Intersectable.h"
#include "Material.h"

namespace mat {

class Dielectric : public Material {
   public:
    explicit Dielectric(float indexOfRefraction);

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    ) override;

   private:
    float m_indexOfRefraction;
};

}  // namespace mat

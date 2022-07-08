#pragma once

#include "geom/IntersectRecord.h"
#include "Material.h"

namespace mat {

// diffuse
class Lambertian : public Material {
   public:
    explicit Lambertian(const glm::vec3& albedo);

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    ) override;

   private:
    glm::vec3 m_albedo;
};

}  // namespace mat

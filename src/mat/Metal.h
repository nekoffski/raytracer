#pragma once

#include "geom/IntersectRecord.h"
#include "Material.h"

namespace mat {

class Metal : public Material {
   public:
    explicit Metal(const glm::vec3& albedo, float fuzziness);

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    );

   private:
    float m_fuzziness;
    glm::vec3 m_albedo;
};

}  // namespace mat
#pragma once

#include <glm/glm.hpp>

#include "Intersectable.h"

namespace geom {

class Sphere : public Intersectable {
   public:
    struct Params {
        glm::vec3 center;
        glm::vec3 color;
        float r;
    };

    explicit Sphere(const Params& params);
    const glm::vec3& getColor() const override;

    glm::vec2 getUV(const glm::vec3& hitPoint) const override;
    glm::vec3 getNormal(const glm::vec3& hitPoint) const override;

    std::optional<Intersectable::HitRecord> intersect(const kc::math::Ray& ray
    ) const override;

   private:
    Params m_params;
};

}  // namespace geom
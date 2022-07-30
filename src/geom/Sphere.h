#pragma once

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "Intersectable.h"

namespace geom {

class Sphere : public Intersectable {
   public:
    explicit Sphere(const glm::vec3& position, float radius, mat::Material* material);

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override;

    bvh::AABB getBoundingBox() const override;

   private:
    glm::vec3 m_position;
    float m_radius;
    mat::Material* m_material;
};

}  // namespace geom

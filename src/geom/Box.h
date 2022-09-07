#pragma once

#include <memory>

#include "Intersectable.h"
#include "IntersectableCollection.h"

#include "mat/Material.h"

namespace geom {

class Box : public Intersectable {
   public:
    explicit Box(const glm::vec3& min, const glm::vec3& max, mat::Material* material);

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override;

    bvh::AABB getBoundingBox() const override;

   private:
    glm::vec3 m_min;
    glm::vec3 m_max;

    IntersectableCollection m_walls;
    std::vector<std::unique_ptr<Intersectable>> m_wallsInstances;
};

}  // namespace geom
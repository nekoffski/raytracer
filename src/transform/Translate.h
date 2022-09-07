#pragma once

#include "geom/Intersectable.h"

namespace transform {

class Translate : public geom::Intersectable {
   public:
    explicit Translate(const glm::vec3& offset, geom::Intersectable* root)
        : m_offset(offset), m_root(root) {}

    std::optional<geom::IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override {
        kc::math::Ray translatedRay{ray.getOrigin() - m_offset, ray.getDirection()};

        auto hitRecord = m_root->intersect(translatedRay, min, max);

        if (not hitRecord) return {};

        hitRecord->hitPoint += m_offset;
        hitRecord->normal = geom::calculateNormal(translatedRay, hitRecord->normal);

        return hitRecord;
    }

    bvh::AABB getBoundingBox() const override {
        auto rootBoundingBox = m_root->getBoundingBox();

        return bvh::AABB{
            rootBoundingBox.getMin() + m_offset,
            rootBoundingBox.getMax() + m_offset,
        };
    }

   private:
    geom::Intersectable* m_root;
    glm::vec3 m_offset;
};

}  // namespace transform
#pragma once

#include "geom/Intersectable.h"

namespace bvh {

class Node : public geom::Intersectable {
   public:
    std::optional<geom::IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override {
        if (not m_boundingBox.hit(ray, min, max)) return {};

        auto hit = m_left->intersect(ray, min, max);

        //  Left and right may overlap
        if (auto rightHit = m_right->intersect(ray, min, hit ? hit->t : max); rightHit)
            hit = rightHit;

        return hit;
    }

    AABB getBoundingBox() const override { return m_boundingBox; }

   private:
    AABB m_boundingBox;

    geom::Intersectable* m_left;
    geom::Intersectable* m_right;
};

}  // namespace bvh

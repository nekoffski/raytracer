#pragma once

#include <vector>

#include "geom/Intersectable.h"
#include "bvh/Node.h"

namespace bvh {

class Node : public geom::Intersectable {
   public:
    explicit Node(
        const std::vector<geom::Intersectable*>& objects, uint64_t start = 0,
        uint64_t end = 1
    );

    std::optional<geom::IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override;

    AABB getBoundingBox() const override;

   private:
    AABB m_boundingBox;

    geom::Intersectable* m_left;
    geom::Intersectable* m_right;

    std::vector<Node> m_nodes;
};

}  // namespace bvh

#pragma once

#include <vector>
#include <optional>

#include "Intersectable.h"
#include "IntersectRecord.h"
#include "bvh/Node.h"

namespace geom {

class IntersectableCollection : public Intersectable {
   public:
    template <typename... Args> void addObjects(Args&&... args) { (add(args), ...); }

    void clear();

    void add(Intersectable* object);

    bvh::Node getBoundingVolume() const {
        return bvh::Node{m_objects, 0, m_objects.size()};
    }

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override;

    bvh::AABB getBoundingBox() const override;

   private:
    std::vector<Intersectable*> m_objects;
};

}  // namespace geom
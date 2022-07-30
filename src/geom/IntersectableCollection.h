#pragma once

#include <vector>
#include <optional>

#include "Intersectable.h"
#include "IntersectRecord.h"

namespace geom {

class IntersectableCollection : public Intersectable {
   public:
    template <typename... Args> void addObjects(Args&&... args) { (add(args), ...); }

    void clear();

    void add(Intersectable* object);

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override;

    bvh::AABB getBoundingBox() const override;

   private:
    std::vector<Intersectable*> m_objects;
};

}  // namespace geom
#pragma once

#include <optional>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

#include "IntersectRecord.h"
#include "bvh/AABB.h"

namespace geom {

struct Intersectable {
    virtual ~Intersectable() = default;

    virtual std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) = 0;

    virtual bvh::AABB getBoundingBox() const = 0;

    virtual mat::Material* getMaterial() const { return nullptr; }
};

class IntersectableDecorator : public Intersectable {
   public:
    explicit IntersectableDecorator(Intersectable* root) : m_root(root) {}

    mat::Material* getMaterial() const override { return m_root->getMaterial(); }

   protected:
    Intersectable* m_root;
};

}  // namespace geom

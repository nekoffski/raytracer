#pragma once

#include <kc/math/Utils.hpp>
#include <kc/core/Log.h>

#include "Intersectable.h"

#include "mat/Isotropic.h"
#include "mat/Lambertian.h"

namespace geom {

class ConstantMedium : public Intersectable {
    static texture::Texture* getAlbedo(const Intersectable* boundary) {
        const auto material =
            dynamic_cast<const mat::Lambertian*>(boundary->getMaterial());

        ASSERT(
            material != nullptr,
            "To apply ConstantMedium vfx the object must provide Lambertian material"
        );

        return material->getAlbedo();
    }

   public:
    explicit ConstantMedium(Intersectable* boundary, float density)
        : m_boundary(boundary)
        , m_negativeInversedDensity(-1.0f / density)
        , m_phaseFunction(getAlbedo(boundary)) {}

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override {
        std::optional<IntersectRecord> h1, h2;

        if (h1 = m_boundary->intersect(ray, -INFINITY, INFINITY); not h1) return {};

        if (h2 = m_boundary->intersect(ray, h1->t + 0.0001f, INFINITY); not h2) return {};

        h1->t = std::max(h1->t, min);
        h2->t = std::min(h2->t, max);

        if (h1->t >= h2->t) return {};

        h1->t = std::max(0.0f, h1->t);

        const auto rayLength      = glm::length(ray.getDirection());
        const auto distanceInside = (h2->t - h1->t) * rayLength;
        const auto hitDistance =
            m_negativeInversedDensity * std::log(kc::math::random<float>());

        if (hitDistance > distanceInside) return {};

        const auto t = h1->t + hitDistance / rayLength;

        return IntersectRecord{
            .hitPoint = ray.at(t),
            .normal   = glm::vec3{1.0f, 0.0f, 0.0f},
            .t        = t,
            .u        = 0.0f,
            .v        = 0.0f,
            .material = &m_phaseFunction
        };
    }

    bvh::AABB getBoundingBox() const override { return m_boundary->getBoundingBox(); }

   private:
    Intersectable* m_boundary;
    float m_negativeInversedDensity;

    mat::Isotropic m_phaseFunction;
};

}  // namespace geom
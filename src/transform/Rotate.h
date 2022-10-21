#pragma once

#include <cmath>

#include <glm/glm.hpp>

#include "geom/Intersectable.h"

namespace transform {

class RotateY : public geom::Intersectable {
   public:
    explicit RotateY(float angle, geom::Intersectable* root)
        : m_angle(angle), m_root(root) {
        const auto radians = glm::radians(m_angle);
        m_sinTheta         = std::sin(radians);
        m_cosTheta         = std::cos(radians);

        auto boundingBox = root->getBoundingBox();

        glm::vec3 min{INFINITY, INFINITY, INFINITY};
        glm::vec3 max = -min;

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    auto x =
                        i * boundingBox.getMax().x + (1 - i) * boundingBox.getMin().x;
                    auto y =
                        j * boundingBox.getMax().y + (1 - j) * boundingBox.getMin().y;
                    auto z =
                        k * boundingBox.getMax().z + (1 - k) * boundingBox.getMin().z;

                    auto newX = m_cosTheta * x + m_sinTheta * z;
                    auto newZ = -m_sinTheta * x + m_cosTheta * z;

                    glm::vec3 tester{newX, y, newZ};

                    for (int c = 0; c < 3; ++c) {
                        min[c] = std::min(min[c], tester[c]);
                        max[c] = std::max(max[c], tester[c]);
                    }
                }
            }
        }

        m_boundingBox = bvh::AABB{min, max};
    }

    std::optional<geom::IntersectRecord> intersect(
        const kc::math::Ray& ray, float min, float max
    ) override {
        auto origin    = ray.getOrigin();
        auto direction = ray.getDirection();

        origin[0] = m_cosTheta * ray.getOrigin()[0] - m_sinTheta * ray.getOrigin()[2];
        origin[2] = m_sinTheta * ray.getOrigin()[0] + m_cosTheta * ray.getOrigin()[2];

        direction[0] =
            m_cosTheta * ray.getDirection()[0] - m_sinTheta * ray.getDirection()[2];
        direction[2] =
            m_sinTheta * ray.getDirection()[0] + m_cosTheta * ray.getDirection()[2];

        kc::math::Ray rotatedRay(origin, direction);

        auto hitRecord = m_root->intersect(rotatedRay, min, max);

        if (not hitRecord) return {};

        auto hitPoint = hitRecord->hitPoint;
        auto normal   = hitRecord->normal;

        hitPoint[0] =
            m_cosTheta * hitRecord->hitPoint[0] + m_sinTheta * hitRecord->hitPoint[2];
        hitPoint[2] =
            -m_sinTheta * hitRecord->hitPoint[0] + m_cosTheta * hitRecord->hitPoint[2];

        normal[0] = m_cosTheta * hitRecord->normal[0] + m_sinTheta * hitRecord->normal[2];
        normal[2] =
            -m_sinTheta * hitRecord->normal[0] + m_cosTheta * hitRecord->normal[2];

        hitRecord->hitPoint = hitPoint;
        hitRecord->normal   = geom::calculateNormal(rotatedRay, normal);

        return hitRecord;
    }

    mat::Material* getMaterial() const override { return m_root->getMaterial(); }

    bvh::AABB getBoundingBox() const override { return m_boundingBox; }

   private:
    float m_angle;
    float m_cosTheta;
    float m_sinTheta;

    geom::Intersectable* m_root;

    bvh::AABB m_boundingBox;
};

}  // namespace transform
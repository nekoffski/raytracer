#pragma once

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "Hittable.h"

namespace geom {

class Sphere : public Hittable {
   public:
    explicit Sphere(const glm::vec3& position, float radius, mat::Material* material)
        : m_position(position), m_radius(radius), m_material(material) {}

    std::optional<HitRecord> hit(const kc::math::Ray& ray, float min, float max)
        override {
        auto L  = ray.getOrigin() - m_position;
        float a = glm::dot(ray.getDirection(), ray.getDirection());

        float b = 2 * glm::dot(ray.getDirection(), L);
        float c = glm::dot(L, L) - m_radius * m_radius;

        auto roots = kc::math::solveQuadraticEquation(a, b, c);

        if (not roots) return {};

        auto [t0, t1] = *roots;

        if (t0 > t1) std::swap(t0, t1);

        if (t0 < 0) {
            t0 = t1;                // if t0 is negative, let's use t1 instead
            if (t0 < 0) return {};  // both t0 and t1 are negative
        }

        if (t0 < min || max < t0) return {};

        auto hitPoint = ray.at(t0);

        return HitRecord{
            .hitPoint = hitPoint,
            .normal   = glm::normalize(hitPoint - m_position),
            .t        = t0,
            .material = m_material};
    }

   private:
    glm::vec3 m_position;
    float m_radius;
    mat::Material* m_material;
};

}  // namespace geom
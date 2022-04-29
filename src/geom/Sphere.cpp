#include "Sphere.h"

#include <kc/math/Utils.hpp>

namespace geom {

Sphere::Sphere(const Params& params) : m_params(params) {}

const glm::vec3& Sphere::getColor() const { return m_params.color; }

glm::vec3 Sphere::getNormal(const glm::vec3& hitPoint) const {
    return glm::normalize(hitPoint - m_params.center);
}

std::optional<Intersectable::HitRecord> Sphere::intersect(const kc::math::Ray& ray
) const {
    // analytic solution
    auto L  = ray.getOrigin() - m_params.center;
    float a = glm::dot(ray.getDirection(), ray.getDirection());

    float b = 2 * glm::dot(ray.getDirection(), L);
    float c = glm::dot(L, L) - m_params.r * m_params.r;

    auto roots = kc::math::solveQuadraticEquation(a, b, c);

    if (not roots) return {};

    auto [t0, t1] = *roots;

    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1;                // if t0 is negative, let's use t1 instead
        if (t0 < 0) return {};  // both t0 and t1 are negative
    }

    return Intersectable::HitRecord{t0};
}

}  // namespace geom
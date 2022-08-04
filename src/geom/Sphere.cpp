#include "Sphere.h"

namespace geom {

Sphere::Sphere(const glm::vec3& position, float radius, mat::Material* material)
    : m_position(position), m_radius(radius), m_material(material) {}

bvh::AABB Sphere::getBoundingBox() const {
    auto diagonal = glm::vec3{m_radius};
    return bvh::AABB{m_position - diagonal, m_position + diagonal};
}

std::pair<float, float> getUV(const glm::vec3& point) {
    auto theta = std::acos(-point.y);
    auto phi   = std::atan2(-point.z, point.x) + M_PI;

    return {phi / (2.0f * M_PI), theta / M_PI};
}

std::optional<IntersectRecord> Sphere::intersect(
    const kc::math::Ray& ray, float min, float max
) {
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

    const auto hitPoint = ray.at(t0);
    const auto normal   = glm::normalize(hitPoint - m_position);
    const auto [u, v]   = getUV(normal);

    return IntersectRecord{
        .hitPoint = hitPoint,
        .normal   = normal,
        .t        = t0,
        .u        = u,
        .v        = v,
        .material = m_material,
    };
}

}  // namespace geom
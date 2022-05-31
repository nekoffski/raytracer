#include "Triangle.h"

namespace geom {

const glm::vec3 &Triangle::getColor() const { return color; }

std::optional<Intersectable::HitRecord> Triangle::intersect(const kc::math::Ray &ray
) const {
    return intersectTriangle(vertices, ray);
}

glm::vec3 Triangle::getNormal(const glm::vec3 &hitPoint) const {
    return glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[0]);
}

std::optional<Intersectable::HitRecord> Triangle::intersectTriangle(
    const std::vector<glm::vec3> &vertices, const kc::math::Ray &ray
) {
    return intersectTriangle(vertices[0], vertices[1], vertices[2], ray);
}

std::optional<Intersectable::HitRecord> Triangle::intersectTriangle(
    const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
    const kc::math::Ray &ray
) {
    const auto origin    = ray.getOrigin();
    const auto direction = ray.getDirection();

    auto v0v1 = v1 - v0;
    auto v0v2 = v2 - v0;

    auto pvec = glm::cross(direction, v0v2);
    float det = glm::dot(v0v1, pvec);

    constexpr float epsilon = 0.0001f;

    if (std::fabs(det) < epsilon)  // ray is parallel
        return {};

    float invDet = 1.0f / det;

    auto tvec = origin - v0;

    float u = glm::dot(tvec, pvec) * invDet;

    if (u < 0 || u > 1) return {};

    auto qvec = glm::cross(tvec, v0v1);
    float v   = glm::dot(direction, qvec) * invDet;

    if (v < 0 || u + v > 1) return {};

    return HitRecord{glm::dot(v0v2, qvec) * invDet};
}

}  // namespace geom
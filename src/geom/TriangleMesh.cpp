#include "TriangleMesh.h"

namespace geom {

const glm::vec3 &TriangleMesh::getColor() const { return color; }

std::optional<Intersectable::HitRecord> TriangleMesh::intersect(const kc::math::Ray &ray
) const {
    std::optional<HitRecord> hitRecord;
    float nearest = std::numeric_limits<float>::max();

    for (int i = 0; i < indices.size(); i += 3) {
        auto triangleHitRecord = Triangle::intersectTriangle(
            vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], ray
        );

        if (triangleHitRecord && triangleHitRecord->t < nearest) {
            hitRecord = triangleHitRecord;
            nearest   = triangleHitRecord->t;
        }
    }

    return hitRecord;
}

glm::vec3 TriangleMesh::getNormal(const glm::vec3 &hitPoint) const {
    return glm::vec3{1.0f};
}

}  // namespace geom
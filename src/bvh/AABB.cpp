#include "AABB.h"

namespace bvh {

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max) {}

const glm::vec3& AABB::getMin() const { return m_min; }
const glm::vec3& AABB::getMax() const { return m_max; }

bool AABB::hit(const kc::math::Ray& ray, float tMin, float tMax) const {
    static constexpr uint32_t dimensions = 3;

    // TODO: optimize

    auto direction = ray.getDirection();
    auto origin    = ray.getOrigin();

    for (int i = 0; i < dimensions; ++i) {
        auto l = (m_min[i] - origin[i]) / direction[i];
        auto r = (m_max[i] - origin[i]) / direction[i];

        auto t0 = std::min(l, r);
        auto t1 = std::max(l, r);

        tMin = std::max(t0, tMin);
        tMax = std::min(t1, tMax);

        if (tMax < tMin) return false;
    }

    return true;
}

AABB AABB::mergeWith(const AABB& oth) const {
    const auto [min, max] = getNewBoundingBoxMinMax(oth);
    return AABB{min, max};
}

void AABB::mergeInPlaceWith(const AABB& oth) {
    std::tie(m_min, m_max) = getNewBoundingBoxMinMax(oth);
}

std::pair<glm::vec3, glm::vec3> AABB::getNewBoundingBoxMinMax(const AABB& oth) const {
    auto min = glm::vec3{
        std::min(m_min.x, oth.m_min.x),
        std::min(m_min.y, oth.m_min.y),
        std::min(m_min.z, oth.m_min.z),
    };

    auto max = glm::vec3{
        std::max(m_max.x, oth.m_max.x),
        std::max(m_max.y, oth.m_max.y),
        std::max(m_max.z, oth.m_max.z),
    };

    return {min, max};
}

}  // namespace bvh

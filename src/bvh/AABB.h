#pragma once

#include <tuple>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

namespace bvh {

class AABB {
   public:
    AABB() = default;
    explicit AABB(const glm::vec3& min, const glm::vec3& max);

    const glm::vec3& getMin() const;
    const glm::vec3& getMax() const;

    bool hit(const kc::math::Ray& ray, float tMin, float tMax) const;

    AABB mergeWith(const AABB& oth) const;
    void mergeInPlaceWith(const AABB& oth);

   private:
    std::pair<glm::vec3, glm::vec3> getNewBoundingBoxMinMax(const AABB& oth) const;

    glm::vec3 m_min;
    glm::vec3 m_max;
};

}  // namespace bvh
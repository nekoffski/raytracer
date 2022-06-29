#pragma once

#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

namespace geom {

struct HitRecord {
    glm::vec3 hitPoint;
    glm::vec3 normal;
    float t;
};

struct Hittable {
    virtual std::optional<HitRecord> hit(
        const kc::math::Ray& ray, float min, float max
    ) = 0;
};

class HittableCollection : public Hittable {
   public:
    void clear() { m_objects.clear(); }

    void add(Hittable* object) { m_objects.push_back(object); }

    std::optional<HitRecord> hit(const kc::math::Ray& ray, float min, float max)
        override {
        std::optional<HitRecord> hitRecord;
        float closest = std::numeric_limits<float>::max();

        for (auto& object : m_objects) {
            if (auto result = object->hit(ray, min, closest); result) {
                closest   = result->t;
                hitRecord = *result;
            }
        }
        return hitRecord;
    }

   private:
    std::vector<Hittable*> m_objects;
};

}  // namespace geom
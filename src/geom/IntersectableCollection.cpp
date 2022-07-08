#include "IntersectableCollection.h"

namespace geom {

void IntersectableCollection::clear() { m_objects.clear(); }

void IntersectableCollection::add(Intersectable* object) { m_objects.push_back(object); }

std::optional<IntersectRecord> IntersectableCollection::intersect(
    const kc::math::Ray& ray, float min, float max
) {
    std::optional<IntersectRecord> IntersectRecord;
    float closest = std::numeric_limits<float>::max();

    for (auto& object : m_objects) {
        if (auto result = object->intersect(ray, min, closest); result) {
            closest         = result->t;
            IntersectRecord = *result;
        }
    }
    return IntersectRecord;
}

}  // namespace geom

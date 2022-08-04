#include "Node.h"

#include <kc/core/Log.h>
#include <kc/math/Utils.hpp>

namespace bvh {

namespace {

inline bool compareBoxes(geom::Intersectable* a, geom::Intersectable* b, int axis) {
    ASSERT(0 <= axis && axis < 3, fmt::format("Invalid axis - {}", axis));

    auto boxA = a->getBoundingBox();
    auto boxB = b->getBoundingBox();

    return boxA.getMin()[axis] < boxB.getMin()[axis];
}

inline bool compareBoxesX(geom::Intersectable* a, geom::Intersectable* b) {
    return compareBoxes(a, b, 0);
}

inline bool compareBoxesY(geom::Intersectable* a, geom::Intersectable* b) {
    return compareBoxes(a, b, 1);
}

inline bool compareBoxesZ(geom::Intersectable* a, geom::Intersectable* b) {
    return compareBoxes(a, b, 2);
}

inline auto getComparator(int axis) {
    return axis == 0 ? compareBoxesX : (axis == 1 ? compareBoxesY : compareBoxesZ);
}

}  // namespace

Node::Node(
    const std::vector<geom::Intersectable*>& objects, uint64_t start, uint64_t end
) {
    std::vector<geom::Intersectable*> obj = objects;

    auto axis       = kc::math::random<int>(0, 2);
    auto comparator = getComparator(axis);
    auto span       = end - start;

    if (span == 1) {
        m_left = m_right = obj[start];
    } else if (span == 2) {
        m_left  = obj[start];
        m_right = obj[start + 1];

        if (not comparator(m_left, m_right)) {
            using std::swap;
            swap(m_left, m_right);
        }
    } else {
        std::sort(obj.begin() + start, obj.begin() + end, comparator);

        auto mid = start + span / 2;

        m_nodes.reserve(2);

        m_nodes.emplace_back(obj, start, mid);
        m_left = &m_nodes.back();

        m_nodes.emplace_back(obj, mid, end);
        m_right = &m_nodes.back();
    }

    ASSERT(m_left && m_right, "BVH nodes are not set");

    auto leftBox  = m_left->getBoundingBox();
    auto rightBox = m_right->getBoundingBox();

    m_boundingBox = leftBox.mergeWith(rightBox);
}

std::optional<geom::IntersectRecord> Node::intersect(
    const kc::math::Ray& ray, float min, float max
) {
    if (not m_boundingBox.hit(ray, min, max)) return {};

    auto hit = m_left->intersect(ray, min, max);

    //  Left and right may overlap
    if (auto rightHit = m_right->intersect(ray, min, hit ? hit->t : max); rightHit)
        hit = rightHit;

    return hit;
}

AABB Node::getBoundingBox() const { return m_boundingBox; }

}  // namespace bvh

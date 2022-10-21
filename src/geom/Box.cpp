#include "Box.h"

#include "Rectangle.h"

namespace geom {

Box::Box(const glm::vec3& min, const glm::vec3& max, mat::Material* material)
    : m_min(min), m_max(max), m_material(material) {
    static constexpr int wallsCount = 6;
    m_wallsInstances.reserve(wallsCount);

    m_wallsInstances.push_back(std::make_unique<RectangleXY>(
        m_min.x, m_max.x, m_min.y, m_max.y, m_max.z, material
    ));
    m_wallsInstances.push_back(std::make_unique<RectangleXY>(
        m_min.x, m_max.x, m_min.y, m_max.y, m_min.z, material
    ));

    m_wallsInstances.push_back(std::make_unique<RectangleXZ>(
        m_min.x, m_max.x, m_min.z, m_max.z, m_max.y, material
    ));
    m_wallsInstances.push_back(std::make_unique<RectangleXZ>(
        m_min.x, m_max.x, m_min.z, m_max.z, m_min.y, material
    ));

    m_wallsInstances.push_back(std::make_unique<RectangleYZ>(
        m_min.y, m_max.y, m_min.z, m_max.z, m_max.x, material
    ));
    m_wallsInstances.push_back(std::make_unique<RectangleYZ>(
        m_min.y, m_max.y, m_min.z, m_max.z, m_min.x, material
    ));

    for (auto& wall : m_wallsInstances) m_walls.add(wall.get());
}

std::optional<IntersectRecord> Box::intersect(
    const kc::math::Ray& ray, float min, float max
) {
    return m_walls.intersect(ray, min, max);
}

bvh::AABB Box::getBoundingBox() const { return bvh::AABB{m_min, m_max}; }

}  // namespace geom

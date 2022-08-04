#include "Solid.h"

namespace texture {

Solid::Solid(const glm::vec3& color) : m_color(color) {}

glm::vec3 Solid::getColor(
    [[maybe_unused]] float u, [[maybe_unused]] float v,
    [[maybe_unused]] const glm::vec3& p
) const {
    return m_color;
}

}  // namespace texture
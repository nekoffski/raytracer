#include "Checker.h"

namespace texture {

Checker::Checker(Texture* even, Texture* odd) : m_even(even), m_odd(odd) {}

glm::vec3 Checker::getColor(float u, float v, const glm::vec3& p) const {
    auto s = std::sin(10.0f * p.x) * std::sin(10.0f * p.y) * std::sin(10.0f * p.z);

    return s < 0 ? m_odd->getColor(u, v, p) : m_even->getColor(u, v, p);
}

}  // namespace texture
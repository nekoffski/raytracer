#include "Diffuse.h"

namespace light {

Diffuse::Diffuse(const glm::vec3& color)
    : m_solidTexture(texture::Solid{color}), m_texture(std::addressof(*m_solidTexture)) {}

Diffuse::Diffuse(texture::Texture* texture) : m_texture(texture) {}

std::optional<mat::ScatterRecord> Diffuse::scatter(
    [[maybe_unused]] const kc::math::Ray& ray,
    [[maybe_unused]] const geom::IntersectRecord& hitRecord
) {
    return {};
}

glm::vec3 Diffuse::emit(float u, float v, const glm::vec3& p) const {
    return m_texture->getColor(u, v, p);
}

}  // namespace light

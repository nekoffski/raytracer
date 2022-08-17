#pragma once

#include "texture/Texture.h"
#include "mat/Material.h"

namespace light {

class Diffuse : public mat::Material {
   public:
    explicit Diffuse(texture::Texture* texture) : m_texture(texture) {}

    std::optional<mat::ScatterRecord> scatter(
        [[maybe_unused]] const kc::math::Ray& ray,
        [[maybe_unused]] const geom::IntersectRecord& hitRecord
    ) override {
        return {};
    }

    glm::vec3 emit(float u, float v, const glm::vec3& p) const override {
        return m_texture->getColor(u, v, p);
    }

   private:
    texture::Texture* m_texture;
};

}  // namespace light
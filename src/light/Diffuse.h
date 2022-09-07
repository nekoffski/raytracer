#pragma once

#include <optional>

#include "texture/Texture.h"
#include "texture/Solid.h"
#include "mat/Material.h"

namespace light {

class Diffuse : public mat::Material {
   public:
    explicit Diffuse(const glm::vec3& color);
    explicit Diffuse(texture::Texture* texture);

    std::optional<mat::ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    ) override;

    glm::vec3 emit(float u, float v, const glm::vec3& p) const override;

   private:
    std::optional<texture::Solid> m_solidTexture;
    texture::Texture* m_texture;
};

}  // namespace light
#pragma once

#include <optional>

#include "geom/IntersectRecord.h"
#include "Material.h"
#include "texture/Texture.h"
#include "texture/Solid.h"

namespace mat {

// diffuse
class Lambertian : public Material {
   public:
    explicit Lambertian(const glm::vec3& color);
    explicit Lambertian(texture::Texture* albedo);

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    ) override;

    float scatteringPdf(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord,
        const kc::math::Ray& scatteredRay
    ) const override;

    texture::Texture* getAlbedo() const { return m_albedo; }

   private:
    std::optional<texture::Solid> m_solidAlbedo;
    texture::Texture* m_albedo;
};

}  // namespace mat

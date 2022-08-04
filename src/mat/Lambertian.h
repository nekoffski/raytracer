#pragma once

#include "geom/IntersectRecord.h"
#include "Material.h"
#include "texture/Texture.h"

namespace mat {

// diffuse
class Lambertian : public Material {
   public:
    explicit Lambertian(texture::Texture* albedo);

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    ) override;

   private:
    texture::Texture* m_albedo;
};

}  // namespace mat

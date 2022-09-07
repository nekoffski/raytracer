#pragma once

#include "Material.h"
#include "texture/Texture.h"

namespace mat {

class Isotropic : public Material {
   public:
    explicit Isotropic(texture::Texture* albedo);

    std::optional<ScatterRecord> scatter(
        const kc::math::Ray& ray, const geom::IntersectRecord& hitRecord
    );

   private:
    texture::Texture* m_albedo;
};

}  // namespace mat
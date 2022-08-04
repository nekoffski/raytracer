#pragma once

#include "Texture.h"

namespace texture {

class Solid : public Texture {
   public:
    explicit Solid(const glm::vec3& color);

    glm::vec3 getColor(float u, float v, const glm::vec3& p) const override;

   private:
    glm::vec3 m_color;
};

}  // namespace texture
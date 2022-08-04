#pragma once

#include "Texture.h"

namespace texture {

class Checker : public Texture {
   public:
    explicit Checker(Texture* even, Texture* odd);

    glm::vec3 getColor(float u, float v, const glm::vec3& p) const override;

   private:
    Texture* m_even;
    Texture* m_odd;
};

}  // namespace texture
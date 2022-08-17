#pragma once

#include <kc/core/Log.h>

#include "Texture.h"
#include "Perlin.h"

namespace texture {

class Noisy : public Texture {
   public:
    explicit Noisy(float scale = 1.0f) : m_scale(scale) {}

    glm::vec3 getColor(float u, float v, const glm::vec3& p) const override {
        auto noise = m_noiseGenerator.getNoise(m_scale * p);
        auto color = glm::vec3{1.0f} * 0.5f * (1.0f + noise);

        if (color.x < 0) return glm::vec3{0.0f};

        return color;
    }

   private:
    float m_scale;
    Perlin m_noiseGenerator;
};

}  // namespace texture
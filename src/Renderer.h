#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

#include "geom/Intersectable.h"

class Renderer {
   public:
    using Framebuffer = std::vector<glm::vec3>;

    struct Config {
        const int width;
        const int height;
    };

    explicit Renderer(const Config &config);

    const Framebuffer &getFramebuffer() const;

    void render(const std::vector<geom::Intersectable *> scene);

   private:
    geom::Intersectable *trace(
        const kc::math::Ray &ray, const std::vector<geom::Intersectable *> &objects,
        float &t
    );

    glm::vec3 castRay(
        const kc::math::Ray &ray, const std::vector<geom::Intersectable *> &objects,
        int maxRecursion = 15
    );

    Config m_config;

    float m_aspect;
    Framebuffer m_framebuffer;
};
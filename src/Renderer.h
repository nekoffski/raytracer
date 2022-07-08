#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>

#include "geom/Intersectable.h"
#include "Config.h"
#include "Camera.h"

class Renderer {
   public:
    using Framebuffer = std::vector<glm::vec3>;

    explicit Renderer(
        const Config& config, const Camera& camera, geom::Intersectable* world
    );

    const Framebuffer& getFramebuffer() const;

    void render(int depth = 5);
    glm::vec3 traceRay(const kc::math::Ray& ray, int depth);

   private:
    const Config& m_config;
    const Camera& m_camera;
    Framebuffer m_framebuffer;
    geom::Intersectable* m_world;
};
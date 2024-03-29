#pragma once

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <kc/math/Ray.h>
#include <kc/core/Clock.h>

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

    void render(int depth, int samplesPerPixel);
    glm::vec3 traceRay(const kc::math::Ray& ray, int depth);

   private:
    std::pair<float, float> getUV(int i, int j) const;
    int getProgress(int index);
    std::string formatProgress(int index);

    const Config& m_config;
    const Camera& m_camera;
    Framebuffer m_framebuffer;
    geom::Intersectable* m_world;

    kc::core::Clock m_clock;
    kc::core::Clock::TimePoint m_startTime;
};
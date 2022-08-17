#include "Renderer.h"

#include <iostream>
#include <kc/math/Utils.hpp>

glm::vec3 background(const kc::math::Ray& ray) {
    auto direction = glm::normalize(ray.getDirection());

    float t = 0.5f * (direction.y + 1.0f);
    return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
}

Renderer::Renderer(const Config& config, const Camera& camera, geom::Intersectable* world)
    : m_config(config)
    , m_camera(camera)
    , m_framebuffer(config.width * config.height, glm::vec3{0.0f})
    , m_world(world) {}

const Renderer::Framebuffer& Renderer::getFramebuffer() const { return m_framebuffer; }

glm::vec3 applyGammaCorrection(glm::vec3 color) {
    static const float factor     = 1.0f / 2.2f;
    static constexpr int channels = 3;

    for (int i = 0; i < channels; ++i) color[i] = std::pow(color[i], factor);

    return color;
}

int Renderer::getProgress(int index) {
    return static_cast<int>(
        100.0f * static_cast<float>(m_config.height - index) /
        static_cast<float>(m_config.height)
    );
}

std::pair<float, float> Renderer::getUV(int i, int j) const {
    auto u = static_cast<float>(i + kc::math::random<float>(-0.5f, 0.5f)) /
             (m_config.width - 1);
    auto v = static_cast<float>(j + kc::math::random<float>(-0.5f, 0.5f)) /
             (m_config.height - 1);

    return {u, v};
}

void Renderer::render(int depth) {
    static constexpr int samplesPerPixel = 25;

    for (int j = m_config.height - 1; j >= 0; --j) {
        std::cout << getProgress(j) << "% done\n";

        for (int i = 0u; i < m_config.width; ++i) {
            glm::vec3 color{0.0f};

            for (int k = 0; k < samplesPerPixel; ++k) {
                auto [u, v] = getUV(i, j);

                color += traceRay(m_camera.getRay(u, v), depth);
            }

            color /= static_cast<float>(samplesPerPixel);
            m_framebuffer[(m_config.height - j - 1) * m_config.width + i] =
                applyGammaCorrection(color);
        }
    }
}

glm::vec3 Renderer::traceRay(const kc::math::Ray& ray, int depth) {
    static const glm::vec3 black{0.0f};
    static constexpr float nearest = 0.001f;

    if (depth <= 0) return black;

    static constexpr float max = std::numeric_limits<float>::max();

    if (auto result = m_world->intersect(ray, nearest, max); result) {
        const auto emitted =
            result->material->emit(result->u, result->v, result->hitPoint);

        if (auto s = result->material->scatter(ray, *result); s)
            return emitted + s->attenuation * traceRay(s->ray, depth - 1);

        return emitted;
    }

    return glm::vec3{0.1f};
}

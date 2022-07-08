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

void Renderer::render(int depth) {
    const float aspect = (float)m_config.width / m_config.height;

    auto viewport_height = 2.0;
    auto viewport_width  = aspect * viewport_height;
    auto focal_length    = 1.0;

    auto origin     = glm::vec3(0, 0, 0);
    auto horizontal = glm::vec3(viewport_width, 0, 0);
    auto vertical   = glm::vec3(0, viewport_height, 0);
    auto lower_left_corner =
        origin - horizontal / 2.0f - vertical / 2.0f - glm::vec3(0, 0, focal_length);

    static constexpr int samplesPerPixel = 8;

    for (int j = m_config.height - 1; j >= 0; --j) {
        std::cout << (int
                     )((float)((m_config.height - j) / (float)m_config.height) * 100.0f)
                  << "% done\n";

        for (int i = 0u; i < m_config.width; ++i) {
            glm::vec3 color{0.0f};

            for (int k = 0; k < samplesPerPixel; ++k) {
                auto u = static_cast<float>(i + kc::math::random<float>(-0.5f, 0.5f)) /
                         (m_config.width - 1);
                auto v = static_cast<float>(j + kc::math::random<float>(-0.5f, 0.5f)) /
                         (m_config.height - 1);

                auto targetPoint = lower_left_corner + u * horizontal + v * vertical;
                auto ray         = m_camera.getRay(u, v);

                color += traceRay(ray, depth);
            }

            color /= static_cast<float>(samplesPerPixel);

            static auto applyGammaCorrection = [](float value) {
                const float factor = 1.0f / 2.2f;
                return std::pow(value, factor);
            };

            color.x = applyGammaCorrection(color.x);
            color.y = applyGammaCorrection(color.y);
            color.z = applyGammaCorrection(color.z);

            m_framebuffer[(m_config.height - j - 1) * m_config.width + i] = color;
        }
    }
}

glm::vec3 Renderer::traceRay(const kc::math::Ray& ray, int depth) {
    if (depth <= 0) return glm::vec3{0.0f};

    static constexpr float max = std::numeric_limits<float>::max();

    if (auto result = m_world->intersect(ray, 0.001f, max); result) {
        if (auto s = result->material->scatter(ray, *result); s)
            return s->attenuation * traceRay(s->ray, depth - 1);

        return glm::vec3{0.0f};
    }

    return background(ray);
}

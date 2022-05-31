#include "Renderer.h"

Renderer::Renderer(const Config &config)
    : m_config(config)
    , m_aspect(static_cast<float>(config.width) / config.height)
    , m_framebuffer(config.width * config.height) {}

const Renderer::Framebuffer &Renderer::getFramebuffer() const { return m_framebuffer; }

void Renderer::render(const std::vector<geom::Intersectable *> scene) {
    auto &cfg = m_config;

    for (int j = 0; j < cfg.height; ++j) {
        for (int i = 0; i < cfg.width; ++i) {
            float x = (2 * (i + 0.5) / (float)cfg.width - 1) * m_aspect;
            float y = (1 - 2 * (j + 0.5) / (float)cfg.height);

            auto rayDirection = glm::normalize(glm::vec3{x, y, -1.0f});
            kc::math::Ray ray{glm::vec3{0.0f}, rayDirection};

            m_framebuffer[j * cfg.width + i] = castRay(ray, scene);
        }
    }
}

Renderer::TraceRecord Renderer::trace(
    const kc::math::Ray &ray, const std::vector<geom::Intersectable *> &objects
) {
    float nearestHit               = std::numeric_limits<float>::max();
    geom::Intersectable *hitObject = nullptr;

    for (auto &object : objects) {
        for (auto &object : objects) {
            if (auto hitRecord = object->intersect(ray);
                hitRecord && nearestHit > hitRecord->t) {
                nearestHit = hitRecord->t;
                hitObject  = object;
            }
        }
    }

    return TraceRecord{hitObject, nearestHit};
}

glm::vec3 Renderer::castRay(
    const kc::math::Ray &ray, const std::vector<geom::Intersectable *> &objects
) {
    const static glm::vec3 background{0.7f, 0.75f, 1.0f};

    const auto [hitObject, hitDistance] = trace(ray, objects);

    if (not hitObject) return background * 0.3f;

    auto hitPoint = ray.at(hitDistance);
    auto n        = hitObject->getNormal(hitPoint);
    auto d        = ray.getDirection();

    const float facingRatio = std::max(0.0f, glm::dot(-d, n));

    // light
    auto l              = glm::normalize(glm::vec3{-1.0, 0.3f, 1.0f});
    auto lightIntensity = 0.5f;

    return hitObject->getColor() * lightIntensity * std::max(0.0f, glm::dot(n, l));
}

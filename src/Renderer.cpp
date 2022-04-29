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

geom::Intersectable *Renderer::trace(
    const kc::math::Ray &ray, const std::vector<geom::Intersectable *> &objects, float &t
) {
    float nearestHit = 100000000000.0f;

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
    t = nearestHit;
    return hitObject;
}

glm::vec3 Renderer::castRay(
    const kc::math::Ray &ray, const std::vector<geom::Intersectable *> &objects,
    int maxRecursion
) {
    glm::vec3 background{0.7f, 0.75f, 1.0f};

    if (maxRecursion < 0) return glm::vec3{0.0f};

    float hitT;
    auto hitObject = trace(ray, objects, hitT);

    if (hitObject) {
        auto hitPoint = ray.at(hitT);
        auto n        = hitObject->getNormal(hitPoint);
        auto d        = ray.getDirection();

        auto r = glm::normalize(d - 2 * (glm::dot(d, n)) * n);
        kc::math::Ray newRay{hitPoint, r};

        const float facingRatio = std::max(0.0f, glm::dot(-d, n));

        const float v = 0.3;
        return facingRatio * (1 - v) * hitObject->getColor() +
               v * castRay(newRay, objects, maxRecursion - 1);
    }

    return background;
}

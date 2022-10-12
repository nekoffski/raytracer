#include "Renderer.h"

#include <iostream>

#include <kc/core/Log.h>
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

struct TimePoint {
    int64_t hour;
    int64_t minute;
    int64_t seconds;
};

template <typename T> static TimePoint parseTimePoint(const T& timePoint) {
    auto msElapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(timePoint).count();

    static constexpr int64_t msInSecond = 1000;
    static constexpr int64_t msInMinute = msInSecond * 60;
    static constexpr int64_t msInHour   = msInMinute * 60;

    const auto hours = msElapsed / msInHour;
    msElapsed %= msInHour;

    const auto minutes = msElapsed / msInMinute;
    msElapsed %= msInMinute;

    const auto seconds = msElapsed / msInSecond;

    return TimePoint{hours, minutes, seconds};
}

std::string Renderer::formatProgress(int index) {
    const auto [hours, minutes, seconds] = parseTimePoint(m_clock.now() - m_startTime);

    return fmt::format(
        "Elapsed time {:0>2}:{:0>2}:{:0>2} - done {:0>2}%", hours, minutes, seconds,
        getProgress(index)
    );
}

std::pair<float, float> Renderer::getUV(int i, int j) const {
    auto u = static_cast<float>(i + kc::math::random<float>(-0.5f, 0.5f)) /
             (m_config.width - 1);
    auto v = static_cast<float>(j + kc::math::random<float>(-0.5f, 0.5f)) /
             (m_config.height - 1);

    return {u, v};
}

void Renderer::render(int depth, int samplesPerPixel) {
    m_startTime = m_clock.now();

    for (int j = m_config.height - 1; j >= 0; --j) {
        std::cout << "\t\r" << formatProgress(j) << std::flush;

        for (int i = 0u; i < m_config.width; ++i) {
            glm::vec3 color{0.0f};

            for (int k = 0; k < samplesPerPixel; ++k) {
                const auto [u, v] = getUV(i, j);
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

    return black;
}

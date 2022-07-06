#include <kc/core/FileSystem.h>
#include <kc/math/Ray.h>

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "geom/Sphere.h"
#include "geom/Hittable.h"

#include "mat/Metal.h"
#include "mat/Dielectric.h"
#include "mat/Lambertian.h"

#include <iostream>

struct Config {
    uint64_t width;
    uint64_t height;
};

glm::vec3 background(const kc::math::Ray& ray) {
    auto direction = glm::normalize(ray.getDirection());

    float t = 0.5f * (direction.y + 1.0f);
    return (1.0f - t) * glm::vec3(1.0, 1.0, 1.0) + t * glm::vec3(0.5, 0.7, 1.0);
}

class camera {
   public:
    camera() {
        auto aspect_ratio    = 16.0f / 9.0f;
        auto viewport_height = 2.0f;
        auto viewport_width  = aspect_ratio * viewport_height;
        auto focal_length    = 1.0f;

        origin     = glm::vec3(0, 0, 0);
        horizontal = glm::vec3(viewport_width, 0.0, 0.0);
        vertical   = glm::vec3(0.0, viewport_height, 0.0);
        lower_left_corner =
            origin - horizontal / 2.0f - vertical / 2.0f - glm::vec3(0, 0, focal_length);
    }

    kc::math::Ray get_ray(float u, float v) const {
        return kc::math::Ray(
            origin, lower_left_corner + u * horizontal + v * vertical - origin
        );
    }

   private:
    glm::vec3 origin;
    glm::vec3 lower_left_corner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
} cam;

class Renderer {
   public:
    using Framebuffer = std::vector<glm::vec3>;

    explicit Renderer(const Config& config, geom::Hittable* world)
        : m_config(config)
        , m_framebuffer(config.width * config.height, glm::vec3{0.0f})
        , m_world(world) {}

    const Framebuffer& getFramebuffer() const { return m_framebuffer; }

    void render(int depth = 5) {
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
                         )((float)((m_config.height - j) / (float)m_config.height) *
                           100.0f)
                      << "% done\n";

            for (int i = 0u; i < m_config.width; ++i) {
                glm::vec3 color{0.0f};

                for (int k = 0; k < samplesPerPixel; ++k) {
                    auto u =
                        static_cast<float>(i + kc::math::random<float>(-0.5f, 0.5f)) /
                        (m_config.width - 1);
                    auto v =
                        static_cast<float>(j + kc::math::random<float>(-0.5f, 0.5f)) /
                        (m_config.height - 1);

                    auto targetPoint = lower_left_corner + u * horizontal + v * vertical;
                    auto ray         = cam.get_ray(u, v);

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

    glm::vec3 traceRay(const kc::math::Ray& ray, int depth) {
        if (depth <= 0) return glm::vec3{0.0f};

        static constexpr float max = std::numeric_limits<float>::max();

        if (auto result = m_world->hit(ray, 0.001f, max); result) {
            if (auto s = result->material->scatter(ray, *result); s)
                return s->attenuation * traceRay(s->ray, depth - 1);

            return glm::vec3{0.0f};
        }

        return background(ray);
    }

   private:
    const Config& m_config;
    Framebuffer m_framebuffer;
    geom::Hittable* m_world;
};

void saveImage(const Renderer::Framebuffer& framebuffer, const Config& config) {
    std::ostringstream str;
    str << "P3 " << config.width << " " << config.height << " 255\n";
    for (const auto& pixel : framebuffer)
        str << (int)(255.0f * pixel.x) << " " << (int)(255.0f * pixel.y) << " "
            << (int)(255.0f * pixel.z) << '\n';

    kc::core::FileSystem{}.writeFile(
        "image.ppm", str.str(), kc::core::FileSystem::WritePolicy::override
    );
}

int main() {
    Config config{.width = 1600, .height = 900};

    geom::HittableCollection world;

    using namespace mat;

    Lambertian l1(glm::vec3(0.8, 0.8, 0.0));
    Lambertian l2(glm::vec3(0.7, 0.3, 0.3));

    Metal m1(glm::vec3(0.8, 0.8, 0.8), 0.3f);
    Metal m2(glm::vec3(0.8, 0.6, 0.2), 0.9f);

    Dielectric d1{1.5f};

    geom::Sphere s1{
        glm::vec3{0.0, -100.5, -1.0},
        100.0f, &l1
    };
    geom::Sphere s2{
        glm::vec3{0.0, 0.0, -1.0},
        0.5f, &l2
    };
    geom::Sphere s3{
        glm::vec3{-1.0, 0.0, -1.0},
        0.5f, &d1
    };
    geom::Sphere s4{
        glm::vec3{1.0, 0.0, -1.0},
        0.5f, &m2
    };

    world.addObjects(&s1, &s2, &s3, &s4);

    Renderer renderer{config, &world};
    renderer.render();

    saveImage(renderer.getFramebuffer(), config);
    return 0;
}

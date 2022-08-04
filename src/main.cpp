#include <kc/core/FileSystem.h>
#include <kc/math/Ray.h>

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "geom/Sphere.h"
#include "geom/Intersectable.h"
#include "geom/IntersectableCollection.h"

#include "mat/Metal.h"
#include "mat/Dielectric.h"
#include "mat/Lambertian.h"

#include "texture/Solid.h"
#include "texture/Checker.h"

#include "Config.h"
#include "Renderer.h"

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

    geom::IntersectableCollection world;

    using namespace mat;
    using namespace texture;

    Solid t1{glm::vec3(0.8, 0.8, 0.0)};
    Solid t2{glm::vec3(0.7, 0.3, 0.3)};

    Solid t3{glm::vec3{1.0f}};
    Solid t4{glm::vec3{0.0f}};

    Checker c1{&t3, &t4};

    Lambertian l1(&c1);
    Lambertian l2(&t2);

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
        0.5f, &m1
    };
    geom::Sphere s4{
        glm::vec3{1.0, 0.0, -1.0},
        0.5f, &m2
    };

    world.addObjects(&s1, &s2, &s3, &s4);

    Camera camera{
        glm::vec3{-1, 1, 0.5},
        glm::vec3{0,  0, -1 },
        glm::vec3{0,  1, 0  },
        90.0f,
        1600.0f / 900.0f,
        2.0f,
        5.0f
    };

    auto boundingVolume = world.getBoundingVolume();

    Renderer renderer{config, camera, &boundingVolume};
    renderer.render();

    saveImage(renderer.getFramebuffer(), config);
    return 0;
}

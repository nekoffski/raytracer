#include <kc/core/FileSystem.h>
#include <kc/math/Ray.h>

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "geom/Intersectable.h"
#include "geom/Sphere.h"
#include "Renderer.h"

int main() {
    Renderer::Config cfg{.width = 800, .height = 600};

    Renderer renderer{cfg};

    std::vector<geom::Intersectable *> objects;

    geom::Sphere::Params s1Description{
        .center = glm::vec3{1.5f, 0.0f, -5.0f},
        .color  = glm::vec3{0.0f, 0.0f, 1.0f },
        .r      = 1.0f
    };
    geom::Sphere s1{s1Description};

    geom::Sphere::Params s2Description{
        .center = glm::vec3{-1.5f, 0.0f, -5.0f},
        .color  = glm::vec3{1.0f,  0.0f, 0.0f },
        .r      = 1.0f
    };
    geom::Sphere s2{s2Description};

    geom::Sphere::Params s3Description{
        .center = glm::vec3{0.0f, -5.0f, -7.0f},
        .color  = glm::vec3{1.0f, 1.0f,  1.0f },
        .r      = 5.0f
    };
    geom::Sphere s3{s3Description};

    objects.push_back(&s1);
    objects.push_back(&s2);
    objects.push_back(&s3);

    renderer.render(objects);

    std::ostringstream str;
    str << "P3 " << cfg.width << " " << cfg.height << " 255\n";
    for (const auto &pixel : renderer.getFramebuffer())
        str << (int)(255.0f * pixel.x) << " " << (int)(255.0f * pixel.y) << " "
            << (int)(255.0f * pixel.z) << '\n';

    kc::core::FileSystem{}.writeFile(
        "image.ppm", str.str(), kc::core::FileSystem::WritePolicy::override
    );

    // m1.vertices = {
    //     glm::vec3{-1.0f, -1.0f, -9.0f},
    //     glm::vec3{1.0f,  -1.0f, -9.0f},
    //     glm::vec3{1.0f,  1.0f,  -9.0f},
    //     glm::vec3{-1.0f, 1.0f,  -9.0f}
    // };
    // m1.indices = {0, 1, 3, 1, 2, 3};
    // m1.color   = glm::vec3{1.0f, 1.0f, 0.0f};

    return 0;
}
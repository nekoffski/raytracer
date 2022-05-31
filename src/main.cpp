#include <kc/core/FileSystem.h>
#include <kc/math/Ray.h>

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "geom/Intersectable.h"
#include "geom/Sphere.h"
#include "geom/Triangle.h"
#include "geom/TriangleMesh.h"

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

    geom::Triangle t1;

    t1.vertices = {
        glm::vec3{2.5f, -1.0f, -3.0f},
        glm::vec3{3.5f, -1.0f, -3.0f},
        glm::vec3{3.5f, 1.0f,  -3.0f}
    };

    t1.color = glm::vec3{1.0f, 1.0f, 0.0f};

    geom::TriangleMesh m1;

    m1.vertices = {
        glm::vec3{-3.0f, -2.0f, -7.0f},
        glm::vec3{3.0f,  -2.0f, -7.0f},
        glm::vec3{3.0f,  4.0f,  -7.0f},
        glm::vec3{-3.0f, 4.0f,  -7.0f}
    };

    m1.indices = {0, 1, 3, 1, 2, 3};
    m1.color   = glm::vec3{0.0f, 1.0f, 0.0f};

    objects.push_back(&s1);
    objects.push_back(&s2);
    objects.push_back(&s3);
    objects.push_back(&t1);
    objects.push_back(&m1);

    renderer.render(objects);

    std::ostringstream str;
    str << "P3 " << cfg.width << " " << cfg.height << " 255\n";
    for (const auto &pixel : renderer.getFramebuffer())
        str << (int)(255.0f * pixel.x) << " " << (int)(255.0f * pixel.y) << " "
            << (int)(255.0f * pixel.z) << '\n';

    kc::core::FileSystem{}.writeFile(
        "image.ppm", str.str(), kc::core::FileSystem::WritePolicy::override
    );

    return 0;
}
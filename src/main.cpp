
#include <boost/program_options.hpp>

#include <kc/core/FileSystem.h>
#include <kc/core/Log.h>
#include <kc/math/Ray.h>

#include <glm/glm.hpp>
#include <kc/math/Utils.hpp>

#include "geom/Sphere.h"
#include "geom/Intersectable.h"
#include "geom/IntersectableCollection.h"
#include "geom/Rectangle.h"
#include "geom/Box.h"

#include "mat/Metal.h"
#include "mat/Dielectric.h"
#include "mat/Lambertian.h"

#include "texture/Solid.h"
#include "texture/Checker.h"
#include "texture/Noisy.h"
#include "texture/Image.h"

#include "transform/Rotate.h"
#include "transform/Translate.h"

#include "light/Diffuse.h"

#include "Config.h"
#include "Renderer.h"

#include "geom/ConstantMedium.h"

#include "image/MeanFilter.h"

#include "scene/SceneLoader.h"

kc::core::FileSystem fs{};

void saveImage(
    const Renderer::Framebuffer& framebuffer, const Config& config,
    const std::string& imagePath
) {
    static auto clamp = [](float value) -> float {
        return std::clamp(value, 0.0f, 1.0f);
    };

    std::ostringstream str;
    str << "P3 " << config.width << " " << config.height << " 255\n";
    for (const auto& pixel : framebuffer)
        str << (int)(255.0f * clamp(pixel.x)) << " " << (int)(255.0f * clamp(pixel.y))
            << " " << (int)(255.0f * clamp(pixel.z)) << '\n';

    fs.writeFile(imagePath, str.str(), kc::core::FileSystem::WritePolicy::override);
}

struct Args {
    int depth;
    int samplesPerPixel;
    std::string imagePath;
    std::string scenePath;
};

Args processArgs(int argc, char** argv) {
    namespace po = boost::program_options;

    static constexpr int defaultDepth           = 10;
    static constexpr int defaultSamplesPerPixel = 15;
    static const std::string defaultImagePath   = "image.ppm";

    po::options_description description("Allowed options");

    description.add_options()("help", "produce help message")(
        "depth", po::value<int>()->default_value(defaultDepth),
        "Maximum depth of the tracing algorithm"
    )("samples", po::value<int>()->default_value(defaultSamplesPerPixel),
      "Ray samples per pixel")(
        "output", po::value<std::string>()->required(), "Path to the output image"
    )("scene", po::value<std::string>()->required(), "Path to the scene description");

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(description).run(), variables);

    if (variables.contains("help")) {
        LOG_INFO("{}", description);
        exit(0);
    }

    po::notify(variables);

    return Args{
        variables.at("depth").as<int>(),
        variables.at("samples").as<int>(),
        variables.at("output").as<std::string>(),
        variables.at("scene").as<std::string>(),
    };
}

int main(int argc, char** argv) {
    kc::core::initLogging("raytracer", "%v");

    Config config{.width = 800, .height = 800};

    // geom::IntersectableCollection world;

    // using namespace mat;
    // using namespace texture;

    // Solid r{
    //     glm::vec3{.65f, .05f, .05f}
    // };

    // Solid w{glm::vec3{.73f}};

    // Solid g{
    //     glm::vec3{.12f, .45f, .15f}
    // };

    // Solid ll{glm::vec3{15.0f}};

    // light::Diffuse light{glm::vec3{15.0f}};

    // Lambertian red{
    //     glm::vec3{.65f, .05f, .05f}
    // };
    // Lambertian white{glm::vec3{.73f}};
    // Lambertian green{
    //     glm::vec3{.12f, .45f, .15f}
    // };

    // Solid b{glm::vec3{0.0f}};

    // // geom::RectangleYZ r1(0, 555, 0, 555, 555, &green);
    // // geom::RectangleYZ r2(0, 555, 0, 555, 0, &red);
    // // geom::RectangleXZ r3(213, 343, 227, 332, 554, &light);
    // // geom::RectangleXZ r4(0, 555, 0, 555, 0, &white);
    // // geom::RectangleXZ r5(0, 555, 0, 555, 555, &white);
    // // geom::RectangleXY r6(0, 555, 0, 555, 555, &white);

    // geom::Box box1{glm::vec3(0, 0, 0), glm::vec3(165, 330, 165), &white};

    // transform::RotateY bbb1{15, &box1};

    // transform::Translate bbbb1{
    //     glm::vec3{265, 0, 295},
    //     &bbb1
    // };

    // geom::Box box2{glm::vec3(0, 0, 0), glm::vec3(165, 165, 165), &white};

    // transform::RotateY bbb2{-18, &box2};
    // transform::Translate bbbb2{
    //     glm::vec3{130, 0, 64},
    //     &bbb2
    // };

    // // geom::ConstantMedium smoke1{&bbbb1, 0.01f, &b};
    // // geom::ConstantMedium smoke2{&bbbb2, 0.01f, &w};

    // world.addObjects(&bbbb1);

    // geom::IntersectableCollection boxes1;

    // // ground
    // texture::Solid groundTexture{
    //     glm::vec3{0.48, 0.83, 0.53}
    // };
    // mat::Lambertian ground{&groundTexture};

    // std::vector<geom::Box> boxes1Objects;

    // const int boxes_per_side = 20;
    // for (int i = 0; i < boxes_per_side; i++) {
    //     for (int j = 0; j < boxes_per_side; j++) {
    //         auto w  = 100.0;
    //         auto x0 = -1000.0 + i * w;
    //         auto z0 = -1000.0 + j * w;
    //         auto y0 = 0.0;
    //         auto x1 = x0 + w;
    //         auto y1 = kc::math::random<float>(1.0f, 101.0f);
    //         auto z1 = z0 + w;

    //         boxes1Objects.push_back(
    //             geom::Box(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1), &ground)
    //         );
    //     }
    // }

    // for (auto& box : boxes1Objects) boxes1.add(&box);
    // bvh::Node boxes1Bvh{boxes1.getObjects()};

    // // light
    // texture::Solid lightTexture{
    //     glm::vec3{7, 7, 7}
    // };
    // light::Diffuse lightMat{&lightTexture};
    // geom::RectangleXZ light{123, 423, 147, 412, 554, &lightMat};

    // world.add(&light);

    // // spheres
    // mat::Dielectric sphere1Mat{1.5f};
    // mat::Metal sphere2Mat{
    //     glm::vec3{0.8, 0.8, 0.9},
    //     1.0f
    // };

    // geom::Sphere s1{
    //     glm::vec3{260, 150, 45},
    //     50, &sphere1Mat
    // };
    // geom::Sphere s2{
    //     glm::vec3{0, 150, 145},
    //     50, &sphere2Mat
    // };

    // geom::Sphere boundary{
    //     glm::vec3{360, 150, 145},
    //     70, &sphere1Mat
    // };

    // texture::Solid albedo1{
    //     glm::vec3{0.4, 0.6, 0.9}
    // };
    // texture::Solid albedo2{glm::vec3{1.0f}};

    // geom::ConstantMedium c1{&boundary, 0.6, &albedo1};
    // geom::ConstantMedium c2{&boundary, 0.0001, &albedo2};

    // world.addObjects(&s1, &s2, &boundary, &c1, &c2);

    // // earth
    // texture::Image eartText{"earthmap.jpg"};
    // mat::Lambertian earthMat{&eartText};

    // geom::Sphere s3{
    //     glm::vec3{400, 200, 400},
    //     100, &earthMat
    // };

    // world.add(&s3);

    // geom::IntersectableCollection box2;
    // std::vector<geom::Sphere> box2Objects;

    // for (int j = 0; j < 250; j++) {
    //     box2Objects.emplace_back(kc::math::randomVec3(0, 165), 10, &white);
    // }

    // for (auto& s : box2Objects) box2.add(&s);

    // world.add(&box2);

    // // render
    // auto boundingVolume = world.getBoundingVolume();

    // Camera camera(
    //     glm::vec3(478, 278, -600), glm::vec3(278, 278, 0), glm::vec3(0.0f, 1.0f, 0.0f),
    //     40.0f, 1.0f, 1.0f, 1.0f
    // );

    const auto& [depth, samplesPerPixel, imagePath, scenePath] = processArgs(argc, argv);

    auto scene = scene::SceneLoader{}.fromFile(scenePath, fs).load();
    Renderer renderer{config, *scene.camera, &scene.world};

    LOG_INFO(
        "Render params depth={}, samples per pixel={}, output image={}", depth,
        samplesPerPixel, imagePath
    );

    renderer.render(depth, samplesPerPixel);

    // image::MeanFilter meanFilter{};
    // auto outputImage = meanFilter.apply(renderer.getFramebuffer());

    saveImage(renderer.getFramebuffer(), config, imagePath);
    return 0;
}

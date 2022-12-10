
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
    uint64_t width;
    uint64_t height;
    std::string imagePath;
    std::string scenePath;
};

Args processArgs(int argc, char** argv) {
    namespace po = boost::program_options;

    static constexpr int defaultDepth           = 10;
    static constexpr int defaultSamplesPerPixel = 15;
    static const std::string defaultImagePath   = "image.ppm";
    static constexpr uint64_t defaultWidth      = 800ul;
    static constexpr uint64_t defaultHeight     = 600ul;

    po::options_description description("Allowed options");

    description.add_options()("help", "produce help message")(
        "depth", po::value<int>()->default_value(defaultDepth),
        "Maximum depth of the tracing algorithm"
    )("samples", po::value<int>()->default_value(defaultSamplesPerPixel),
      "Ray samples per pixel")(
        "output", po::value<std::string>()->required(), "Path to the output image"
    )("scene", po::value<std::string>()->required(), "Path to the scene description")(
        "width", po::value<uint64_t>()->default_value(defaultWidth), "Output image width"
    )("height", po::value<uint64_t>()->default_value(defaultHeight),
      "Output image height");

    po::variables_map variables;
    po::store(po::command_line_parser(argc, argv).options(description).run(), variables);

    if (variables.contains("help")) {
        LOG_INFO("{}", description);
        exit(0);
    }

    po::notify(variables);

    return Args{
        .depth           = variables.at("depth").as<int>(),
        .samplesPerPixel = variables.at("samples").as<int>(),
        .width           = variables.at("width").as<uint64_t>(),
        .height          = variables.at("height").as<uint64_t>(),
        .imagePath       = variables.at("output").as<std::string>(),
        .scenePath       = variables.at("scene").as<std::string>(),
    };
}

int main(int argc, char** argv) {
    kc::core::initLogging("raytracer", "%v");

    const auto& [depth, samplesPerPixel, width, height, imagePath, scenePath] =
        processArgs(argc, argv);

    Config config{.width = width, .height = height};

    auto scene = scene::SceneLoader{}.fromFile(scenePath, fs).load();
    Renderer renderer{config, *scene.camera, &scene.world};

    LOG_INFO(
        "Render params depth={}, samples per pixel={}, output image={}", depth,
        samplesPerPixel, imagePath
    );

    renderer.render(depth, samplesPerPixel);

    saveImage(renderer.getFramebuffer(), config, imagePath);
    return 0;
}

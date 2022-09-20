#include "SceneLoader.h"

#include "texture/Solid.h"
#include "texture/Image.h"

#include "mat/Dielectric.h"
#include "mat/Lambertian.h"
#include "mat/Metal.h"

#include "light/Diffuse.h"

#include "geom/Rectangle.h"

namespace scene {

void SceneLoader::processCamera(const Node& root) {
    auto camera = std::make_unique<Camera>(
        fieldFrom(root).withName("look-from").ofType<glm::vec3>().get(),
        fieldFrom(root).withName("look-at").ofType<glm::vec3>().get(),
        fieldFrom(root).withName("up").ofType<glm::vec3>().get(),
        fieldFrom(root).withName("vertical-fov").ofType<float>().get(),
        fieldFrom(root).withName("aspect").ofType<float>().get(),
        fieldFrom(root).withName("aperture").ofType<float>().get(),
        fieldFrom(root).withName("focus-distance").ofType<float>().get()
    );

    config().camera = std::move(camera);
}

void SceneLoader::processFields(const Node& root) {
    try {
        LOG_INFO("Processing scene file");

        processCamera(fieldFrom(root).withName("camera").asObject().get());

        processEntities(root, "textures", config().textures, getTexturesFactories());
        processEntities(root, "materials", config().materials, getMaterialsFactories());
        processEntities(root, "objects", config().objects, getObjectsFactories());

        for (auto& [_, object] : config().objects) config().world.add(object.get());

    } catch (const kc::core::ErrorBase& e) {
        LOG_ERROR("Could not process scene - {}", e.what());
        throw SceneError{};
    }
}

StrKeyMap<SceneLoader::TextureFactory> SceneLoader::getTexturesFactories() const {
    auto createSolidTexture = [](SceneLoader* loader, const Node& root) {
        return std::make_unique<texture::Solid>(
            loader->fieldFrom(root).withName("color").ofType<glm::vec3>().get()
        );
    };

    auto createImageTexture = [](SceneLoader* loader, const Node& root) {
        const auto fileName =
            loader->fieldFrom(root).withName("filename").ofType<std::string>().get();
        return std::make_unique<texture::Image>(fileName);
    };

    return StrKeyMap<TextureFactory>{
        {"Solid", createSolidTexture},
        {"Image", createImageTexture},
    };
}

StrKeyMap<SceneLoader::ObjectFactory> SceneLoader::getObjectsFactories() const {
    static auto createRectangle = [](SceneLoader* loader, const Node& node) {
        auto orientation =
            loader->fieldFrom(node).withName("orientation").ofType<std::string>().get();

        auto edges = loader->fieldFrom(node).withName("edges").ofType<glm::vec4>().get();
        auto distance =
            loader->fieldFrom(node).withName("distance").ofType<float>().get();

        auto& materials = loader->config().materials;

        auto material =
            loader->fieldFrom(node).withName("material").ofType<std::string>().get();

        ASSERT(
            materials.contains(material), "Could not find material with name: {}",
            material
        );

        return geom::Rectangle::create(
            geom::Rectangle::orientationFromString(orientation), edges, distance,
            materials.at(material).get()
        );
    };

    return StrKeyMap<ObjectFactory>{
        {"Rectangle", createRectangle},
    };
}

StrKeyMap<SceneLoader::MaterialFactory> SceneLoader::getMaterialsFactories() const {
    auto createMetal = [](SceneLoader* loader, const Node& node) {
        return std::make_unique<mat::Metal>(
            loader->fieldFrom(node).withName("albedo").ofType<glm::vec3>().get(),
            loader->fieldFrom(node).withName("fuzziness").ofType<float>().get()
        );
    };

    auto createDiffuseLight = [](SceneLoader* loader, const Node& node) {
        return std::make_unique<light::Diffuse>(
            loader->fieldFrom(node).withName("color").ofType<glm::vec3>().get()
        );
    };

    auto createLambertian = [](SceneLoader* loader, const Node& node) {
        ASSERT(
            node.isMember("color") || node.isMember("texture"),
            "Lambertian material must have color or texture provided"
        );

        if (node.isMember("color")) {
            return std::make_unique<mat::Lambertian>(
                loader->fieldFrom(node).withName("color").ofType<glm::vec3>().get()
            );
        }

        const auto textureName =
            loader->fieldFrom(node).withName("texture").ofType<std::string>().get();
        auto& textures = loader->config().textures;

        ASSERT(
            textures.contains(textureName), "Could not find texture with name {}",
            textureName
        );

        return std::make_unique<mat::Lambertian>(textures.at(textureName).get());
    };

    auto createDielectric = [](SceneLoader* loader, const Node& node) {
        const auto refractionIndex =
            loader->fieldFrom(node).withName("index-of-refraction").ofType<float>().get();

        return std::make_unique<mat::Dielectric>(refractionIndex);
    };

    return StrKeyMap<SceneLoader::MaterialFactory>{
        {"Lambertian",   createLambertian  },
        {"Metal",        createMetal       },
        {"Dielectric",   createDielectric  },
        {"DiffuseLight", createDiffuseLight},
    };
}

std::pair<std::string, std::string> SceneLoader::getNameAndType(const Node& node) {
    return {
        fieldFrom(node).withName("name").ofType<std::string>().get(),
        fieldFrom(node).withName("type").ofType<std::string>().get()};
}

}  // namespace scene

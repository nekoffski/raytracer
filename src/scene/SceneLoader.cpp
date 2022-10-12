#include "SceneLoader.h"

#include "texture/Solid.h"
#include "texture/Image.h"

#include "mat/Dielectric.h"
#include "mat/Lambertian.h"
#include "mat/Metal.h"

#include "light/Diffuse.h"

#include "geom/Sphere.h"
#include "geom/Rectangle.h"
#include "geom/Box.h"

#include "transform/Translate.h"

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

        processObjects(root);

    } catch (const kc::core::ErrorBase& e) {
        LOG_ERROR("Could not process scene - {}", e.what());
        throw SceneError{};
    }
}

void SceneLoader::processObjects(const Node& root) {
    auto& container      = config().objects;
    const auto factories = getObjectsFactories();

    const auto transformFactories = getTransformFactories();

    static const std::string jsonField = "objects";
    for (auto& entity : fieldFrom(root).withName(jsonField).asArray().get()) {
        const auto& [name, type] = getNameAndType(entity);
        validate(jsonField, type, name, container, factories);

        geom::Intersectable* objectToRender =
            container.insert({name, factories.at(type)(this, entity)})
                .first->second.get();  // TODO: ugly as hell, wrap into some helper

        if (entity.isMember("transforms")) {
            for (auto& transformDescription :
                 fieldFrom(entity).withName("transforms").asArray().get()) {
                const auto& [name, type] = getNameAndType(transformDescription);

                ASSERT(
                    transformFactories.contains(type), "Could not find {} transform", type
                );

                LOG_INFO("Processing transform: {}/{}", type, name);

                // TODO: ugly as hell, wrap into some helper
                objectToRender =
                    container
                        .insert(
                            {name, transformFactories.at(type
                                   )(this, transformDescription, objectToRender)}
                        )
                        .first->second.get();
            }
        }

        config().world.add(objectToRender);
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

StrKeyMap<SceneLoader::TransformFactory> SceneLoader::getTransformFactories() const {
    static auto createTranslate =
        [](SceneLoader* loader, const Node& node, geom::Intersectable* object) {
        return std::make_unique<transform::Translate>(
            loader->fieldFrom(node).withName("offset").ofType<glm::vec3>().get(), object
        );
    };

    return StrKeyMap<TransformFactory>{
        {"Translate", createTranslate}
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

    static auto createSphere = [](SceneLoader* loader, const Node& node) {
        auto position =
            loader->fieldFrom(node).withName("position").ofType<glm::vec3>().get();
        auto radius = loader->fieldFrom(node).withName("radius").ofType<float>().get();

        auto& materials = loader->config().materials;

        auto material =
            loader->fieldFrom(node).withName("material").ofType<std::string>().get();

        ASSERT(
            materials.contains(material), "Could not find material with name: {}",
            material
        );

        return std::make_unique<geom::Sphere>(
            position, radius, materials.at(material).get()
        );
    };

    static auto createBox = [](SceneLoader* loader, const Node& node) {
        auto min = loader->fieldFrom(node).withName("min").ofType<glm::vec3>().get();
        auto max = loader->fieldFrom(node).withName("max").ofType<glm::vec3>().get();

        auto& materials = loader->config().materials;

        auto material =
            loader->fieldFrom(node).withName("material").ofType<std::string>().get();

        ASSERT(
            materials.contains(material), "Could not find material with name: {}",
            material
        );

        return std::make_unique<geom::Box>(min, max, materials.at(material).get());
    };

    return StrKeyMap<ObjectFactory>{
        {"Rectangle", createRectangle},
        {"Sphere",    createSphere   },
        {"Box",       createBox      }
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

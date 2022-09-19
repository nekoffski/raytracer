#pragma once

#include <unordered_map>
#include <memory>

#include <kc/core/Log.h>
#include <kc/core/ErrorBase.hpp>
#include <kc/json/JsonConfigLoader.hpp>
#include <kc/json/Utils.hpp>

#include "texture/Solid.h"
#include "texture/Image.h"

#include "mat/Dielectric.h"
#include "mat/Lambertian.h"
#include "mat/Metal.h"

#include "light/Diffuse.h"

#include "geom/Rectangle.h"

DEFINE_ERROR(SceneError);

namespace scene {

using namespace kc::json;

template <typename T> using StrKeyMap = std::unordered_map<std::string, T>;

struct Scene {
    StrKeyMap<std::unique_ptr<texture::Texture>> textures;
    StrKeyMap<std::unique_ptr<mat::Material>> materials;
    StrKeyMap<std::unique_ptr<geom::Intersectable>> objects;

    geom::IntersectableCollection world;
};

class SceneLoader : public JsonConfigLoader<Scene>, NodeHelper<SceneError> {
   private:
    using Helper = NodeHelper<SceneError>;

    using TextureFactory = std::function<
        std::unique_ptr<texture::Texture>(SceneLoader* loader, const Node&)>;

    using ObjectFactory = std::function<
        std::unique_ptr<geom::Intersectable>(SceneLoader* loader, const Node&)>;

    using MaterialFactory =
        std::function<std::unique_ptr<mat::Material>(SceneLoader* loader, const Node&)>;

    void processFields(const Node& root) override {
        try {
            LOG_INFO("Processing scene file");

            processEntities(root, "textures", config().textures, getTexturesFactories());

            processEntities(
                root, "materials", config().materials, getMaterialsFactories()
            );

            processEntities(root, "objects", config().objects, getObjectsFactories());

            for (auto& [_, object] : config().objects) config().world.add(object.get());

        } catch (const SceneError& e) {
            LOG_ERROR("Could not process scene - {}", e.what());
            throw e;
        }
    }

    void processEntities(
        const Node& root, const std::string& tag, auto& container, const auto&& factories
    ) {
        const auto entityType = tag.substr(0, tag.size() - 1);

        for (auto& entity : fieldFrom(root).withName(tag).asArray().get()) {
            const auto& [name, type] = getNameAndType(entity);

            LOG_INFO("Processing {}: name {}, type {}", entityType, name, type);

            ASSERT(not container.contains(name), fmt::format("{} already exists", name));
            ASSERT(factories.contains(type), "{} object type not supported", type);

            container[name] = factories.at(type)(this, entity);
        }
    }

    StrKeyMap<TextureFactory> getTexturesFactories() const {
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

    StrKeyMap<ObjectFactory> getObjectsFactories() const {
        static auto createRectangle = [](SceneLoader* loader, const Node& node) {
            auto orientation = loader->fieldFrom(node)
                                   .withName("orientation")
                                   .ofType<std::string>()
                                   .get();

            auto edges =
                loader->fieldFrom(node).withName("edges").ofType<glm::vec4>().get();
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

    StrKeyMap<MaterialFactory> getMaterialsFactories() const {
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
            const auto refractionIndex = loader->fieldFrom(node)
                                             .withName("index-of-refraction")
                                             .ofType<float>()
                                             .get();

            return std::make_unique<mat::Dielectric>(refractionIndex);
        };

        return StrKeyMap<MaterialFactory>{
            {"Lambertian",   createLambertian  },
            {"Metal",        createMetal       },
            {"Dielectric",   createDielectric  },
            {"DiffuseLight", createDiffuseLight},
        };
    }

    std::pair<std::string, std::string> getNameAndType(const Node& node) {
        return {
            fieldFrom(node).withName("name").ofType<std::string>().get(),
            fieldFrom(node).withName("type").ofType<std::string>().get()};
    }
};

}  // namespace scene

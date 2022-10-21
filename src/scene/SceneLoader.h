#pragma once

#include <unordered_map>
#include <memory>

#include <kc/core/Log.h>
#include <kc/core/ErrorBase.hpp>
#include <kc/json/JsonConfigLoader.hpp>
#include <kc/json/Utils.hpp>

#include "geom/IntersectableCollection.h"
#include "geom/Intersectable.h"
#include "mat/Material.h"
#include "texture/Texture.h"
#include "Camera.h"

DEFINE_ERROR(SceneError);

namespace scene {

using namespace kc::json;

template <typename T> using StrKeyMap = std::unordered_map<std::string, T>;

struct Scene {
    StrKeyMap<std::unique_ptr<texture::Texture>> textures;
    StrKeyMap<std::unique_ptr<mat::Material>> materials;
    StrKeyMap<std::unique_ptr<geom::Intersectable>> objects;

    geom::IntersectableCollection world;

    std::unique_ptr<Camera> camera;
};

class SceneLoader : public JsonConfigLoader<Scene>, NodeHelper<SceneError> {
   private:
    using Helper = NodeHelper<SceneError>;

    using TextureFactory = std::function<
        std::unique_ptr<texture::Texture>(SceneLoader* loader, const Node&)>;

    using ObjectFactory = std::function<
        std::unique_ptr<geom::Intersectable>(SceneLoader* loader, const Node&)>;

    using TransformFactory = std::function<std::unique_ptr<
        geom::Intersectable>(SceneLoader* loader, const Node&, geom::Intersectable*)>;

    using VfxFactory = TransformFactory;

    using MaterialFactory =
        std::function<std::unique_ptr<mat::Material>(SceneLoader* loader, const Node&)>;

    void processFields(const Node& root) override;

    void processCamera(const Node& root);

    static auto insert(auto& container, const std::string& key, auto&& value) {
        auto iterator = container.insert({key, std::move(value)}).first;
        auto& object  = iterator->second;

        return object.get();
    }

    static std::string getEntityType(const std::string& tag) {
        return tag.substr(0, tag.size() - 1);
    }

    void validate(
        const std::string& tag, const std::string& type, const std::string& name,
        auto& container, const auto& factories
    ) {
        LOG_INFO("Processing {}: name {}, type {}", getEntityType(tag), name, type);

        ASSERT(not container.contains(name), fmt::format("{} already exists", name));
        ASSERT(factories.contains(type), "{} object type not supported", type);
    }

    void processEntities(
        const Node& root, const std::string& tag, auto& container, const auto& factories
    ) {
        for (auto& entity : fieldFrom(root).withName(tag).asArray().get()) {
            const auto& [name, type] = getNameAndType(entity);
            validate(tag, type, name, container, factories);
            container[name] = factories.at(type)(this, entity);
        }
    }

    geom::Intersectable* processWrappers(
        const Node& root, const std::string& tag, auto& container, const auto& factories,
        geom::Intersectable* object
    ) {
        if (root.isMember(tag)) {
            const auto descriptions = fieldFrom(root).withName(tag).asArray().get();

            for (const auto& description : descriptions) {
                const auto& [name, type] = getNameAndType(description);

                ASSERT(factories.contains(type), "Could not find {}", type);
                LOG_INFO("Processing: {}/{}", type, name);

                const auto& factory = factories.at(type);

                object = insert(container, name, factory(this, description, object));
            }
        }
        return object;
    }

    void processObjects(const Node& root);

    StrKeyMap<TextureFactory> getTexturesFactories() const;

    StrKeyMap<ObjectFactory> getObjectsFactories() const;

    StrKeyMap<TransformFactory> getTransformFactories() const;

    StrKeyMap<VfxFactory> getVfxFactories() const;

    StrKeyMap<MaterialFactory> getMaterialsFactories() const;

    std::pair<std::string, std::string> getNameAndType(const Node& node);
};

}  // namespace scene

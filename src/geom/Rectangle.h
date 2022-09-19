#pragma once

#include <memory>

#include "Intersectable.h"
#include "mat/Material.h"

namespace geom {

class RectangleXY : public Intersectable {
   public:
    RectangleXY() {}

    RectangleXY(float _x0, float _x1, float _y0, float _y1, float _k, mat::Material* mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat){};

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& r, float t_min, float t_max
    ) override {
        auto t = (k - r.getOrigin().z) / r.getDirection().z;
        if (t < t_min || t > t_max) return {};
        auto x = r.getOrigin().x + t * r.getDirection().x;
        auto y = r.getOrigin().y + t * r.getDirection().y;
        if (x < x0 || x > x1 || y < y0 || y > y1) return {};

        return IntersectRecord{
            .hitPoint = r.at(t),
            .normal   = calculateNormal(r, glm::vec3{0.0f, 0.0f, 1.0f}),
            .t        = t,
            .u        = (x - x0) / (x1 - x0),
            .v        = (y - y0) / (y1 - y0),
            .material = mp,
        };
    }

    bvh::AABB getBoundingBox() const override {
        return bvh::AABB{
            glm::vec3{x0, y0, k - 0.00001f},
            glm::vec3{x1, y1, k + 0.00001f},
        };
    }

   public:
    mat::Material* mp;
    float x0, x1, y0, y1, k;
};

class RectangleXZ : public Intersectable {
   public:
    RectangleXZ() {}

    RectangleXZ(float _x0, float _x1, float _z0, float _z1, float _k, mat::Material* mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat){};

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& r, float t_min, float t_max
    ) override {
        auto t = (k - r.getOrigin().y) / r.getDirection().y;
        if (t < t_min || t > t_max) return {};
        auto x = r.getOrigin().x + t * r.getDirection().x;
        auto z = r.getOrigin().z + t * r.getDirection().z;
        if (x < x0 || x > x1 || z < z0 || z > z1) return {};

        return IntersectRecord{
            .hitPoint = r.at(t),
            .normal   = calculateNormal(r, glm::vec3{0.0f, 1.0f, 0.0f}),
            .t        = t,
            .u        = (x - x0) / (x1 - x0),
            .v        = (z - z0) / (z1 - z0),
            .material = mp,
        };
    }

    bvh::AABB getBoundingBox() const override {
        return bvh::AABB{
            glm::vec3{x0, k - 0.00001f, z0},
            glm::vec3{x1, k + 0.00001f, z1},
        };
    }

   public:
    mat::Material* mp;
    float x0, x1, z0, z1, k;
};

class RectangleYZ : public Intersectable {
   public:
    RectangleYZ() {}

    RectangleYZ(float _y0, float _y1, float _z0, float _z1, float _k, mat::Material* mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat){};

    std::optional<IntersectRecord> intersect(
        const kc::math::Ray& r, float t_min, float t_max
    ) override {
        auto t = (k - r.getOrigin().x) / r.getDirection().x;
        if (t < t_min || t > t_max) return {};
        auto y = r.getOrigin().y + t * r.getDirection().y;
        auto z = r.getOrigin().z + t * r.getDirection().z;
        if (z < z0 || z > z1 || y < y0 || y > y1) return {};

        return IntersectRecord{
            .hitPoint = r.at(t),
            .normal   = calculateNormal(r, glm::vec3{1.0f, 0.0f, 0.0f}),
            .t        = t,
            .u        = (y - y0) / (y1 - y0),
            .v        = (z - z0) / (z1 - z0),
            .material = mp,
        };
    }

    bvh::AABB getBoundingBox() const override {
        return bvh::AABB{
            glm::vec3{k - 0.00001f, y0, z0},
            glm::vec3{k + 0.00001f, y1, z1},
        };
    }

   public:
    mat::Material* mp;
    float y0, y1, z0, z1, k;
};

namespace Rectangle {

enum class Orientation { xy, xz, yz };

inline Orientation orientationFromString(const std::string& input) {
    if (input == "XY")
        return Orientation::xy;
    else if (input == "XZ")
        return Orientation::xz;
    else if (input == "YZ")
        return Orientation::yz;

    throw std::runtime_error("Unknown orientation string");
}

inline std::unique_ptr<Intersectable> create(
    Orientation orientation, float _y0, float _y1, float _z0, float _z1, float _k,
    mat::Material* mat
) {
    switch (orientation) {
        case Orientation::xy:
            return std::make_unique<RectangleXY>(_y0, _y1, _z0, _z1, _k, mat);

        case Orientation::xz:
            return std::make_unique<RectangleXZ>(_y0, _y1, _z0, _z1, _k, mat);

        case Orientation::yz:
            return std::make_unique<RectangleYZ>(_y0, _y1, _z0, _z1, _k, mat);

        default:
            return nullptr;
    }
}

inline std::unique_ptr<Intersectable> create(
    Orientation orientation, glm::vec4 edges, float _k, mat::Material* mat
) {
    return create(orientation, edges[0], edges[1], edges[2], edges[3], _k, mat);
}

};  // namespace Rectangle

}  // namespace geom
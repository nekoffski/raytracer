#pragma once

#include <optional>
#include <vector>

#include "../math/Ray.h"
#include "../math/Vec3.hpp"

struct IntersectRecord {
    Vec3 p;
    float t;
    Vec3 normal;
    bool frontFace;

    void setFaceNormal(Ray& r, Vec3& outwardNormal) {
        frontFace = r.Direction().dot(outwardNormal) < 0.0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class Shape {
public:
    virtual std::optional<IntersectRecord> intersect(Ray& r, float tMin, float tMax) = 0;
};

class ShapeList {
public:
    void push(Shape* shape) {
        m_shapes.push_back(shape);
    }

    void clear() {
        m_shapes.clear();
    }

    std::pair<bool, IntersectRecord> intersect(Ray& r, float tMin, float tMax) {
        float closest = tMax;
        IntersectRecord res;
        bool found = false;

        for (auto& shape : m_shapes) {
            if (auto f = shape->intersect(r, tMin, closest); f) {
                closest = f->t;
                res = *f;
                found = true;
            }
        }

        return { found, res };
    }

private:
    std::vector<Shape*> m_shapes;
};

#pragma once

#include "Shape.h"

class Sphere : public Shape {
public:
    explicit Sphere(Vec3 c, float r)
        : m_center(c)
        , m_radius(r) {
    }

    std::optional<IntersectRecord> intersect(Ray& r, float tMin, float tMax) override {
        Vec3 oc = r.Origin() - m_center;
        float a = r.Direction().length() * r.Direction().length(); // to optimize
        float half_b = oc.dot(r.Direction());
        float c = oc.length() * oc.length() - m_radius * m_radius;
        float disc = half_b * half_b - a * c;

        if (disc <= 0.0f)
            return {};

        float root = std::sqrt(disc);
        float tmp = (-half_b - root) / a;

        if (tmp < tMax && tmp > tMin) {
            Vec3 ff = r.at(tmp);
            auto rec = IntersectRecord{ ff, tmp, (ff - m_center) / m_radius, false };
            Vec3 outward_normal = (rec.p - m_center) / m_radius;
            rec.setFaceNormal(r, outward_normal);
            return rec;
        } else {
            tmp = (-half_b + root) / a;
            if (tmp < tMax && tmp > tMin) {
                Vec3 ff = r.at(tmp);
                auto rec = IntersectRecord{ ff, tmp, (ff - m_center) / m_radius, false };
                Vec3 outward_normal = (rec.p - m_center) / m_radius;
                rec.setFaceNormal(r, outward_normal);
                return rec;
            }
        }
        return {};
    }

private:
    Vec3 m_center;
    float m_radius;
};

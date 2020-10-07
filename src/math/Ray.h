#pragma once

#include "Vec3.hpp"

class Ray {
public:
    explicit Ray(Vec3 o, Vec3 d)
        : m_origin(o)
        , m_direction(d) {
    }

    Vec3 Origin() {
        return m_origin;
    }

    Vec3 Direction() {
        return m_direction;
    }

    Vec3 at(float t) {
        return m_origin + m_direction * t;
    }

private:
    Vec3 m_origin;
    Vec3 m_direction;
};
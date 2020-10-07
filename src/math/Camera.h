#pragma once

#include "Ray.h"
#include "Vec3.hpp"

class Camera {
public:
    explicit Camera(float aspect, float viewportHeight, float focalLength);

    Vec3 Origin();
    Vec3 LowerLeft();
    Vec3 Horizontal();
    Vec3 Vertical();
    Ray getRay(float u, float v);

private:
    Vec3 m_origin;
    Vec3 m_horizontal;
    Vec3 m_vertical;
    Vec3 m_lowerLeft;
};
#include "Camera.h"

Camera::Camera(float aspect, float viewportHeight, float focalLength)
    : m_origin(0.0f, 0.0f, 0.0)
    , m_horizontal(viewportHeight * aspect, 0.0f, 0.0f)
    , m_vertical(0.0f, viewportHeight, 0.0f)
    , m_lowerLeft(m_origin - m_horizontal / 2.0f - m_vertical / 2.0f - Vec3{ 0.0f, 0.0f, focalLength }) {
}

Vec3 Camera::Origin() {
    return m_origin;
}

Vec3 Camera::LowerLeft() {
    return m_lowerLeft;
}

Vec3 Camera::Horizontal() {
    return m_horizontal;
}

Vec3 Camera::Vertical() {
    return m_vertical;
}

Ray Camera::getRay(float u, float v) {
    return Ray{
        m_origin, m_lowerLeft + m_horizontal * u + m_vertical * v - m_origin
    };
}
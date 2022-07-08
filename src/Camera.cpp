#include "Camera.h"

#include <kc/math/Utils.hpp>

glm::vec3 randomVecInUnitDisk() {
    while (1) {
        auto p = glm::vec3{
            kc::math::random<float>(-1.0f, 1.0f), kc::math::random<float>(-1.0f, 1.0f),
            0.0f};
        if (glm::length(p) >= 1) continue;

        return p;
    }
}

Camera::Camera(
    const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up,
    float vfov,  // vertical field-of-view in degrees
    float aspectRatio, float aperture, float focusDist
) {
    auto theta           = glm::radians(vfov);
    auto h               = std::tan(theta / 2);
    float viewportHeight = 2.0f * h;
    float viewportWidth  = aspectRatio * viewportHeight;

    w = glm::normalize(lookFrom - lookAt);
    u = glm::normalize(glm::cross(up, w));
    v = glm::cross(w, u);

    auto focal_length = 1.0f;

    origin     = lookFrom;
    horizontal = focusDist * viewportWidth * u;
    vertical   = focusDist * viewportHeight * v;

    lowerLeftCorner = origin - horizontal / 2.0f - vertical / 2.0f - focusDist * w;
}

kc::math::Ray Camera::getRay(float s, float t) const {
    glm::vec3 rd     = lensRadius * randomVecInUnitDisk();
    glm::vec3 offset = u * rd.x + v * rd.y;

    return kc::math::Ray(
        origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset
    );
}

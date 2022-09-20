#pragma once

#include <kc/math/Ray.h>
#include <glm/glm.hpp>

class Camera {
   public:
    Camera(
        const glm::vec3& lookFrom, const glm::vec3& lookAt, const glm::vec3& up,
        float verticalFov, float aspect, float aperture, float focusDistance
    );

    kc::math::Ray getRay(float s, float t) const;

   private:
    glm::vec3 origin;
    glm::vec3 lowerLeftCorner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;
    float lensRadius;
};

// struct Triangle : Intersectable {
//     glm::vec3 v0;
//     glm::vec3 v1;
//     glm::vec3 v2;

//     glm::vec3 color;

//     glm::vec3 getColor() const override { return color; }

//     bool intersect(const Ray &ray, float &t0) const override {
//         return intersectTriangle(v0, v1, v2, ray, t0);
//     }

//     static bool intersectTriangle(
//         const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, const Ray &ray,
//         float &t0
//     ) {
//         auto origin    = ray.getOrigin();
//         auto direction = ray.getDirection();

//         auto v0v1 = v1 - v0;
//         auto v0v2 = v2 - v0;

//         auto pvec = glm::cross(direction, v0v2);
//         float det = glm::dot(v0v1, pvec);

//         constexpr float epsilon = 0.0001f;

//         if (std::fabs(det) < epsilon)  // ray is parallel
//             return false;

//         float invDet = 1.0f / det;

//         auto tvec = origin - v0;

//         float u = glm::dot(tvec, pvec) * invDet;
//         if (u < 0 || u > 1) return false;

//         auto qvec = glm::cross(tvec, v0v1);
//         float v   = glm::dot(direction, qvec) * invDet;

//         if (v < 0 || u + v > 1) return false;

//         t0 = glm::dot(v0v2, qvec) * invDet;

//         return true;
//     }
// };

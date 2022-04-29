
// struct TriangleMesh : Intersectable {
//     std::vector<int> indices;
//     std::vector<glm::vec3> vertices;

//     glm::vec3 color;

//     glm::vec3 getColor() const override { return color; }

//     bool intersect(const Ray &ray, float &t0) const override {
//         bool intersected = false;
//         float tnear      = std::numeric_limits<float>::max();

//         for (int i = 0; i < indices.size(); i += 3) {
//             if (Triangle::intersectTriangle(
//                     vertices[indices[i]], vertices[indices[i + 1]],
//                     vertices[indices[i + 2]], ray, t0
//                 ) &&
//                 t0 < tnear) {
//                 tnear = t0;
//                 intersected |= true;
//             }
//         }

//         return intersected;
//     }

//     void generate() {}
// };

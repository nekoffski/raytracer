#pragma once

#include <vector>

#include <kc/math/Utils.hpp>
#include <kc/core/Log.h>

namespace texture {

class Perlin {
    template <typename T, int X, int Y, int Z>
    using Array3D = std::array<std::array<std::array<T, Z>, Y>, X>;

   public:
    Perlin() {
        m_randomFloats.reserve(s_pointCount);
        m_randomVectors.reserve(s_pointCount);

        for (int i = 0; i < s_pointCount; ++i) {
            m_randomFloats.push_back(kc::math::random<float>());
            m_randomVectors.push_back(glm::normalize(kc::math::randomVec3(-1.0f, 1.0f)));
        }

        generatePermutation(m_permX);
        generatePermutation(m_permY);
        generatePermutation(m_permZ);
    }

    float getNoise(const glm::vec3& p) const {
        auto u = p.x - std::floor(p.x);
        auto v = p.y - std::floor(p.y);
        auto w = p.z - std::floor(p.z);

        auto i = static_cast<int>(std::floor(p.x));
        auto j = static_cast<int>(std::floor(p.y));
        auto k = static_cast<int>(std::floor(p.z));

        Array3D<glm::vec3, 2, 2, 2> weights;

        for (int di = 0; di < 2; ++di) {
            for (int dj = 0; dj < 2; ++dj) {
                for (int dk = 0; dk < 2; ++dk) {
                    weights[di][dj][dk] = m_randomVectors
                        [m_permX[(i + di) & 255] ^ m_permY[(j + dj) & 255] ^
                         m_permZ[(k + dk) & 255]];
                }
            }
        }

        return trilinearInterpolate(weights, u, v, w);
    }

   private:
    static float trilinearInterpolate(
        Array3D<glm::vec3, 2, 2, 2>& array, float u, float v, float w
    ) {
        float sum          = 0.0f;
        constexpr int size = 2;

        // Hermitian smoothing
        float uu = u * u * (3.0f - 2.0f * u);
        float vv = v * v * (3.0f - 2.0f * v);
        float ww = w * w * (3.0f - 2.0f * w);

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                for (int k = 0; k < size; ++k) {
                    glm::vec3 weight{u - i, v - j, w - k};

                    sum +=
                        ((i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                         (j * ww + (1 - k) * (1 - ww)) *
                         glm::dot(array[i][j][k], weight));
                }
            }
        }

        return sum;
    }

    static constexpr int s_pointCount = 128;

    std::vector<glm::vec3> m_randomVectors;
    std::vector<float> m_randomFloats;

    std::vector<int> m_permX;
    std::vector<int> m_permY;
    std::vector<int> m_permZ;

    static void generatePermutation(std::vector<int>& p) {
        p.resize(s_pointCount);
        std::iota(p.begin(), p.end(), 0);

        static auto permute = [](std::vector<int>& p) -> void {
            for (int i = p.size() - 1; i > 0; i--)
                std::swap(p[i], p[kc::math::random<int>(0, i)]);
        };

        permute(p);
    }
};

}  // namespace texture

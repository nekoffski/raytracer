#pragma once

#include <array>
#include <numeric>

#include <glm/glm.hpp>

#include "Renderer.h"

#include "Filter.h"

namespace image {

class MeanFilter : public Filter {
    using Batch = std::vector<glm::vec3>;

   public:
    explicit MeanFilter(int batchSize = defaultBatchSize)
        : m_batchSize(batchSize), m_invertedBatchSize(1.0f / m_batchSize) {}

    Renderer::Framebuffer apply(const Renderer::Framebuffer& input) override {
        auto output = input;

        static constexpr int kernelSize = 3;

        for (int j = 0; j < height; ++j)
            for (int i = 0; i < width; ++i)
                output[j * width + i] = getAverage(getBatch(i, j, input));

        return output;
    }

    glm::vec3 getAverage(const Batch& batch) {
        auto sum = std::accumulate(batch.begin(), batch.end(), glm::vec3{0.0f});

        return m_invertedBatchSize * sum;
    }

    Batch getBatch(int i, int j, const Renderer::Framebuffer& input) {
        Batch batch;
        batch.reserve(m_batchSize);

        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                int ii = i + x;
                int jj = j + y;

                if (ii < 0) ii = 0;

                if (jj < 0) jj = 0;

                if (ii > width - 1) ii = width - 1;

                if (jj > height - 1) jj = height - 1;

                batch.push_back(input[jj * width + ii]);
            }
        }

        return batch;
    }

    static constexpr int defaultBatchSize = 3;
    static constexpr int width            = 800;  // TODO: make it configurable
    static constexpr int height           = 800;

    int m_batchSize;
    float m_invertedBatchSize;
};

}  // namespace image
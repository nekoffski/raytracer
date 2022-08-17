#include "Image.h"

#include <kc/core/Scope.hpp>
#include <stb.h>

#include <cstring>
#include <stdexcept>

#include "Texture.h"

namespace texture {

Image::Image(const std::string& filename) {
    const auto data =
        stbi_load(filename.c_str(), &m_width, &m_height, &m_channels, desiredChannels);

    if (not data) throw std::runtime_error{"Could not load image"};

    ON_SCOPE_EXIT { delete data; };

    const auto size = m_width * m_height * m_channels;

    m_data.resize(size);
    std::memcpy(m_data.data(), data, size);

    m_bytesPerScanline = m_channels * m_width;
}

glm::vec3 Image::getColor(float u, float v, const glm::vec3& p) const {
    u = std::clamp(u, 0.0f, 1.0f);
    v = 1.0f - std::clamp(v, 0.0f, 1.0f);

    auto i = static_cast<int>(u * m_width);
    auto j = static_cast<int>(v * m_height);

    if (i >= m_width) i = m_width - 1;
    if (j >= m_height) j = m_height - 1;

    const auto colorScale = 1.0f / 255.0f;
    const auto pixel      = m_data.data() + j * m_bytesPerScanline + i * m_channels;

    return colorScale * glm::vec3{pixel[0], pixel[1], pixel[2]};
}

}  // namespace texture
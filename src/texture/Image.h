#pragma once

#include <vector>
#include <string>

#include "Texture.h"

namespace texture {

class Image : public Texture {
    static constexpr int desiredChannels = 3;

   public:
    explicit Image(const std::string& filename);

    glm::vec3 getColor(float u, float v, const glm::vec3& p) const override;

   private:
    std::vector<unsigned char> m_data;

    int m_width;
    int m_height;
    int m_channels;
    int m_bytesPerScanline;
};

}  // namespace texture

#pragma once

#include <cmath>
#include <random>

class Vec3 {
public:
    explicit Vec3(float x = 0, float y = 0.0f, float z = 0.0f)
        : m_x(x)
        , m_y(y)
        , m_z(z) {
    }

    static Vec3 random(float min, float max) {
        static std::random_device randomDevice;
        std::mt19937 generator(randomDevice());
        std::uniform_real_distribution<float> distribution(min, max);

        return Vec3{
            distribution(generator), distribution(generator), distribution(generator)
        };
    }

    static Vec3 randomInUnitSphere() {
        while (true) {
            auto v = random(-1.0f, 1.0f);
            if (v.length() >= 1.0f)
                return v;
        }
    }

    static Vec3 randomInHemisphere(const Vec3& norm) {
        auto v = randomInUnitSphere();
        if (v.dot(norm) > 0.0)
            return v;
        return -v;
    }

    float X() const {
        return m_x;
    }

    float Y() const {
        return m_y;
    }

    float Z() const {
        return m_z;
    }

    float length() {
        return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    }

    float lengthSquared() {
        auto sq = length();
        return sq * sq;
    }

    float dot(const Vec3& rhs) {
        return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
    }

    Vec3 normalize() {
        float l = length();
        return Vec3{
            m_x / l,
            m_y / l,
            m_z / l
        };
    }

    Vec3 operator-() {
        return Vec3{
            -m_x, -m_y, -m_z
        };
    }

    Vec3 operator+(const Vec3& rhs) {
        return Vec3{
            m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z
        };
    }

    Vec3 operator-(const Vec3& rhs) {
        return Vec3{
            m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z
        };
    }

    Vec3 operator*(float rhs) {
        return Vec3{
            m_x * rhs, m_y * rhs, m_z * rhs
        };
    }

    Vec3 operator/(float rhs) {
        return Vec3{
            m_x / rhs, m_y / rhs, m_z / rhs
        };
    }

private:
    float m_x;
    float m_y;
    float m_z;
};

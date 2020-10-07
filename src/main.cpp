#include <iostream>
#include <random>

#include "geometry/Shape.h"
#include "geometry/Sphere.h"
#include "math/Camera.h"
#include "math/Ray.h"
#include "math/Vec3.hpp"
#include "math/utils.hpp"

void printColor(Vec3 v, int samplesPerPixel) {
    auto r = v.X();
    auto g = v.Y();
    auto b = v.Z();

    auto scale = 1.0f / samplesPerPixel;

    r *= scale;
    g *= scale;
    b *= scale;

    int ir = 256.0 * clamp(std::sqrt(r), 0.0, 0.999);
    int ig = 256.0 * clamp(std::sqrt(g), 0.0, 0.999);
    int ib = 256.0 * clamp(std::sqrt(b), 0.0, 0.999);
    std::cout << (int)ir << ' ' << (int)ig << ' ' << (int)ib << '\n';
}

Vec3 rayColor(Ray r, ShapeList& world, int depth) {
    if (depth < 0)
        return Vec3{ 0.0f, 0.0f, 0.0f };

    if (auto [found, ff] = world.intersect(r, 0.001f, 999999.0f); found) {
        auto target = ff.p + ff.normal + Vec3::randomInHemisphere(ff.normal);
        return rayColor(Ray{ ff.p, target - ff.p }, world, depth - 1) * 0.5;
    }

    auto dir = r.Direction().normalize();
    auto t = 0.5f * (dir.Y() + 1.0f);
    // std::cout << t;
    return Vec3{ 1.0f, 1.0f, 1.0f } * (1.0f - t) + Vec3{ 0.5f, 0.7f, 1.0f } * t;
}

int main() {
    const int W = 800;
    const int H = 600;
    const int MAX_DEPTH = 1;
    const int SAMPLES_PER_PIXEL = 1;

    const float ASPECT = static_cast<float>(W) / (float)H;

    const float viewportHeight = 2.0f;
    const float viewportWidth = viewportHeight * ASPECT;
    const float focalLength = 1.0f;

    auto origin = Vec3{ 0.0f, 0.0f, 0.0f };
    auto hor = Vec3{ viewportWidth, 0.0f, 0.0f };
    auto vert = Vec3{ 0.0f, viewportHeight, 0.0f };
    auto lowerLeft = origin - (hor / 2.0f) - (vert / 2.0f) - Vec3{ 0.0f, 0.0f, focalLength };

    ShapeList world;

    Sphere s1(Vec3{ 0.0f, 0.0f, -1.0f }, 0.5f);
    Sphere s2(Vec3{ 0.0f, -100.5f, -1.0f }, 100.0f);

    world.push(&s1);
    world.push(&s2);

    Camera camera(ASPECT, viewportHeight, focalLength);

    std::cout << "P3\n"
              << W << ' ' << H << '\n'
              << 255 << '\n';

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (int j = H - 1; j >= 0; --j) {
        for (int i = W - 1; i >= 0; --i) {
            Vec3 color{ 0.0f, 0.0f, 0.0f };

            for (int z = 0; z < SAMPLES_PER_PIXEL; ++z) {
                float u = (static_cast<float>(i) + dis(gen)) / ((float)W - 1.0f);
                float v = (static_cast<float>(j) + dis(gen)) / ((float)H - 1.0f);

                Ray r = camera.getRay(u, v);
                color = color + rayColor(r, world, MAX_DEPTH);
            }

            printColor(color, SAMPLES_PER_PIXEL);
        }
    }

    return 0;
}

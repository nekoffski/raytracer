#include <kc/core/Log.h>

int main() {
    kc::core::initLogging("raytracer");
    LOG_INFO("Starting");

    return 0;
}
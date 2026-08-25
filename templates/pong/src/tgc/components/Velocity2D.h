#pragma once

#include <glm/vec2.hpp>

namespace tgc::components {
    struct Velocity2D {
        glm::vec2 linear{};
        float angular{};
    };
} // namespace tgc::components

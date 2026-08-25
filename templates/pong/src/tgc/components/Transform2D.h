#pragma once

#include <glm/vec2.hpp>

namespace tgc::components {
    struct Transform2D {
        glm::vec2 position{};
        float rotation{};
        glm::vec2 scale{1.0F};
    };
} // namespace tgc::components

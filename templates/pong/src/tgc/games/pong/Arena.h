#pragma once

#include <glm/vec4.hpp>

namespace tgc::games::pong {
    struct Arena {
        float width{16.0F};
        float height{9.0F};
        float wallThickness{0.18F};
        glm::vec4 clearColor{0.015F, 0.02F, 0.035F, 1.0F};
    };
} // namespace tgc::games::pong

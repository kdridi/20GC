#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace tgc::components {
    struct Rectangle {
        glm::vec2 size{1.0F};
        glm::vec4 color{1.0F};
    };
} // namespace tgc::components

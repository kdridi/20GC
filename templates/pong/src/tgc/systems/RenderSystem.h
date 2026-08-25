#pragma once

#include "tgc/gpu/GpuRenderer.h"

#include <entt/entity/registry.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace tgc::systems {
    class RenderSystem {
    public:
        static bool render(
            tgc::gpu::GpuRenderer &renderer,
            const entt::registry &registry,
            glm::vec2 worldSize,
            glm::vec4 clearColor);
    };
} // namespace tgc::systems

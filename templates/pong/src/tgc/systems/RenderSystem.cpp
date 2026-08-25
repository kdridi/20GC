#include "tgc/systems/RenderSystem.h"

#include "tgc/components/Rectangle.h"
#include "tgc/components/Transform2D.h"
#include "tgc/gpu/GpuRenderer.h"

#include <SDL3/SDL_log.h>

#include <cmath>

namespace tgc::systems {
    bool RenderSystem::render(
        tgc::gpu::GpuRenderer &renderer,
        const entt::registry &registry,
        glm::vec2 worldSize,
        glm::vec4 clearColor)
    {
        if (!std::isfinite(worldSize.x) || !std::isfinite(worldSize.y) || worldSize.x <= 0.0F ||
            worldSize.y <= 0.0F || !std::isfinite(clearColor.r) || !std::isfinite(clearColor.g) ||
            !std::isfinite(clearColor.b) || !std::isfinite(clearColor.a)) {
            SDL_Log("Invalid render configuration");
            return false;
        }

        renderer.beginFrame(worldSize, clearColor);

        registry.view<const tgc::components::Transform2D, const tgc::components::Rectangle>().each(
            [&](const tgc::components::Transform2D &transform, const tgc::components::Rectangle &rectangle) {
                renderer.drawRectangle(
                    transform.position,
                    transform.rotation,
                    rectangle.size,
                    transform.scale,
                    rectangle.color);
            });

        return renderer.endFrame();
    }
} // namespace tgc::systems

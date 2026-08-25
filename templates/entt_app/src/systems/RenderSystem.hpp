#pragma once

#include <SDL3/SDL.h>
#include <entt/entt.hpp>

namespace core {
    class Window;
}

namespace systems {
    class RenderSystem {
    public:
        RenderSystem(SDL_Renderer &_renderer, const entt::registry &_registry)
            : m_renderer(_renderer), m_registry(_registry) {}

        void render() const noexcept;

    private:
        SDL_Renderer &m_renderer;
        const entt::registry &m_registry;
    };
} // namespace systems
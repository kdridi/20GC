#include "systems/RenderSystem.hpp"

#include "components/AABB.hpp"
#include "components/Color.hpp"

#include <SDL3/SDL.h>

namespace systems {
    void RenderSystem::render() const noexcept
    {
        SDL_SetRenderDrawColor(&m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(&m_renderer);

        m_registry.view<components::Color, components::AABB>().each([&](const auto &_color, const auto &_aabb) -> void {
            if (!SDL_SetRenderDrawColor(&m_renderer, _color.data[0], _color.data[1], _color.data[2], _color.data[3])) {
                SDL_Log("SDL_SetRenderDrawColor Error: %s", SDL_GetError());
                return;
            }

            const SDL_FRect rect{
                .x = static_cast<float>(_aabb.x()),
                .y = static_cast<float>(_aabb.y()),
                .w = static_cast<float>(_aabb.width()),
                .h = static_cast<float>(_aabb.height()),
            };

            SDL_RenderFillRect(&m_renderer, &rect);
        });

        // Draw everything.
        SDL_RenderPresent(&m_renderer);
    }
} // namespace systems

#pragma once

#include <core/CleanupStack.hpp>

struct SDL_Window;
struct SDL_Renderer;

namespace core {
    class Window : private CleanupStack {
    public:
        bool init() noexcept;

        SDL_Renderer *renderer() const noexcept { return m_renderer; }

    private:
        SDL_Window *m_window{};
        SDL_Renderer *m_renderer{};
    };
} // namespace core
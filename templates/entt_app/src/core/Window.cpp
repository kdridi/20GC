#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <core/Window.hpp>

#include <SDL3/SDL.h>

namespace core {
    bool Window::init() noexcept
    {
        if (!SDL_CreateWindowAndRenderer("Pong", 640, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN, &m_window, &m_renderer)) {
            SDL_Log("SDL_CreateWindowAndRenderer Error: %s", SDL_GetError());
            return false;
        }
        defer([window = m_window]() noexcept -> void { SDL_DestroyWindow(window); });
        defer([renderer = m_renderer]() noexcept -> void { SDL_DestroyRenderer(renderer); });

        if (!SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
            SDL_Log("SDL_SetWindowPosition Error: %s", SDL_GetError());
            return false;
        }

        if (!SDL_ShowWindow(m_window)) {
            SDL_Log("SDL_ShowWindow Error: %s", SDL_GetError());
            return false;
        }

        return true;
    }

} // namespace core
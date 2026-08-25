#include "core/Game.hpp"
#include "components/AABB.hpp"
#include "components/Color.hpp"

#include <SDL3/SDL.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_scancode.h>
#include <cstdlib>

namespace core {
    bool Game::init() noexcept
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            SDL_Log("SDL_Init Error: %s", SDL_GetError());
            return false;
        }
        defer([]() noexcept -> void { SDL_Quit(); });

        if (!m_window.init())
            return false;

        m_render_system = std::make_unique<systems::RenderSystem>(*m_window.renderer(), m_registry);
        if (!m_render_system)
            return false;

        if (!initEntities())
            return false;

        return true;
    }

    int Game::run() noexcept
    {
        const constexpr double dt = 1000.0 / 60.0;

        double total_time = 0.0;
        double accumulator = 0.0;
        double current_time = SDL_GetTicks();
        double new_time = 0.0;
        double frame_time = 0.0;

        m_running = true;
        while (m_running) {
            new_time = SDL_GetTicks();
            frame_time = new_time - current_time;
            current_time = new_time;

            accumulator += frame_time;

            runEvent();

            while (accumulator >= dt) {
                runUpdate(dt);

                accumulator -= dt;
                total_time += dt;
            }

            runRender();
        }

        return EXIT_SUCCESS;
    }

    bool Game::initEntities() noexcept
    {
        const auto player_paddle = m_registry.create();
        const auto cpu_paddle = m_registry.create();
        const auto ball = m_registry.create();

        m_registry.emplace<components::AABB>(cpu_paddle, 10, 10, 10, 500);
        m_registry.emplace<components::Color>(cpu_paddle, 0, 255, 0, 255);

        m_registry.emplace<components::AABB>(player_paddle, 800, 10, 10, 500);
        m_registry.emplace<components::Color>(player_paddle, 0, 0, 255, 255);

        m_registry.emplace<components::AABB>(ball, 50, 50, 10, 10);
        m_registry.emplace<components::Color>(ball, 255, 0, 0, 255);

        return true;
    }

    void Game::runEvent() noexcept
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                m_running = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.repeat == false && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                SDL_Event quit{};
                quit.type = SDL_EVENT_QUIT;
                SDL_PushEvent(&quit);
            }
        }
    }

    void Game::runUpdate(double dt) noexcept
    {
    }

    void Game::runRender() const noexcept
    {
        m_render_system->render();
    }
} // namespace core

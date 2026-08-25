#include "tgc/core/Application.h"

#include "tgc/core/GameBootstrap.h"
#include "tgc/core/IGame.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <exception>

namespace tgc::core {
    namespace {
        const char *registrationError(GameRegistrationStatus status) noexcept
        {
            switch (status) {
            case GameRegistrationStatus::missing:
                return "No game is registered";
            case GameRegistrationStatus::duplicate:
                return "Multiple games were registered";
            case GameRegistrationStatus::invalidFactory:
                return "The registered game factory is invalid";
            case GameRegistrationStatus::storageFailure:
                return "The game registration could not be stored";
            case GameRegistrationStatus::registered:
                return "The game registration is inconsistent";
            }
            return "Unknown game registration error";
        }
    } // namespace

    Application::Application() = default;

    Application::~Application() noexcept
    {
        shutdown();
    }

    SDL_AppResult Application::reportException() noexcept
    {
        try {
            throw;
        } catch (const std::exception &exception) {
            SDL_Log("Unhandled exception: %s", exception.what());
        } catch (...) {
            SDL_Log("Unhandled unknown exception");
        }
        return SDL_APP_FAILURE;
    }

    SDL_AppResult Application::init() noexcept
    {
        try {
            const GameBootstrap *bootstrap{registeredGame()};
            if (bootstrap == nullptr || bootstrap->createGame == nullptr) {
                SDL_Log("%s", registrationError(gameRegistrationStatus()));
                return SDL_APP_FAILURE;
            }
            m_config = bootstrap->config;

            if (m_config.name.empty() || m_config.version.empty() || m_config.identifier.empty() ||
                (m_config.sdlInitFlags & SDL_INIT_VIDEO) == 0 || m_config.windowWidth <= 0 ||
                m_config.windowHeight <= 0 || !std::isfinite(m_config.fixedUpdateRate) ||
                m_config.fixedUpdateRate <= 0.0) {
                SDL_Log("Invalid game configuration");
                return SDL_APP_FAILURE;
            }
            m_fixedDeltaSeconds = 1.0 / m_config.fixedUpdateRate;
            const float fixedDeltaSeconds{static_cast<float>(m_fixedDeltaSeconds)};
            if (!std::isfinite(m_fixedDeltaSeconds) || m_fixedDeltaSeconds <= 0.0 ||
                !std::isfinite(fixedDeltaSeconds) || fixedDeltaSeconds <= 0.0F) {
                SDL_Log("Invalid fixed update rate");
                return SDL_APP_FAILURE;
            }

            if (!SDL_SetAppMetadata(m_config.name.c_str(), m_config.version.c_str(), m_config.identifier.c_str())) {
                SDL_Log("SDL_SetAppMetadata warning: %s", SDL_GetError());
            }
            if (!SDL_Init(m_config.sdlInitFlags)) {
                SDL_Log("SDL_Init failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
            }
            constexpr SDL_WindowFlags flags{
                SDL_WINDOW_RESIZABLE |
                    SDL_WINDOW_HIGH_PIXEL_DENSITY |
                    SDL_WINDOW_HIDDEN,
            };
            m_window = SDL_CreateWindow(
                m_config.name.c_str(),
                m_config.windowWidth,
                m_config.windowHeight,
                flags);
            if (m_window == nullptr) {
                SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
            }

            if (!m_renderer.init(m_window)) {
                return SDL_APP_FAILURE;
            }

            m_game = bootstrap->createGame();
            if (m_game == nullptr || !m_game->init()) {
                SDL_Log("Game initialization failed");
                return SDL_APP_FAILURE;
            }

            if (!SDL_ShowWindow(m_window)) {
                SDL_Log("SDL_ShowWindow failed: %s", SDL_GetError());
                return SDL_APP_FAILURE;
            }

            m_previousTimeNs = SDL_GetTicksNS();
            return SDL_APP_CONTINUE;
        } catch (...) {
            return reportException();
        }
    }

    SDL_AppResult Application::handleEvent(const SDL_Event &event) noexcept
    {
        const std::scoped_lock lock{m_callbackMutex};
        if (m_shuttingDown) {
            return SDL_APP_SUCCESS;
        }
        if (m_game == nullptr) {
            return SDL_APP_FAILURE;
        }

        if (event.type == SDL_EVENT_WILL_ENTER_BACKGROUND || event.type == SDL_EVENT_DID_ENTER_BACKGROUND) {
            m_paused = true;
        } else if (event.type == SDL_EVENT_WILL_ENTER_FOREGROUND) {
            m_resetClock = true;
        } else if (event.type == SDL_EVENT_DID_ENTER_FOREGROUND) {
            m_resetClock = true;
            m_paused = false;
        }

        if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
            return SDL_APP_SUCCESS;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE) {
            return SDL_APP_SUCCESS;
        }

        try {
            m_game->handleEvent(event);
            return SDL_APP_CONTINUE;
        } catch (...) {
            return reportException();
        }
    }

    SDL_AppResult Application::iterate() noexcept
    {
        const std::scoped_lock lock{m_callbackMutex};
        if (m_shuttingDown) {
            return SDL_APP_SUCCESS;
        }
        if (m_game == nullptr) {
            return SDL_APP_FAILURE;
        }

        try {
            const Uint64 currentTimeNs{SDL_GetTicksNS()};
            if (m_resetClock || m_paused) {
                m_resetClock = false;
                m_previousTimeNs = currentTimeNs;
                m_accumulator = 0.0;
                if (m_paused) {
                    return SDL_APP_CONTINUE;
                }
            }

            constexpr double maximumFrameSeconds{0.25};
            const double frameSeconds{std::min(
                static_cast<double>(currentTimeNs - m_previousTimeNs) / static_cast<double>(SDL_NS_PER_SECOND),
                maximumFrameSeconds)};
            m_previousTimeNs = currentTimeNs;
            m_accumulator += frameSeconds;

            constexpr std::size_t maximumFixedUpdatesPerIteration{1'000};
            std::size_t fixedUpdates{};
            while (m_accumulator >= m_fixedDeltaSeconds && fixedUpdates < maximumFixedUpdatesPerIteration) {
                m_game->fixedUpdate(static_cast<float>(m_fixedDeltaSeconds));
                m_accumulator -= m_fixedDeltaSeconds;
                ++fixedUpdates;
            }
            if (m_accumulator >= m_fixedDeltaSeconds) {
                m_accumulator = std::fmod(m_accumulator, m_fixedDeltaSeconds);
            }

            return m_game->render(m_renderer) ? SDL_APP_CONTINUE : SDL_APP_FAILURE;
        } catch (...) {
            return reportException();
        }
    }

    void Application::shutdown() noexcept
    {
        const std::scoped_lock lock{m_callbackMutex};
        if (m_shuttingDown) {
            return;
        }
        m_shuttingDown = true;

        if (m_game != nullptr) {
            m_game->shutdown(m_renderer);
            m_game.reset();
        }
        m_renderer.shutdown();
        if (m_window != nullptr) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
    }
} // namespace tgc::core

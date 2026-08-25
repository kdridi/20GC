#pragma once

#include "tgc/core/GameConfig.h"
#include "tgc/core/IGame.h"
#include "tgc/gpu/GpuRenderer.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

#include <memory>
#include <mutex>

namespace tgc::core {
    class Application {
    public:
        Application();
        ~Application() noexcept;

        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        SDL_AppResult init() noexcept;
        SDL_AppResult handleEvent(const SDL_Event &event) noexcept;
        SDL_AppResult iterate() noexcept;
        void shutdown() noexcept;

    private:
        static SDL_AppResult reportException() noexcept;

        GameConfig m_config{};
        SDL_Window *m_window{};
        tgc::gpu::GpuRenderer m_renderer{};
        std::unique_ptr<IGame> m_game{};
        Uint64 m_previousTimeNs{};
        double m_fixedDeltaSeconds{};
        double m_accumulator{};
        std::mutex m_callbackMutex{};
        bool m_paused{};
        bool m_resetClock{};
        bool m_shuttingDown{};
    };
} // namespace tgc::core

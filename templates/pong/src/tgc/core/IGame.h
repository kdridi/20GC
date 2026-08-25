#pragma once

#include "tgc/gpu/GpuRenderer.h"

#include <SDL3/SDL_events.h>

namespace tgc::core {
    class IGame {
    public:
        virtual ~IGame();

        virtual bool init() = 0;
        virtual void shutdown(tgc::gpu::GpuRenderer &renderer) noexcept;
        virtual void handleEvent(const SDL_Event &event) = 0;
        virtual void fixedUpdate(float deltaSeconds) = 0;
        virtual bool render(tgc::gpu::GpuRenderer &renderer) const = 0;
    };
} // namespace tgc::core

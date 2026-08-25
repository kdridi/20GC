#pragma once

#include "core/CleanupStack.hpp"
#include "core/Window.hpp"
#include "systems/RenderSystem.hpp"

#include <entt/entt.hpp>

#include <memory>

namespace core {
    class Game : private CleanupStack {
    public:
        bool init() noexcept;

        int run() noexcept;

    private:
        bool initEntities() noexcept;

        void runEvent() noexcept;
        void runUpdate(double dt) noexcept;
        void runRender() const noexcept;

        Window m_window{};
        entt::registry m_registry{};
        bool m_running{};

        std::unique_ptr<systems::RenderSystem> m_render_system{};
    };
} // namespace core

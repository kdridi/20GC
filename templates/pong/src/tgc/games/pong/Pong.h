#pragma once

#include "tgc/core/IGame.h"
#include "tgc/games/pong/InputState.h"
#include "tgc/games/pong/components/Side.h"

#include <SDL3/SDL_gamepad.h>
#include <entt/entt.hpp>

#include <vector>

namespace tgc::games::pong {
    class Pong final : public tgc::core::IGame {
    public:
        ~Pong() noexcept override;

        bool init() override;
        void handleEvent(const SDL_Event &event) override;
        void fixedUpdate(float deltaSeconds) override;
        bool render(tgc::gpu::GpuRenderer &renderer) const override;

    private:
        void createArena();
        void createPaddle(tgc::games::pong::components::Side side);
        void resetBall(float horizontalDirection);
        void scorePoint(tgc::games::pong::components::Side side);
        void resetScore();
        void updateTouch(const SDL_TouchFingerEvent &event, bool pressed);
        float gamepadAxis(tgc::games::pong::components::Side side) const noexcept;
        void openGamepad(SDL_JoystickID joystickId);
        void closeGamepad(SDL_JoystickID joystickId) noexcept;

        entt::registry m_registry{};
        entt::entity m_ball{entt::null};
        InputState m_input{};
        std::vector<SDL_Gamepad *> m_gamepads{};
    };
} // namespace tgc::games::pong

#include "tgc/games/pong/Pong.h"

#include "tgc/components/Rectangle.h"
#include "tgc/components/Transform2D.h"
#include "tgc/components/Velocity2D.h"
#include "tgc/games/pong/Arena.h"
#include "tgc/games/pong/Score.h"
#include "tgc/games/pong/components/Ball.h"
#include "tgc/games/pong/components/Decoration.h"
#include "tgc/games/pong/components/Paddle.h"
#include "tgc/games/pong/components/ScoreMarker.h"
#include "tgc/games/pong/systems/CollisionSystem.h"
#include "tgc/gpu/GpuRenderer.h"
#include "tgc/systems/RenderSystem.h"

#include <SDL3/SDL.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

namespace tgc::games::pong {
    namespace {
        constexpr float paddleSpeed{7.5F};
        constexpr float ballSpeed{7.0F};
        constexpr glm::vec2 paddleSize{0.28F, 1.65F};
        constexpr glm::vec2 ballSize{0.32F, 0.32F};
        constexpr glm::vec4 foreground{0.9F, 0.93F, 1.0F, 1.0F};
        constexpr glm::vec4 leftColor{0.2F, 0.75F, 1.0F, 1.0F};
        constexpr glm::vec4 rightColor{1.0F, 0.35F, 0.55F, 1.0F};
    } // namespace

    Pong::~Pong() noexcept
    {
        for (SDL_Gamepad *gamepad : m_gamepads) {
            SDL_CloseGamepad(gamepad);
        }
    }

    bool Pong::init()
    {
        m_registry.ctx().emplace<Arena>();
        m_registry.ctx().emplace<Score>();

        createArena();
        createPaddle(components::Side::left);
        createPaddle(components::Side::right);

        m_ball = m_registry.create();
        m_registry.emplace<components::Ball>(m_ball);
        m_registry.emplace<tgc::components::Transform2D>(m_ball);
        m_registry.emplace<tgc::components::Rectangle>(m_ball, ballSize, foreground);
        m_registry.emplace<tgc::components::Velocity2D>(m_ball);
        resetBall(1.0F);
        return true;
    }

    void Pong::createArena()
    {
        const Arena &arena{m_registry.ctx().get<Arena>()};
        const float halfHeight{arena.height * 0.5F};

        const auto createRectangle = [&](glm::vec2 position, glm::vec2 size, glm::vec4 color) {
            const entt::entity entity{m_registry.create()};
            m_registry.emplace<components::Decoration>(entity);
            m_registry.emplace<tgc::components::Transform2D>(entity, position);
            m_registry.emplace<tgc::components::Rectangle>(entity, size, color);
        };

        createRectangle(
            {0.0F, halfHeight - arena.wallThickness * 0.5F},
            {arena.width, arena.wallThickness},
            foreground);
        createRectangle(
            {0.0F, -halfHeight + arena.wallThickness * 0.5F},
            {arena.width, arena.wallThickness},
            foreground);

        constexpr float dividerHeight{0.38F};
        constexpr float dividerSpacing{0.72F};
        for (float y{-3.6F}; y <= 3.6F; y += dividerSpacing) {
            createRectangle({0.0F, y}, {0.08F, dividerHeight}, {0.35F, 0.4F, 0.52F, 1.0F});
        }
    }

    void Pong::createPaddle(components::Side side)
    {
        const Arena &arena{m_registry.ctx().get<Arena>()};
        const float x{(arena.width * 0.5F - 0.65F) * (side == components::Side::left ? -1.0F : 1.0F)};

        const entt::entity paddle{m_registry.create()};
        m_registry.emplace<components::Paddle>(paddle, side);
        m_registry.emplace<tgc::components::Transform2D>(paddle, glm::vec2{x, 0.0F});
        m_registry.emplace<tgc::components::Rectangle>(
            paddle,
            paddleSize,
            side == components::Side::left ? leftColor : rightColor);
    }

    void Pong::resetBall(float horizontalDirection)
    {
        auto &transform{m_registry.get<tgc::components::Transform2D>(m_ball)};
        auto &velocity{m_registry.get<tgc::components::Velocity2D>(m_ball)};
        const Score &score{m_registry.ctx().get<Score>()};

        transform.position = {};
        const float verticalDirection{((score.left + score.right) % 2 == 0) ? 1.0F : -1.0F};
        velocity.linear = {horizontalDirection * ballSpeed, verticalDirection * 2.25F};
    }

    void Pong::scorePoint(components::Side side)
    {
        Score &score{m_registry.ctx().get<Score>()};
        int &value{side == components::Side::left ? score.left : score.right};
        ++value;

        const float direction{side == components::Side::left ? -1.0F : 1.0F};
        const float x{direction * (1.15F + static_cast<float>(value - 1) * 0.34F)};
        const entt::entity marker{m_registry.create()};
        m_registry.emplace<components::ScoreMarker>(marker);
        m_registry.emplace<tgc::components::Transform2D>(marker, glm::vec2{x, 3.85F});
        m_registry.emplace<tgc::components::Rectangle>(
            marker,
            glm::vec2{0.18F, 0.36F},
            side == components::Side::left ? leftColor : rightColor);

        SDL_Log("Score: %d - %d", score.left, score.right);
    }

    void Pong::resetScore()
    {
        m_registry.ctx().get<Score>() = {};

        std::vector<entt::entity> markers;
        for (const entt::entity entity : m_registry.view<components::ScoreMarker>()) {
            markers.push_back(entity);
        }
        for (const entt::entity entity : markers) {
            m_registry.destroy(entity);
        }
        resetBall(1.0F);
    }

    void Pong::updateTouch(const SDL_TouchFingerEvent &event, bool pressed)
    {
        const Arena &arena{m_registry.ctx().get<Arena>()};
        const float worldY{(0.5F - event.y) * arena.height};

        if (!pressed) {
            if (m_input.leftTouchActive && event.fingerID == m_input.leftFinger) {
                m_input.leftTouchActive = false;
            }
            if (m_input.rightTouchActive && event.fingerID == m_input.rightFinger) {
                m_input.rightTouchActive = false;
            }
            return;
        }

        if (m_input.leftTouchActive && event.fingerID == m_input.leftFinger) {
            m_input.leftTouchY = worldY;
            return;
        }
        if (m_input.rightTouchActive && event.fingerID == m_input.rightFinger) {
            m_input.rightTouchY = worldY;
            return;
        }

        if (event.x < 0.5F) {
            m_input.leftTouchActive = true;
            m_input.leftFinger = event.fingerID;
            m_input.leftTouchY = worldY;
        } else {
            m_input.rightTouchActive = true;
            m_input.rightFinger = event.fingerID;
            m_input.rightTouchY = worldY;
        }
    }

    float Pong::gamepadAxis(components::Side side) const noexcept
    {
        const std::size_t index{side == components::Side::left ? 0U : 1U};
        if (index >= m_gamepads.size()) {
            return 0.0F;
        }

        SDL_Gamepad *gamepad{m_gamepads[index]};
        float axis{-static_cast<float>(SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY)) / 32767.0F};
        axis += static_cast<float>(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP));
        axis -= static_cast<float>(SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN));
        return std::abs(axis) < 0.15F ? 0.0F : std::clamp(axis, -1.0F, 1.0F);
    }

    void Pong::openGamepad(SDL_JoystickID joystickId)
    {
        if (SDL_Gamepad *gamepad{SDL_OpenGamepad(joystickId)}; gamepad != nullptr) {
            m_gamepads.push_back(gamepad);
        } else {
            SDL_Log("SDL_OpenGamepad failed: %s", SDL_GetError());
        }
    }

    void Pong::closeGamepad(SDL_JoystickID joystickId) noexcept
    {
        const auto iterator{std::find_if(m_gamepads.begin(), m_gamepads.end(), [&](SDL_Gamepad *gamepad) {
            return SDL_GetGamepadID(gamepad) == joystickId;
        })};
        if (iterator != m_gamepads.end()) {
            SDL_CloseGamepad(*iterator);
            m_gamepads.erase(iterator);
        }
    }

    void Pong::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
            const bool pressed{event.type == SDL_EVENT_KEY_DOWN};
            switch (event.key.scancode) {
            case SDL_SCANCODE_W:
                m_input.leftUp = pressed;
                break;
            case SDL_SCANCODE_S:
                m_input.leftDown = pressed;
                break;
            case SDL_SCANCODE_UP:
                m_input.rightUp = pressed;
                break;
            case SDL_SCANCODE_DOWN:
                m_input.rightDown = pressed;
                break;
            case SDL_SCANCODE_R:
                if (pressed && !event.key.repeat) {
                    resetScore();
                }
                break;
            default:
                break;
            }
        } else if (event.type == SDL_EVENT_FINGER_DOWN || event.type == SDL_EVENT_FINGER_MOTION) {
            updateTouch(event.tfinger, true);
        } else if (event.type == SDL_EVENT_FINGER_UP || event.type == SDL_EVENT_FINGER_CANCELED) {
            updateTouch(event.tfinger, false);
        } else if (event.type == SDL_EVENT_GAMEPAD_ADDED) {
            openGamepad(event.gdevice.which);
        } else if (event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            closeGamepad(event.gdevice.which);
        } else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
            m_input.leftUp = false;
            m_input.leftDown = false;
            m_input.rightUp = false;
            m_input.rightDown = false;
        }
    }

    void Pong::fixedUpdate(float deltaSeconds)
    {
        const Arena &arena{m_registry.ctx().get<Arena>()};
        const float paddleLimit{arena.height * 0.5F - arena.wallThickness - paddleSize.y * 0.5F};

        m_registry.view<components::Paddle, tgc::components::Transform2D>().each(
            [&](const components::Paddle &paddle, tgc::components::Transform2D &transform) {
                const bool left{paddle.side == components::Side::left};
                const bool touchActive{left ? m_input.leftTouchActive : m_input.rightTouchActive};
                if (touchActive) {
                    transform.position.y = std::clamp(
                        left ? m_input.leftTouchY : m_input.rightTouchY,
                        -paddleLimit,
                        paddleLimit);
                    return;
                }

                const float keyboardAxis{
                    static_cast<float>(left ? m_input.leftUp : m_input.rightUp) -
                        static_cast<float>(left ? m_input.leftDown : m_input.rightDown),
                };
                const float axis{std::clamp(keyboardAxis + gamepadAxis(paddle.side), -1.0F, 1.0F)};
                transform.position.y = std::clamp(
                    transform.position.y + axis * paddleSpeed * deltaSeconds,
                    -paddleLimit,
                    paddleLimit);
            });

        auto &ballTransform{m_registry.get<tgc::components::Transform2D>(m_ball)};
        const auto &ballVelocity{m_registry.get<tgc::components::Velocity2D>(m_ball)};
        ballTransform.position += ballVelocity.linear * deltaSeconds;

        const Score &score{m_registry.ctx().get<Score>()};
        const float horizontalBallSpeed{
            ballSpeed + 0.3F * static_cast<float>(score.left + score.right),
        };
        const std::optional scorer{
            systems::CollisionSystem::update(m_registry, arena, horizontalBallSpeed),
        };
        if (scorer.has_value()) {
            scorePoint(*scorer);
            resetBall(*scorer == components::Side::left ? 1.0F : -1.0F);
        }
    }

    bool Pong::render(tgc::gpu::GpuRenderer &renderer) const
    {
        const Arena &arena{m_registry.ctx().get<Arena>()};
        return tgc::systems::RenderSystem::render(
            renderer,
            m_registry,
            {arena.width, arena.height},
            arena.clearColor);
    }
} // namespace tgc::games::pong

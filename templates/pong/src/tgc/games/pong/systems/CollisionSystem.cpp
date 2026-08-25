#include "tgc/games/pong/systems/CollisionSystem.h"

#include "tgc/components/Rectangle.h"
#include "tgc/components/Transform2D.h"
#include "tgc/components/Velocity2D.h"
#include "tgc/games/pong/components/Ball.h"
#include "tgc/games/pong/components/Paddle.h"

#include <algorithm>
#include <cmath>

namespace tgc::games::pong::systems {
    namespace {
        bool overlaps(
            const tgc::components::Transform2D &firstTransform,
            const tgc::components::Rectangle &firstRectangle,
            const tgc::components::Transform2D &secondTransform,
            const tgc::components::Rectangle &secondRectangle) noexcept
        {
            const glm::vec2 firstHalf{0.5F * firstRectangle.size * firstTransform.scale};
            const glm::vec2 secondHalf{0.5F * secondRectangle.size * secondTransform.scale};
            const glm::vec2 delta{firstTransform.position - secondTransform.position};
            return std::abs(delta.x) <= firstHalf.x + secondHalf.x &&
                   std::abs(delta.y) <= firstHalf.y + secondHalf.y;
        }
    } // namespace

    std::optional<tgc::games::pong::components::Side> CollisionSystem::update(
        entt::registry &registry,
        const tgc::games::pong::Arena &arena,
        float horizontalBallSpeed)
    {
        std::optional<tgc::games::pong::components::Side> scorer;

        registry.view<
                    tgc::games::pong::components::Ball,
                    tgc::components::Transform2D,
                    tgc::components::Velocity2D,
                    tgc::components::Rectangle>()
            .each([&](
                      tgc::components::Transform2D &ballTransform,
                      tgc::components::Velocity2D &ballVelocity,
                      const tgc::components::Rectangle &ballRectangle) {
                const float ballHalfHeight{ballRectangle.size.y * ballTransform.scale.y * 0.5F};
                const float verticalLimit{arena.height * 0.5F - arena.wallThickness - ballHalfHeight};
                if (ballTransform.position.y > verticalLimit) {
                    ballTransform.position.y = verticalLimit;
                    ballVelocity.linear.y = -std::abs(ballVelocity.linear.y);
                } else if (ballTransform.position.y < -verticalLimit) {
                    ballTransform.position.y = -verticalLimit;
                    ballVelocity.linear.y = std::abs(ballVelocity.linear.y);
                }

                registry.view<
                            tgc::games::pong::components::Paddle,
                            tgc::components::Transform2D,
                            tgc::components::Rectangle>()
                    .each([&](
                              const tgc::games::pong::components::Paddle &paddle,
                              const tgc::components::Transform2D &paddleTransform,
                              const tgc::components::Rectangle &paddleRectangle) {
                        const bool movingTowardPaddle{
                            paddle.side == tgc::games::pong::components::Side::left
                                ? ballVelocity.linear.x < 0.0F
                                : ballVelocity.linear.x > 0.0F,
                        };
                        if (!movingTowardPaddle || !overlaps(ballTransform, ballRectangle, paddleTransform, paddleRectangle)) {
                            return;
                        }

                        const float direction{
                            paddle.side == tgc::games::pong::components::Side::left ? 1.0F : -1.0F,
                        };
                        const float paddleHalfWidth{paddleRectangle.size.x * paddleTransform.scale.x * 0.5F};
                        const float ballHalfWidth{ballRectangle.size.x * ballTransform.scale.x * 0.5F};
                        ballTransform.position.x = paddleTransform.position.x + direction * (paddleHalfWidth + ballHalfWidth);

                        const float paddleHalfHeight{paddleRectangle.size.y * paddleTransform.scale.y * 0.5F};
                        const float hitPosition{std::clamp(
                            (ballTransform.position.y - paddleTransform.position.y) / paddleHalfHeight,
                            -1.0F,
                            1.0F)};
                        ballVelocity.linear.x = direction * horizontalBallSpeed;
                        ballVelocity.linear.y = hitPosition * 5.5F;
                    });

                const float horizontalLimit{
                    arena.width * 0.5F + ballRectangle.size.x * ballTransform.scale.x * 0.5F,
                };
                if (ballTransform.position.x < -horizontalLimit) {
                    scorer = tgc::games::pong::components::Side::right;
                } else if (ballTransform.position.x > horizontalLimit) {
                    scorer = tgc::games::pong::components::Side::left;
                }
            });

        return scorer;
    }
} // namespace tgc::games::pong::systems

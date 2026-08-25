#pragma once

#include "tgc/games/pong/Arena.h"
#include "tgc/games/pong/components/Side.h"

#include <entt/entity/registry.hpp>

#include <optional>

namespace tgc::games::pong::systems {
    class CollisionSystem {
    public:
        static std::optional<tgc::games::pong::components::Side> update(
            entt::registry &registry,
            const tgc::games::pong::Arena &arena,
            float horizontalBallSpeed);
    };
} // namespace tgc::games::pong::systems

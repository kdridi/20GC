#pragma once

#include "tgc/core/GameConfig.h"
#include "tgc/core/GameRegistrationStatus.h"
#include "tgc/core/IGame.h"

#include <memory>

namespace tgc::core {
    struct GameBootstrap {
        GameConfig config{};
        std::unique_ptr<IGame> (*createGame)(){};
    };

    bool registerGame(const GameBootstrap &bootstrap) noexcept;
    const GameBootstrap *registeredGame() noexcept;
    GameRegistrationStatus gameRegistrationStatus() noexcept;
} // namespace tgc::core

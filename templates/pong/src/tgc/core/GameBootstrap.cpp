#include "tgc/core/GameBootstrap.h"

#include <optional>

namespace tgc::core {
    namespace {
        std::optional<GameBootstrap> &bootstrapStorage() noexcept
        {
            static std::optional<GameBootstrap> bootstrap;
            return bootstrap;
        }

        GameRegistrationStatus &registrationStatusStorage() noexcept
        {
            static GameRegistrationStatus status{GameRegistrationStatus::missing};
            return status;
        }
    } // namespace

    bool registerGame(const GameBootstrap &bootstrap) noexcept
    {
        auto &status{registrationStatusStorage()};
        if (status != GameRegistrationStatus::missing) {
            if (status == GameRegistrationStatus::registered) {
                status = GameRegistrationStatus::duplicate;
            }
            return false;
        }
        if (bootstrap.createGame == nullptr) {
            status = GameRegistrationStatus::invalidFactory;
            return false;
        }

        try {
            bootstrapStorage() = bootstrap;
            status = GameRegistrationStatus::registered;
            return true;
        } catch (...) {
            status = GameRegistrationStatus::storageFailure;
            return false;
        }
    }

    const GameBootstrap *registeredGame() noexcept
    {
        const auto &registered{bootstrapStorage()};
        return registered && registrationStatusStorage() == GameRegistrationStatus::registered ? &*registered : nullptr;
    }

    GameRegistrationStatus gameRegistrationStatus() noexcept
    {
        return registrationStatusStorage();
    }
} // namespace tgc::core

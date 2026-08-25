#pragma once

namespace tgc::core {
    enum class GameRegistrationStatus {
        missing,
        registered,
        duplicate,
        invalidFactory,
        storageFailure,
    };
} // namespace tgc::core

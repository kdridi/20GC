#include "tgc/core/IGame.h"

namespace tgc::core {
    IGame::~IGame() = default;

    void IGame::shutdown(tgc::gpu::GpuRenderer &) noexcept
    {
    }
} // namespace tgc::core

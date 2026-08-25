#pragma once

#include <SDL3/SDL_touch.h>

namespace tgc::games::pong {
    struct InputState {
        bool leftUp{};
        bool leftDown{};
        bool rightUp{};
        bool rightDown{};
        bool leftTouchActive{};
        bool rightTouchActive{};
        SDL_FingerID leftFinger{};
        SDL_FingerID rightFinger{};
        float leftTouchY{};
        float rightTouchY{};
    };
} // namespace tgc::games::pong

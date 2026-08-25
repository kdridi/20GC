#pragma once

#include <SDL3/SDL_init.h>

#include <string>

namespace tgc::core {
    struct GameConfig {
        std::string name{};
        std::string version{};
        std::string identifier{};
        SDL_InitFlags sdlInitFlags{SDL_INIT_VIDEO};
        int windowWidth{1280};
        int windowHeight{720};
        double fixedUpdateRate{60.0};
    };
} // namespace tgc::core

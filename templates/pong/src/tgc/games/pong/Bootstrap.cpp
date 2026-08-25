#include "tgc/core/GameBootstrap.h"

#include "tgc/core/IGame.h"
#include "tgc/games/pong/Pong.h"

#include <memory>

namespace tgc::games::pong {
    namespace {
        std::unique_ptr<tgc::core::IGame> createPong()
        {
            return std::make_unique<Pong>();
        }

        [[maybe_unused]] const bool registered{
            tgc::core::registerGame({
                .config = {
                    .name = "20GC Pong GPU",
                    .version = "1.0",
                    .identifier = "com.kdridi.20gc.pong.gpu",
                    .sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_GAMEPAD,
                    .windowWidth = 1280,
                    .windowHeight = 720,
                    .fixedUpdateRate = 120.0,
                },
                .createGame = createPong,
            }),
        };
    } // namespace
} // namespace tgc::games::pong

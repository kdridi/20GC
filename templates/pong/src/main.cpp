#define SDL_MAIN_USE_CALLBACKS 1

#include "tgc/core/Application.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <new>

SDL_AppResult SDL_AppInit(void **appstate, int, char **)
{
    if (appstate == nullptr) {
        return SDL_APP_FAILURE;
    }

    auto *application{new (std::nothrow) tgc::core::Application{}};
    if (application == nullptr) {
        SDL_Log("Could not allocate application");
        *appstate = nullptr;
        return SDL_APP_FAILURE;
    }

    *appstate = application;
    return application->init();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (appstate == nullptr || event == nullptr) {
        return SDL_APP_FAILURE;
    }
    return static_cast<tgc::core::Application *>(appstate)->handleEvent(*event);
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (appstate == nullptr) {
        return SDL_APP_FAILURE;
    }
    return static_cast<tgc::core::Application *>(appstate)->iterate();
}

void SDL_AppQuit(void *appstate, SDL_AppResult)
{
    delete static_cast<tgc::core::Application *>(appstate);
}

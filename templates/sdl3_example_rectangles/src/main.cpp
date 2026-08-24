#include "SDL3/SDL_stdinc.h"
#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window{};
static SDL_Renderer *renderer{};

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
    SDL_SetAppMetadata("Example Renderer Rectangles", "1.0", "com.kdridi.20gc.sdl3.examples.rectangles");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Renderer Rectangles", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_KEY_DOWN && event->key.repeat == false) {
        if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_FRect rects[16];
    const Uint64 now{SDL_GetTicks()};
    int i{};

    const float direction{((now % 2000) >= 1000) ? 1.0f : -1.0f};
    const float scale{((float)(((int)(now % 1000)) - 500) / 500.0f) * direction};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    /* Let's draw a single rectangle (square, really). */
    rects[0].x = rects[0].y = 100;
    rects[0].w = rects[0].h = 100 + (100 * scale);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderRect(renderer, &rects[0]);

    /* Now let's draw several rectangles with one function call. */
    for (i = 0; i < 3; ++i) {
        const float size{(i + 1) * 50.0f};
        rects[i].w = rects[i].h = size + (size * scale);
        rects[i].x = (WINDOW_WIDTH - rects[i].w) / 2;
        rects[i].y = (WINDOW_HEIGHT - rects[i].h) / 2;
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderRects(renderer, rects, 3);

    /* those were rectangle _outlines_, really. You can also draw _filled_ rectangles! */
    rects[0].x = 400;
    rects[0].y = 50;
    rects[0].w = 100 + (100 * scale);
    rects[0].h = 50 + (50 * scale);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rects[0]);

    /* ...and also fill a bunch of rectangles at once... */
    for (i = 0; i < SDL_arraysize(rects); ++i) {
        const float w{((float)WINDOW_WIDTH / SDL_arraysize(rects))};
        const float h = i * 8.0f;
        rects[i].x = i * w;
        rects[i].y = WINDOW_HEIGHT - h;
        rects[i].w = w;
        rects[i].h = h;
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(renderer, rects, SDL_arraysize(rects));

    /* put it all on the screen! */
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

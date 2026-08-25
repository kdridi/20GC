#pragma once

// Active le point d'entrée par callbacks avant toute inclusion de SDL.
#ifndef SDL_MAIN_USE_CALLBACKS
#define SDL_MAIN_USE_CALLBACKS 1
#endif

#include "CleanupStack.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <cstdarg>

namespace SDL3 {

    using AppResult = SDL_AppResult;
    using Event = SDL_Event;
    using FRect = SDL_FRect;
    using InitFlags = SDL_InitFlags;
    using Renderer = SDL_Renderer;
    using Ticks = Uint64;
    using Window = SDL_Window;
    using WindowFlags = SDL_WindowFlags;

    inline constexpr AppResult Continue{SDL_APP_CONTINUE};
    inline constexpr AppResult Success{SDL_APP_SUCCESS};
    inline constexpr AppResult Failure{SDL_APP_FAILURE};

    inline constexpr InitFlags InitVideo{SDL_INIT_VIDEO};
    inline constexpr InitFlags InitGamepad{SDL_INIT_GAMEPAD};

    inline constexpr WindowFlags WindowResizable{SDL_WINDOW_RESIZABLE};
    inline constexpr WindowFlags WindowHighPixelDensity{SDL_WINDOW_HIGH_PIXEL_DENSITY};
    inline constexpr WindowFlags WindowHidden{SDL_WINDOW_HIDDEN};

    inline constexpr Ticks NanosecondsPerSecond{SDL_NS_PER_SECOND};
    inline constexpr Uint8 AlphaOpaque{SDL_ALPHA_OPAQUE};

    inline void Log(const char *format, ...) noexcept
    {
        va_list arguments;
        va_start(arguments, format);
        SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, format, arguments);
        va_end(arguments);
    }

    inline void LogError(const char *message) noexcept
    {
        SDL_Log("%s : %s", message, SDL_GetError());
    }

    inline bool SetAppMetadata(const char *name, const char *version, const char *identifier)
    {
        if (!SDL_SetAppMetadata(name, version, identifier)) {
            LogError("Impossible de definir les metadonnees SDL");
            return false;
        }

        return true;
    }

    inline bool Init(CleanupStack &cleanup_stack, InitFlags flags)
    {
        if (!SDL_Init(flags)) {
            LogError("Impossible d'initialiser SDL");
            return false;
        }

        cleanup_stack.defer([]() noexcept -> void { SDL_Quit(); });
        return true;
    }

    inline bool CreateWindowAndRenderer(
        CleanupStack &cleanup_stack,
        const char *title,
        int width,
        int height,
        WindowFlags window_flags,
        Window *&window,
        Renderer *&renderer)
    {
        window = nullptr;
        renderer = nullptr;
        if (!SDL_CreateWindowAndRenderer(title, width, height, window_flags, &window, &renderer)) {
            LogError("Impossible de creer la fenetre ou le renderer");
            return false;
        }

        cleanup_stack.defer([window]() noexcept -> void { SDL_DestroyWindow(window); });
        cleanup_stack.defer([renderer]() noexcept -> void { SDL_DestroyRenderer(renderer); });
        return true;
    }

    inline bool SetRenderVSync(Renderer *renderer, int vsync)
    {
        if (!SDL_SetRenderVSync(renderer, vsync)) {
            LogError("Impossible de configurer la VSync");
            return false;
        }

        return true;
    }

    inline float GetWindowDisplayScale(Window *window)
    {
        const float scale{SDL_GetWindowDisplayScale(window)};
        if (scale <= 0.0f) {
            LogError("Impossible d'obtenir l'echelle de la fenetre");
            return 1.0f;
        }

        return scale;
    }

    inline bool CenterWindow(Window *window)
    {
        if (!SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
            LogError("Impossible de centrer la fenetre");
            return false;
        }

        return true;
    }

    inline bool ShowWindow(Window *window)
    {
        if (!SDL_ShowWindow(window)) {
            LogError("Impossible d'afficher la fenetre");
            return false;
        }

        return true;
    }

    inline bool IsWindowMinimized(Window *window) noexcept
    {
        return (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) != 0;
    }

    inline bool IsQuitEvent(const Event &event) noexcept
    {
        return event.type == SDL_EVENT_QUIT;
    }

    inline bool IsWindowCloseRequested(const Event &event, Window *window) noexcept
    {
        return event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window);
    }

    inline bool IsWindowDisplayScaleChanged(const Event &event, Window *window) noexcept
    {
        return event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED && event.window.windowID == SDL_GetWindowID(window);
    }

    inline bool IsEscapePressed(const Event &event) noexcept
    {
        return event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE;
    }

    inline Ticks GetTicks() noexcept
    {
        return SDL_GetTicks();
    }

    inline Ticks GetTicksNS() noexcept
    {
        return SDL_GetTicksNS();
    }

    inline void Delay(Ticks milliseconds) noexcept
    {
        SDL_Delay(static_cast<Uint32>(milliseconds));
    }

    inline void DelayPrecise(Ticks nanoseconds) noexcept
    {
        SDL_DelayPrecise(nanoseconds);
    }

    inline bool SetRenderScale(Renderer *renderer, float scale_x, float scale_y)
    {
        if (!SDL_SetRenderScale(renderer, scale_x, scale_y)) {
            LogError("Impossible de definir l'echelle du renderer");
            return false;
        }

        return true;
    }

    inline bool SetRenderDrawColor(Renderer *renderer, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
    {
        if (!SDL_SetRenderDrawColor(renderer, red, green, blue, alpha)) {
            LogError("Impossible de definir la couleur du renderer");
            return false;
        }

        return true;
    }

    inline bool SetRenderDrawColor(Renderer *renderer, float red, float green, float blue, float alpha)
    {
        if (!SDL_SetRenderDrawColorFloat(renderer, red, green, blue, alpha)) {
            LogError("Impossible de definir la couleur du renderer");
            return false;
        }

        return true;
    }

    inline bool RenderClear(Renderer *renderer)
    {
        if (!SDL_RenderClear(renderer)) {
            LogError("Impossible d'effacer le renderer");
            return false;
        }

        return true;
    }

    inline bool RenderRects(Renderer *renderer, const FRect *rectangles, int count)
    {
        if (!SDL_RenderRects(renderer, rectangles, count)) {
            LogError("Impossible de dessiner les rectangles");
            return false;
        }

        return true;
    }

    inline bool RenderPresent(Renderer *renderer)
    {
        if (!SDL_RenderPresent(renderer)) {
            LogError("Impossible de presenter la frame");
            return false;
        }

        return true;
    }

} // namespace SDL3

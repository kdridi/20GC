#include "Pong.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <array>
#include <cmath>

namespace {
    constexpr int MIN_ANIMATION_PERIOD_MS{100};
    constexpr int MAX_ANIMATION_PERIOD_MS{4000};
} // namespace

void Pong::renderImGui(float display_scale, bool vsync_enabled)
{
    ImGui::SetNextWindowSize(ImVec2{420.0f, 0.0f}, ImGuiCond_FirstUseEver);
    ImGui::Begin(GAME_NAME);

    ImGui::Text("20GC : %s", GAME_NAME);
    ImGui::Separator();

    ImGui::SliderInt("Periode de l'animation (ms)", &animation_period_ms, MIN_ANIMATION_PERIOD_MS, MAX_ANIMATION_PERIOD_MS);
    ImGui::ColorEdit3("Couleur de fond", &clear_color.x);

    ImGui::Separator();
    ImGui::TextUnformatted("Cette fenetre est reconstruite a chaque frame.");

    // Framerate vaut parfois zéro pendant la toute première frame.
    const float framerate{ImGui::GetIO().Framerate};
    const float frame_time_ms{framerate > 0.0f ? 1000.0f / framerate : 0.0f};
    ImGui::Text("Temps moyen : %.3f ms/frame", frame_time_ms);
    ImGui::Text("Frequence : %.1f FPS", framerate);
    ImGui::Text("Echelle DPI : %.2fx", display_scale);
    ImGui::TextUnformatted(vsync_enabled ? "Cadencement : VSync" : "Cadencement : limiteur logiciel 60 FPS");

    ImGui::End();
}

SDL_AppResult Pong::renderSDL(SDL_Renderer *renderer, const ImGuiIO &io)
{
    // Effacement unique de la frame. La scène SDL est dessinée en premier,
    // puis ImGui par-dessus, avant la présentation finale.
    if (!SDL_SetRenderDrawColorFloat(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w)) {
        SDL_Log("Impossible de definir la couleur du renderer : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_RenderClear(renderer)) {
        SDL_Log("Impossible d'effacer le renderer : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    std::array<SDL_FRect, 4> rectangles{};

    // Le slider ne peut pas descendre sous MIN_ANIMATION_PERIOD_MS, mais le
    // clamp protège également ce calcul si la valeur est modifiée autrement.
    const int period_ms{std::max(animation_period_ms, 1)};
    const Uint64 now{SDL_GetTicks()};
    const float phase{static_cast<float>(now % static_cast<Uint64>(period_ms)) / static_cast<float>(period_ms)};

    // Onde triangulaire comprise entre -1 et +1. La taille des rectangles
    // évolue ainsi régulièrement de 0 % à 200 % puis revient à 0 %.
    const float animation_scale{1.0f - 4.0f * std::abs(phase - 0.5f)};

    for (std::size_t index{}; index < rectangles.size(); ++index) {
        const float base_size{static_cast<float>(index + 1) * 50.0f};
        SDL_FRect &rectangle{rectangles[index]};

        rectangle.w = base_size * (1.0f + animation_scale);
        rectangle.h = rectangle.w;
        rectangle.x = (io.DisplaySize.x - rectangle.w) / 2.0f;
        rectangle.y = (io.DisplaySize.y - rectangle.h) / 2.0f;
    }

    if (!SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE)) {
        SDL_Log("Impossible de definir la couleur des rectangles : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_RenderRects(renderer, rectangles.data(), static_cast<int>(rectangles.size()))) {
        SDL_Log("Impossible de dessiner les rectangles : %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

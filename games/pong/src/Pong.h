#pragma once

#include <SDL3/SDL_init.h>

#include <imgui.h>

struct SDL_Renderer;

class Pong {
public:
    // Construit l'interface utilisateur de la frame courante.
    void renderImGui(float display_scale, bool vsync_enabled);

    // Efface le framebuffer puis dessine la scène SDL derrière ImGui.
    SDL_AppResult renderSDL(SDL_Renderer *renderer, const ImGuiIO &io);

private:
    int animation_period_ms{2000};
    ImVec4 clear_color{0.08f, 0.08f, 0.10f, 1.0f};
};

// SDL3.hpp demande à SDL de fournir le point d'entrée de l'application et
// d'appeler SDL_AppInit/SDL_AppEvent/SDL_AppIterate/SDL_AppQuit.
#include "ApplicationGraphics.hpp"
#include "CleanupStack.hpp"
#include "SDL3.hpp"

// API métier de Dear ImGui : contexte, fenêtres, widgets et draw data.
#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>

namespace {

    constexpr int MIN_ANIMATION_PERIOD_MS{100};
    constexpr int MAX_ANIMATION_PERIOD_MS{4000};

    struct AppState {
        // État de démonstration conservé d'une frame à l'autre. En immediate mode,
        // ImGui reconstruit les widgets à chaque frame, mais les valeurs manipulées
        // par ces widgets restent la propriété de l'application.
        bool show_demo_window{true};
        int animation_period_ms{2000};
        ImVec4 clear_color{0.08f, 0.08f, 0.10f, 1.0f};

        // Construit l'interface utilisateur. Cette fonction ne dessine encore aucun
        // pixel : les appels ImGui::XXX décrivent les widgets de la frame courante.
        void renderImGui(bool vsync_enabled)
        {
            ImGui::SetNextWindowSize(ImVec2{420.0f, 0.0f}, ImGuiCond_FirstUseEver);
            ImGui::Begin("20GC - SDL3 + Dear ImGui");

            ImGui::Text("Template : %s", TEMPLATE_NAME);
            ImGui::Separator();

            ImGui::SliderInt("Periode de l'animation (ms)", &animation_period_ms, MIN_ANIMATION_PERIOD_MS, MAX_ANIMATION_PERIOD_MS);
            ImGui::ColorEdit3("Couleur de fond", &clear_color.x);
            ImGui::Checkbox("Afficher la demo ImGui", &show_demo_window);

            ImGui::Separator();
            ImGui::TextUnformatted("Cette fenetre est reconstruite a chaque frame.");

            // Framerate vaut parfois zéro pendant la toute première frame.
            const float framerate{ImGui::GetIO().Framerate};
            const float frame_time_ms{framerate > 0.0f ? 1000.0f / framerate : 0.0f};
            ImGui::Text("Temps moyen : %.3f ms/frame", frame_time_ms);
            ImGui::Text("Frequence : %.1f FPS", framerate);
            ImGui::TextUnformatted(vsync_enabled ? "Cadencement : VSync" : "Cadencement : limiteur logiciel 60 FPS");

            ImGui::End();

            // La fenêtre de démonstration officielle est une excellente référence pour
            // découvrir les widgets et consulter leur code d'utilisation.
            if (show_demo_window) {
                ImGui::ShowDemoWindow(&show_demo_window);
            }
        }

        // Dessine une petite scène SDL derrière l'interface ImGui. La valeur de
        // retour permet de propager proprement une éventuelle erreur SDL.
        bool renderSDL(SDL3::Renderer *renderer, const SDL3::RenderSize &render_size)
        {
            // Effacement unique de la frame. La scène SDL est dessinée en premier,
            // puis ImGui par-dessus, avant la présentation finale.
            if (!SDL3::SetRenderDrawColor(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w)) {
                return false;
            }
            if (!SDL3::RenderClear(renderer)) {
                return false;
            }

            std::array<SDL3::FRect, 4> rectangles{};

            // Le slider ne peut pas descendre sous MIN_ANIMATION_PERIOD_MS, mais le
            // clamp protège également ce calcul si la valeur est modifiée autrement.
            const int period_ms{std::max(animation_period_ms, 1)};
            const SDL3::Ticks now{SDL3::GetTicks()};
            const float phase{static_cast<float>(now % static_cast<SDL3::Ticks>(period_ms)) / static_cast<float>(period_ms)};

            // Onde triangulaire comprise entre -1 et +1. La taille des rectangles
            // évolue ainsi régulièrement de 0 % à 200 % puis revient à 0 %.
            const float animation_scale{1.0f - 4.0f * std::abs(phase - 0.5f)};

            for (std::size_t index{}; index < rectangles.size(); ++index) {
                const float base_size{static_cast<float>(index + 1) * 50.0f};
                SDL3::FRect &rectangle{rectangles[index]};

                rectangle.w = base_size * (1.0f + animation_scale);
                rectangle.h = rectangle.w;
                rectangle.x = (render_size.width - rectangle.w) / 2.0f;
                rectangle.y = (render_size.height - rectangle.h) / 2.0f;
            }

            if (!SDL3::SetRenderDrawColor(renderer, 0, 255, 0, SDL3::AlphaOpaque)) {
                return false;
            }

            if (!SDL3::RenderRects(renderer, rectangles.data(), static_cast<int>(rectangles.size()))) {
                return false;
            }

            return true;
        }
    };

    // Toutes les données persistantes de l'application sont regroupées ici.
    // SDL transmet le pointeur placé dans « appstate » à chacun de ses callbacks.
    // Cette approche évite les variables globales et facilite l'ajout de nouveaux
    // états à l'exemple.
    struct AppContext {
        // Déclarée en premier afin d'être détruite en dernier : les ressources
        // externes restent disponibles pendant la destruction des autres membres.
        CleanupStack cleanup_stack{};

        ApplicationGraphics::Runtime graphics{};
        AppState state{};

        // Appelé une seule fois par SDL au démarrage.
        SDL3::AppResult initialize(int, char **)
        {
            // Ce template utilise systématiquement SDL_Renderer et Dear ImGui : leur
            // initialisation forme une unique transaction graphique obligatoire.
            return ApplicationGraphics::Initialize(cleanup_stack, graphics) ? SDL3::Continue : SDL3::Failure;
        }

        // Appelé par SDL pour chaque événement disponible. L'adaptateur transmet
        // toujours l'événement à ImGui et gère Quit, fermeture de fenêtre et Échap.
        SDL3::AppResult dispatchEvent(SDL3::Event &event)
        {
            return ApplicationGraphics::DispatchEvent(graphics, event);
        }

        // Appelé continuellement par SDL : l'adaptateur cadence la frame et dessine
        // la scène avant le draw data ImGui.
        SDL3::AppResult update()
        {
            return ApplicationGraphics::RenderFrame(
                graphics,
                [this]() -> void { state.renderImGui(graphics.vsync_enabled); },
                [this](SDL3::Renderer *renderer, const SDL3::RenderSize &render_size) -> bool {
                    return state.renderSDL(renderer, render_size);
                });
        }
    };

} // namespace

// Appelé une seule fois par SDL au démarrage.
SDL3::AppResult SDL_AppInit(void **_contextPtr, int argc, char **argv)
{
    if (_contextPtr == nullptr)
        return SDL3::Failure;
    *_contextPtr = nullptr;

    // Aucune exception C++ ne doit traverser la frontière des callbacks SDL.
    // En cas d'échec pendant l'initialisation, la destruction du contexte exécute
    // les actions qui avaient déjà été enregistrées.
    AppContext *context{};
    try {
        context = new AppContext{};
        *_contextPtr = context;
        return context->initialize(argc, argv);
    } catch (const std::exception &exception) {
        SDL3::Log("Exception pendant l'initialisation : %s", exception.what());
    } catch (...) {
        SDL3::Log("Exception inconnue pendant l'initialisation");
    }

    delete context;
    *_contextPtr = nullptr;
    return SDL3::Failure;
}

// Appelé par SDL pour chaque événement disponible.
SDL3::AppResult SDL_AppEvent(void *_context, SDL3::Event *_event)
{
    if (_context == nullptr)
        return SDL3::Failure;
    AppContext &context{*static_cast<AppContext *>(_context)};

    if (_event == nullptr)
        return SDL3::Failure;
    SDL3::Event &event = *_event;

    return context.dispatchEvent(event);
}

// Appelé continuellement par SDL : une invocation correspond à une frame.
SDL3::AppResult SDL_AppIterate(void *_context)
{
    if (_context == nullptr)
        return SDL3::Failure;
    AppContext &context{*static_cast<AppContext *>(_context)};

    return context.update();
}

// Appelé une seule fois à l'arrêt, y compris si SDL_AppInit() a échoué.
void SDL_AppQuit(void *_context, SDL3::AppResult)
{
    if (_context == nullptr)
        return;
    AppContext *context{static_cast<AppContext *>(_context)};

    delete context;
}

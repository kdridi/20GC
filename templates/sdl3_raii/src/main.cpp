// SDL3.hpp demande à SDL de fournir le point d'entrée de l'application et
// d'appeler SDL_AppInit/SDL_AppEvent/SDL_AppIterate/SDL_AppQuit.
#include "CleanupStack.hpp"
#include "SDL3.hpp"

// API principale de Dear ImGui : contexte, fenêtres, widgets et draw data.
#include <imgui.h>

// Backend « plateforme » SDL3 : événements, clavier, souris, gamepad,
// presse-papiers, curseurs et mesure du temps.
#include <imgui_impl_sdl3.h>

// Backend « rendu » SDL_Renderer3 : transforme le draw data produit par ImGui
// en appels de dessin exécutés par SDL_Renderer.
#include <imgui_impl_sdlrenderer3.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>

namespace {

    constexpr int WINDOW_WIDTH{960};
    constexpr int WINDOW_HEIGHT{640};
    constexpr int MIN_ANIMATION_PERIOD_MS{100};
    constexpr int MAX_ANIMATION_PERIOD_MS{4000};
    constexpr SDL3::Ticks FALLBACK_FRAME_DURATION_NS{SDL3::NanosecondsPerSecond / 60};

    struct ImGuiState {
        // ScaleAllSizes() multiplie le style courant et n'est donc pas
        // idempotent. Cette copie non mise à l'échelle sert de référence lors
        // de chaque changement d'écran ou de facteur DPI.
        ImGuiStyle base_style{};
        float display_scale{1.0f};

        // Conserve le style sombre non mis à l'échelle comme source de vérité,
        // puis applique le DPI de l'écran contenant actuellement la fenêtre.
        void initWindowDisplayScale(SDL3::Window *window)
        {
            base_style = ImGui::GetStyle();
            updateWindowDisplayScale(window);
        }

        // Réapplique toujours le scale depuis le style de référence. Sans cette
        // restauration, deux appels à ScaleAllSizes(2) produiraient un facteur 4.
        void updateWindowDisplayScale(SDL3::Window *window)
        {
            // L'adaptateur fournit une valeur de repli à 1 si SDL ne parvient pas
            // à déterminer l'échelle de la fenêtre.
            display_scale = SDL3::GetWindowDisplayScale(window);

            ImGuiStyle &style{ImGui::GetStyle()};
            style = base_style;
            style.ScaleAllSizes(display_scale);
            style.FontScaleDpi = display_scale;
        }
    };

    struct AppState {
        // État de démonstration conservé d'une frame à l'autre. En immediate mode,
        // ImGui reconstruit les widgets à chaque frame, mais les valeurs manipulées
        // par ces widgets restent la propriété de l'application.
        bool show_demo_window{true};
        int animation_period_ms{2000};
        ImVec4 clear_color{0.08f, 0.08f, 0.10f, 1.0f};

        // Construit l'interface utilisateur. Cette fonction ne dessine encore aucun
        // pixel : les appels ImGui::XXX décrivent les widgets de la frame courante.
        void renderImGui(float display_scale, bool vsync_enabled)
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
            ImGui::Text("Echelle DPI : %.2fx", display_scale);
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
        SDL3::AppResult renderSDL(SDL3::Renderer *renderer, const ImGuiIO &io)
        {
            // Effacement unique de la frame. La scène SDL est dessinée en premier,
            // puis ImGui par-dessus, avant la présentation finale.
            if (!SDL3::SetRenderDrawColor(renderer, clear_color.x, clear_color.y, clear_color.z, clear_color.w)) {
                return SDL3::Failure;
            }
            if (!SDL3::RenderClear(renderer)) {
                return SDL3::Failure;
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
                rectangle.x = (io.DisplaySize.x - rectangle.w) / 2.0f;
                rectangle.y = (io.DisplaySize.y - rectangle.h) / 2.0f;
            }

            if (!SDL3::SetRenderDrawColor(renderer, 0, 255, 0, SDL3::AlphaOpaque)) {
                return SDL3::Failure;
            }

            if (!SDL3::RenderRects(renderer, rectangles.data(), static_cast<int>(rectangles.size()))) {
                return SDL3::Failure;
            }

            return SDL3::Continue;
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

        SDL3::Window *window{};
        SDL3::Renderer *renderer{};

        // Indique si le renderer a accepté la synchronisation verticale.
        bool vsync_enabled{};

        ImGuiState imGuiState{};
        AppState state{};

        // Appelé une seule fois par SDL au démarrage.
        SDL3::AppResult initialize(int, char **)
        {

            // Ces métadonnées sont utilisées par SDL et macOS pour identifier
            // l'application. Un identifiant de type reverse-DNS doit rester stable.
            SDL3::SetAppMetadata("Example ImGui", "1.0", "com.kdridi.20gc.sdl3.examples.imgui");

            // GAMEPAD est activé parce que la navigation au gamepad est activée plus
            // bas dans ImGuiIO::ConfigFlags.
            if (!SDL3::Init(cleanup_stack, SDL3::InitVideo | SDL3::InitGamepad))
                return SDL3::Failure;

            // HIGH_PIXEL_DENSITY autorise un framebuffer Retina. La fenêtre est créée
            // masquée pour éviter d'afficher une frame incomplète pendant l'initialisation.
            constexpr SDL3::WindowFlags window_flags{SDL3::WindowResizable | SDL3::WindowHighPixelDensity | SDL3::WindowHidden};
            if (!SDL3::CreateWindowAndRenderer(cleanup_stack, "SDL3 + Dear ImGui", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags, window, renderer))
                return SDL3::Failure;

            // La VSync évite de faire tourner SDL_AppIterate() inutilement à plusieurs
            // milliers de frames par seconde. Certains renderers peuvent la refuser :
            // ce n'est donc qu'un avertissement et non une erreur fatale.
            vsync_enabled = SDL3::SetRenderVSync(renderer, 1);

            // ---------------------------------------------------------------------
            // 1. Initialisation du cœur Dear ImGui (ImGui::XXX)
            // ---------------------------------------------------------------------
            IMGUI_CHECKVERSION();
            if (ImGui::CreateContext() == nullptr) {
                SDL3::Log("Impossible de creer le contexte Dear ImGui");
                return SDL3::Failure;
            } else {
                cleanup_stack.defer([]() noexcept -> void { ImGui::DestroyContext(); });
            }

            ImGuiIO &io{ImGui::GetIO()};
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            ImGui::StyleColorsDark();

            imGuiState.initWindowDisplayScale(window);

            // ---------------------------------------------------------------------
            // 2. Backend plateforme SDL3 (ImGui_ImplSDL3_XXX)
            // ---------------------------------------------------------------------
            // Il relie ImGui à la fenêtre SDL et gère les entrées, le temps, le
            // presse-papiers, les curseurs et les gamepads. Il ne dessine rien.
            if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
                SDL3::Log("Impossible d'initialiser le backend plateforme ImGui SDL3");
                return SDL3::Failure;
            } else {
                cleanup_stack.defer([]() noexcept -> void { ImGui_ImplSDL3_Shutdown(); });
            }

            // ---------------------------------------------------------------------
            // 3. Backend de rendu SDL_Renderer (ImGui_ImplSDLRenderer3_XXX)
            // ---------------------------------------------------------------------
            // Il crée les ressources graphiques et saura convertir ImDrawData en
            // commandes compréhensibles par SDL_Renderer.
            if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
                SDL3::Log("Impossible d'initialiser le backend de rendu ImGui SDL_Renderer");
                return SDL3::Failure;
            } else {
                cleanup_stack.defer([]() noexcept -> void { ImGui_ImplSDLRenderer3_Shutdown(); });
            }

            SDL3::CenterWindow(window);

            if (!SDL3::ShowWindow(window)) {
                return SDL3::Failure;
            }

            return SDL3::Continue;
        }

        // Appelé par SDL pour chaque événement disponible.
        SDL3::AppResult dispatchEvent(SDL3::Event &event)
        {
            // Tous les événements doivent être transmis au backend plateforme avant
            // d'être interprétés par l'application. Il met ainsi à jour ImGuiIO.
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (SDL3::IsQuitEvent(event)) {
                return SDL3::Success;
            }

            if (SDL3::IsWindowCloseRequested(event, window)) {
                return SDL3::Success;
            }

            // Lors du passage entre deux écrans ayant des DPI différents, SDL signale
            // le nouveau facteur. Le style est reconstruit depuis base_style afin
            // d'éviter de multiplier plusieurs fois les dimensions déjà mises à l'échelle.
            if (SDL3::IsWindowDisplayScaleChanged(event, window)) {
                imGuiState.updateWindowDisplayScale(window);
            }

            // Échap est ici un raccourci global : il ferme toujours l'exemple, même
            // lorsqu'un widget ImGui possède le focus clavier.
            if (SDL3::IsEscapePressed(event)) {
                return SDL3::Success;
            }

            // Pour ajouter les contrôles d'une scène ou d'un jeu, consulter ensuite :
            //   ImGui::GetIO().WantCaptureKeyboard
            //   ImGui::GetIO().WantCaptureMouse
            // Quand ces valeurs sont vraies, l'événement doit rester réservé à ImGui.

            return SDL3::Continue;
        }

        // Appelé continuellement par SDL : une invocation correspond à une frame.
        SDL3::AppResult update()
        {
            // Ne consomme pas inutilement du CPU lorsque la fenêtre est minimisée.
            if (SDL3::IsWindowMinimized(window)) {
                SDL3::Delay(10);
                return SDL3::Continue;
            }

            // Le temps de départ sert au limiteur logiciel uniquement si la VSync a
            // été refusée par le renderer.
            const SDL3::Ticks frame_start_ns{SDL3::GetTicksNS()};

            // ---------------------------------------------------------------------
            // A. Préparation d'une nouvelle frame ImGui
            // ---------------------------------------------------------------------
            // Le backend de rendu prépare ses ressources pour la frame.
            ImGui_ImplSDLRenderer3_NewFrame();
            // Le backend plateforme actualise le temps, la souris et la taille écran.
            ImGui_ImplSDL3_NewFrame();
            // Le cœur ImGui ouvre enfin la nouvelle frame.
            ImGui::NewFrame();

            // Les appels ImGui::Begin/Button/Slider/etc. construisent l'interface en
            // immediate mode. Ils sont volontairement exécutés à chaque frame.

            state.renderImGui(imGuiState.display_scale, vsync_enabled);

            // Finalise l'interface et produit un ImDrawData indépendant du renderer.
            ImGui::Render();
            const ImGuiIO &io{ImGui::GetIO()};

            // ---------------------------------------------------------------------
            // B. Rendu de la frame
            // ---------------------------------------------------------------------
            // DisplaySize est exprimé en points logiques, tandis que le framebuffer
            // Retina est exprimé en pixels. Ce scale aligne les deux espaces.
            if (!SDL3::SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y)) {
                return SDL3::Failure;
            }

            if (state.renderSDL(renderer, io) == SDL3::Failure)
                return SDL3::Failure;

            // Le backend de rendu traduit ici le draw data générique d'ImGui en
            // appels SDL_Renderer. C'est seulement à cet instant qu'ImGui est dessiné.
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

            if (!SDL3::RenderPresent(renderer)) {
                return SDL3::Failure;
            }

            // Empêche la boucle de monopoliser le CPU lorsqu'aucune VSync n'est
            // disponible. SDL_DelayPrecise() reçoit directement une durée en nanosecondes.
            // Avec la VSync, SDL_RenderPresent() a déjà attendu le rafraîchissement de
            // l'écran. Sans elle, on complète la durée de la frame jusqu'à environ 60 Hz.
            if (vsync_enabled == false) {
                const SDL3::Ticks now_ns{SDL3::GetTicksNS()};
                const SDL3::Ticks elapsed_ns{now_ns - frame_start_ns};
                if (elapsed_ns < FALLBACK_FRAME_DURATION_NS) {
                    SDL3::DelayPrecise(FALLBACK_FRAME_DURATION_NS - elapsed_ns);
                }
            }

            return SDL3::Continue;
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

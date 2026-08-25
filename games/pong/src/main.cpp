// Demande à SDL3 de fournir le point d'entrée de l'application et d'appeler les
// quatre fonctions SDL_AppInit/SDL_AppEvent/SDL_AppIterate/SDL_AppQuit.
#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

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
#include <exception>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

    constexpr int WINDOW_WIDTH{960};
    constexpr int WINDOW_HEIGHT{640};
    constexpr int MIN_ANIMATION_PERIOD_MS{100};
    constexpr int MAX_ANIMATION_PERIOD_MS{4000};
    constexpr Uint64 FALLBACK_FRAME_DURATION_NS{SDL_NS_PER_SECOND / 60};

    struct ImGuiState {
        // ScaleAllSizes() multiplie le style courant et n'est donc pas
        // idempotent. Cette copie non mise à l'échelle sert de référence lors
        // de chaque changement d'écran ou de facteur DPI.
        ImGuiStyle base_style{};
        float display_scale{1.0f};

        // Conserve le style sombre non mis à l'échelle comme source de vérité,
        // puis applique le DPI de l'écran contenant actuellement la fenêtre.
        void init_display_state(SDL_Window *window)
        {
            base_style = ImGui::GetStyle();
            apply_display_scale(window);
        }

        // Le display scale SDL inclut la densité du framebuffer. Celle-ci est déjà
        // appliquée au renderer plus bas via io.DisplayFramebufferScale : l'utiliser
        // aussi pour le style agrandirait deux fois ImGui (notamment sur un écran Retina).
        // Le style ne doit donc recevoir que le facteur restant en coordonnées logiques.
        float get_display_scale(SDL_Window *window)
        {
            const float display_scale{SDL_GetWindowDisplayScale(window)};
            const float pixel_density{SDL_GetWindowPixelDensity(window)};
            if (display_scale <= 0.0f || pixel_density <= 0.0f) {
                return 1.0f;
            }
            return display_scale / pixel_density;
        }

        // Réapplique toujours le scale depuis le style de référence. Sans cette
        // restauration, deux appels à ScaleAllSizes(2) produiraient un facteur 4.
        void apply_display_scale(SDL_Window *window)
        {
            display_scale = get_display_scale(window);

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

            ImGui::Text("Template : %s", GAME_NAME);
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
        SDL_AppResult renderSDL(SDL_Renderer *renderer, const ImGuiIO &io)
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
    };

    class CleanupStack {
    public:
        // std::move_only_function exprime précisément la propriété unique des
        // actions. Le repli conserve la compatibilité avec les versions de libc++
        // qui ne fournissent pas encore cette partie de C++23.
#if defined(__cpp_lib_move_only_function) && __cpp_lib_move_only_function >= 202110L
        using Action = std::move_only_function<void() noexcept>;
#else
        using Action = std::function<void()>;
#endif

        CleanupStack()
        {
            // Effectue l'allocation principale avant l'acquisition des ressources C.
            m_actions.reserve(8);
        }

        CleanupStack(const CleanupStack &) = delete;
        CleanupStack &operator=(const CleanupStack &) = delete;
        CleanupStack(CleanupStack &&) = delete;
        CleanupStack &operator=(CleanupStack &&) = delete;

        ~CleanupStack() noexcept
        {
            for (auto action{m_actions.rbegin()}; action != m_actions.rend(); ++action) {
                (*action)();
            }
        }

        template <typename F>
            requires std::is_nothrow_invocable_v<std::decay_t<F> &>
        void defer(F &&action)
        {
            m_actions.emplace_back(std::forward<F>(action));
        }

    private:
        std::vector<Action> m_actions{};
    };

    // Toutes les données persistantes de l'application sont regroupées ici.
    // SDL transmet le pointeur placé dans « appstate » à chacun de ses callbacks.
    // Cette approche évite les variables globales et facilite l'ajout de nouveaux
    // états à l'exemple.
    struct AppContext : private CleanupStack {
        SDL_Window *window{};
        SDL_Renderer *renderer{};

        // Indique si le renderer a accepté la synchronisation verticale.
        bool vsync_enabled{};

        ImGuiState imGuiState{};
        AppState state{};

        // Appelé une seule fois par SDL au démarrage.
        SDL_AppResult initialize(int, char **)
        {

            // Ces métadonnées sont utilisées par SDL et macOS pour identifier
            // l'application. Un identifiant de type reverse-DNS doit rester stable.
            if (!SDL_SetAppMetadata("Example ImGui", "1.0", "com.kdridi.20gc.sdl3.examples.imgui")) {
                SDL_Log("Impossible de definir les metadonnees SDL : %s", SDL_GetError());
            }

            // GAMEPAD est activé parce que la navigation au gamepad est activée plus
            // bas dans ImGuiIO::ConfigFlags.
            if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
                SDL_Log("Impossible d'initialiser SDL : %s", SDL_GetError());
                return SDL_APP_FAILURE;
            } else {
                defer([]() noexcept -> void { SDL_Quit(); });
            }

            // HIGH_PIXEL_DENSITY autorise un framebuffer Retina. La fenêtre est créée
            // masquée pour éviter d'afficher une frame incomplète pendant l'initialisation.
            constexpr SDL_WindowFlags window_flags{SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN};
            if (!SDL_CreateWindowAndRenderer("SDL3 + Dear ImGui", WINDOW_WIDTH, WINDOW_HEIGHT, window_flags, &window, &renderer)) {
                SDL_Log("Impossible de creer la fenetre ou le renderer : %s", SDL_GetError());
                return SDL_APP_FAILURE;
            } else {
                defer([window = this->window]() noexcept -> void { SDL_DestroyWindow(window); });
                defer([renderer = this->renderer]() noexcept -> void { SDL_DestroyRenderer(renderer); });
            }

            // La VSync évite de faire tourner SDL_AppIterate() inutilement à plusieurs
            // milliers de frames par seconde. Certains renderers peuvent la refuser :
            // ce n'est donc qu'un avertissement et non une erreur fatale.
            vsync_enabled = SDL_SetRenderVSync(renderer, 1);
            if (!vsync_enabled) {
                SDL_Log("Impossible d'activer la VSync, utilisation du limiteur logiciel a 60 FPS : %s", SDL_GetError());
            }

            // ---------------------------------------------------------------------
            // 1. Initialisation du cœur Dear ImGui (ImGui::XXX)
            // ---------------------------------------------------------------------
            IMGUI_CHECKVERSION();
            if (ImGui::CreateContext() == nullptr) {
                SDL_Log("Impossible de creer le contexte Dear ImGui");
                return SDL_APP_FAILURE;
            } else {
                defer([]() noexcept -> void { ImGui::DestroyContext(); });
            }

            ImGuiIO &io{ImGui::GetIO()};
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

            ImGui::StyleColorsDark();

            imGuiState.init_display_state(window);

            // ---------------------------------------------------------------------
            // 2. Backend plateforme SDL3 (ImGui_ImplSDL3_XXX)
            // ---------------------------------------------------------------------
            // Il relie ImGui à la fenêtre SDL et gère les entrées, le temps, le
            // presse-papiers, les curseurs et les gamepads. Il ne dessine rien.
            if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
                SDL_Log("Impossible d'initialiser le backend plateforme ImGui SDL3");
                return SDL_APP_FAILURE;
            } else {
                defer([]() noexcept -> void { ImGui_ImplSDL3_Shutdown(); });
            }

            // ---------------------------------------------------------------------
            // 3. Backend de rendu SDL_Renderer (ImGui_ImplSDLRenderer3_XXX)
            // ---------------------------------------------------------------------
            // Il crée les ressources graphiques et saura convertir ImDrawData en
            // commandes compréhensibles par SDL_Renderer.
            if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
                SDL_Log("Impossible d'initialiser le backend de rendu ImGui SDL_Renderer");
                return SDL_APP_FAILURE;
            } else {
                defer([]() noexcept -> void { ImGui_ImplSDLRenderer3_Shutdown(); });
            }

            if (!SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
                SDL_Log("Impossible de centrer la fenetre : %s", SDL_GetError());
            }

            if (!SDL_ShowWindow(window)) {
                SDL_Log("Impossible d'afficher la fenetre : %s", SDL_GetError());
                return SDL_APP_FAILURE;
            }

            return SDL_APP_CONTINUE;
        }

        // Appelé par SDL pour chaque événement disponible.
        SDL_AppResult dispatchEvent(SDL_Event &event)
        {
            // Tous les événements doivent être transmis au backend plateforme avant
            // d'être interprétés par l'application. Il met ainsi à jour ImGuiIO.
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT) {
                return SDL_APP_SUCCESS;
            }

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window)) {
                return SDL_APP_SUCCESS;
            }

            // Lors du passage entre deux écrans ayant des DPI différents, SDL signale
            // le nouveau facteur. Le style est reconstruit depuis base_style afin
            // d'éviter de multiplier plusieurs fois les dimensions déjà mises à l'échelle.
            if (event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED && event.window.windowID == SDL_GetWindowID(window)) {
                imGuiState.apply_display_scale(window);
            }

            // Échap est ici un raccourci global : il ferme toujours l'exemple, même
            // lorsqu'un widget ImGui possède le focus clavier.
            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                return SDL_APP_SUCCESS;
            }

            // Pour ajouter les contrôles d'une scène ou d'un jeu, consulter ensuite :
            //   ImGui::GetIO().WantCaptureKeyboard
            //   ImGui::GetIO().WantCaptureMouse
            // Quand ces valeurs sont vraies, l'événement doit rester réservé à ImGui.

            return SDL_APP_CONTINUE;
        }

        // Appelé continuellement par SDL : une invocation correspond à une frame.
        SDL_AppResult update()
        {
            // Ne consomme pas inutilement du CPU lorsque la fenêtre est minimisée.
            if ((SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) != 0) {
                SDL_Delay(10);
                return SDL_APP_CONTINUE;
            }

            // Le temps de départ sert au limiteur logiciel uniquement si la VSync a
            // été refusée par le renderer.
            const Uint64 frame_start_ns{SDL_GetTicksNS()};

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
            if (!SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y)) {
                SDL_Log("Impossible de definir l'echelle du renderer : %s", SDL_GetError());
                return SDL_APP_FAILURE;
            }

            if (state.renderSDL(renderer, io) == SDL_APP_FAILURE)
                return SDL_APP_FAILURE;

            // Le backend de rendu traduit ici le draw data générique d'ImGui en
            // appels SDL_Renderer. C'est seulement à cet instant qu'ImGui est dessiné.
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

            if (!SDL_RenderPresent(renderer)) {
                SDL_Log("Impossible de presenter la frame : %s", SDL_GetError());
                return SDL_APP_FAILURE;
            }

            // Empêche la boucle de monopoliser le CPU lorsqu'aucune VSync n'est
            // disponible. SDL_DelayPrecise() reçoit directement une durée en nanosecondes.
            // Avec la VSync, SDL_RenderPresent() a déjà attendu le rafraîchissement de
            // l'écran. Sans elle, on complète la durée de la frame jusqu'à environ 60 Hz.
            if (vsync_enabled == false) {
                const Uint64 now_ns{SDL_GetTicksNS()};
                const Uint64 elapsed_ns{now_ns - frame_start_ns};
                if (elapsed_ns < FALLBACK_FRAME_DURATION_NS) {
                    SDL_DelayPrecise(FALLBACK_FRAME_DURATION_NS - elapsed_ns);
                }
            }

            return SDL_APP_CONTINUE;
        }
    };

} // namespace

// Appelé une seule fois par SDL au démarrage.
SDL_AppResult SDL_AppInit(void **_contextPtr, int argc, char **argv)
{
    if (_contextPtr == nullptr)
        return SDL_APP_FAILURE;
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
        SDL_Log("Exception pendant l'initialisation : %s", exception.what());
    } catch (...) {
        SDL_Log("Exception inconnue pendant l'initialisation");
    }

    delete context;
    *_contextPtr = nullptr;
    return SDL_APP_FAILURE;
}

// Appelé par SDL pour chaque événement disponible.
SDL_AppResult SDL_AppEvent(void *_context, SDL_Event *_event)
{
    if (_context == nullptr)
        return SDL_APP_FAILURE;
    AppContext &context{*static_cast<AppContext *>(_context)};

    if (_event == nullptr)
        return SDL_APP_FAILURE;
    SDL_Event &event = *_event;

    return context.dispatchEvent(event);
}

// Appelé continuellement par SDL : une invocation correspond à une frame.
SDL_AppResult SDL_AppIterate(void *_context)
{
    if (_context == nullptr)
        return SDL_APP_FAILURE;
    AppContext &context{*static_cast<AppContext *>(_context)};

    return context.update();
}

// Appelé une seule fois à l'arrêt, y compris si SDL_AppInit() a échoué.
void SDL_AppQuit(void *_context, SDL_AppResult)
{
    if (_context == nullptr)
        return;
    AppContext *context{static_cast<AppContext *>(_context)};

    delete context;
}

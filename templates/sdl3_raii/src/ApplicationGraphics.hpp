#pragma once

#include "CleanupStack.hpp"
#include "DearImGui.hpp"
#include "SDL3.hpp"

namespace ApplicationGraphics {

    struct Config {
        const char *application_name{"Example ImGui"};
        const char *application_version{"1.0"};
        const char *application_identifier{"com.kdridi.20gc.sdl3.examples.imgui"};
        const char *window_title{"SDL3 + Dear ImGui"};
        int window_width{960};
        int window_height{640};
        SDL3::WindowFlags window_flags{SDL3::WindowResizable | SDL3::WindowHighPixelDensity | SDL3::WindowHidden};
    };

    struct Runtime {
        SDL3::Window *window{};
        SDL3::Renderer *renderer{};
        bool vsync_enabled{};
    };

    inline constexpr SDL3::Ticks FallbackFrameDurationNS{SDL3::NanosecondsPerSecond / 60};

    inline bool Initialize(CleanupStack &cleanup_stack, Runtime &runtime, const Config &config = {})
    {
        auto transaction{cleanup_stack.transaction()};

        // Les métadonnées sont utiles mais leur absence n'empêche pas l'application
        // de fonctionner ; l'adaptateur SDL journalise déjà un éventuel échec.
        SDL3::SetAppMetadata(config.application_name, config.application_version, config.application_identifier);

        if (!SDL3::Init(cleanup_stack, SDL3::InitVideo | SDL3::InitGamepad)) {
            return false;
        }

        if (!SDL3::CreateWindowAndRenderer(
                cleanup_stack,
                config.window_title,
                config.window_width,
                config.window_height,
                config.window_flags,
                runtime.window,
                runtime.renderer)) {
            return false;
        }

        // Le refus de la VSync n'est pas fatal : la boucle principale utilisera
        // son limiteur logiciel de fréquence d'affichage.
        runtime.vsync_enabled = SDL3::SetRenderVSync(runtime.renderer, 1);

        if (!DearImGui::InitForSDLRenderer(cleanup_stack, runtime.window, runtime.renderer)) {
            return false;
        }

        SDL3::CenterWindow(runtime.window);
        if (!SDL3::ShowWindow(runtime.window)) {
            return false;
        }

        transaction.commit();
        return true;
    }

    inline SDL3::AppResult DispatchEvent(const Runtime &runtime, SDL3::Event &event)
    {
        DearImGui::ProcessEvent(event);

        if (SDL3::IsQuitEvent(event) ||
            SDL3::IsWindowCloseRequested(event, runtime.window) ||
            SDL3::IsEscapePressed(event)) {
            return SDL3::Success;
        }

        return SDL3::Continue;
    }

    template<typename RenderImGui, typename RenderScene>
    SDL3::AppResult RenderFrame(Runtime &runtime, RenderImGui &&render_imgui, RenderScene &&render_scene)
    {
        if (SDL3::IsWindowMinimized(runtime.window)) {
            SDL3::Delay(10);
            return SDL3::Continue;
        }

        const SDL3::Ticks frame_start_ns{SDL3::GetTicksNS()};

        // L'interface est construite en premier, mais son draw data sera dessiné
        // après la scène afin qu'ImGui apparaisse toujours au premier plan.
        DearImGui::BeginFrame();
        render_imgui();
        DearImGui::EndFrame();

        if (!DearImGui::ApplyFramebufferScale(runtime.renderer)) {
            return SDL3::Failure;
        }

        SDL3::RenderSize render_size{};
        if (!SDL3::GetRenderSize(runtime.window, render_size)) {
            return SDL3::Failure;
        }

        if (!render_scene(runtime.renderer, render_size)) {
            return SDL3::Failure;
        }

        DearImGui::RenderDrawData(runtime.renderer);
        if (!SDL3::RenderPresent(runtime.renderer)) {
            return SDL3::Failure;
        }

        if (!runtime.vsync_enabled) {
            const SDL3::Ticks elapsed_ns{SDL3::GetTicksNS() - frame_start_ns};
            if (elapsed_ns < FallbackFrameDurationNS) {
                SDL3::DelayPrecise(FallbackFrameDurationNS - elapsed_ns);
            }
        }

        return SDL3::Continue;
    }

} // namespace ApplicationGraphics

#pragma once

#include "CleanupStack.hpp"
#include "SDL3.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace DearImGui {

    namespace detail {

        class ContextDisplayScaleState {
        public:
            // Conserve le style non mis à l'échelle comme source de vérité, puis
            // applique le DPI de l'écran contenant actuellement la fenêtre.
            void initialize(SDL3::Window *window)
            {
                m_base_style = ImGui::GetStyle();
                update(window);
            }

            // ScaleAllSizes() n'est pas idempotent : le style de référence est donc
            // restauré avant chaque changement d'écran ou de facteur DPI.
            void update(SDL3::Window *window)
            {
                const float scale{SDL3::GetWindowDisplayScale(window)};

                ImGuiStyle &style{ImGui::GetStyle()};
                style = m_base_style;
                style.ScaleAllSizes(scale);
                style.FontScaleDpi = scale;
            }

        private:
            ImGuiStyle m_base_style{};
        };

        struct RuntimeState {
            SDL3::Window *window{};
            ContextDisplayScaleState display_scale{};
        };

        inline RuntimeState &GetRuntimeState()
        {
            static RuntimeState state{};
            return state;
        }

        inline bool RegisterWindow(SDL3::Window *window)
        {
            RuntimeState &state{GetRuntimeState()};
            if (state.window != nullptr) {
                SDL3::Log("Une fenetre est deja associee au contexte Dear ImGui");
                return false;
            }

            state.window = window;
            return true;
        }

        inline void UnregisterWindow(SDL3::Window *window) noexcept
        {
            RuntimeState &state{GetRuntimeState()};
            if (state.window == window) {
                state.window = nullptr;
            }
        }

    } // namespace detail

    inline bool InitForSDLRenderer(CleanupStack &cleanup_stack, SDL3::Window *window, SDL3::Renderer *renderer)
    {
        auto transaction{cleanup_stack.transaction()};

        IMGUI_CHECKVERSION();
        if (ImGui::CreateContext() == nullptr) {
            SDL3::Log("Impossible de creer le contexte Dear ImGui");
            return false;
        }
        transaction.defer([]() noexcept -> void { ImGui::DestroyContext(); });

        ImGuiIO &io{ImGui::GetIO()};
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        ImGui::StyleColorsDark();

        if (!ImGui_ImplSDL3_InitForSDLRenderer(window, renderer)) {
            SDL3::Log("Impossible d'initialiser le backend plateforme ImGui SDL3");
            return false;
        }
        transaction.defer([]() noexcept -> void { ImGui_ImplSDL3_Shutdown(); });

        if (!ImGui_ImplSDLRenderer3_Init(renderer)) {
            SDL3::Log("Impossible d'initialiser le backend de rendu ImGui SDL_Renderer");
            return false;
        }
        transaction.defer([]() noexcept -> void { ImGui_ImplSDLRenderer3_Shutdown(); });

        if (!detail::RegisterWindow(window)) {
            return false;
        }
        transaction.defer([window]() noexcept -> void { detail::UnregisterWindow(window); });
        detail::GetRuntimeState().display_scale.initialize(window);

        transaction.commit();
        return true;
    }

    inline void ProcessEvent(SDL3::Event &event)
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

        detail::RuntimeState &state{detail::GetRuntimeState()};
        if (state.window != nullptr && SDL3::IsWindowDisplayScaleChanged(event, state.window)) {
            state.display_scale.update(state.window);
        }
    }

    inline void BeginFrame()
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    inline void EndFrame()
    {
        ImGui::Render();
    }

    inline bool ApplyFramebufferScale(SDL3::Renderer *renderer)
    {
        const ImGuiIO &io{ImGui::GetIO()};
        return SDL3::SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    }

    inline void RenderDrawData(SDL3::Renderer *renderer)
    {
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

} // namespace DearImGui

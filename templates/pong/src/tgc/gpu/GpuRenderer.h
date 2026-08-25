#pragma once

#include "tgc/gpu/SolidVertex.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace tgc::gpu {
    class GpuRenderer {
    public:
        GpuRenderer();
        ~GpuRenderer() noexcept;

        GpuRenderer(const GpuRenderer &) = delete;
        GpuRenderer &operator=(const GpuRenderer &) = delete;

        bool init(SDL_Window *window);
        void shutdown() noexcept;

        void beginFrame(glm::vec2 worldSize, glm::vec4 clearColor);
        void drawRectangle(glm::vec2 position, float rotation, glm::vec2 size, glm::vec2 scale, glm::vec4 color);
        bool endFrame() noexcept;

    private:
        bool createPipeline() noexcept;
        SDL_GPUShader *createShader(SDL_GPUShaderStage stage) const noexcept;
        glm::vec2 worldToNdc(glm::vec2 position) const noexcept;

        SDL_Window *m_window{};
        SDL_GPUDevice *m_device{};
        SDL_GPUGraphicsPipeline *m_pipeline{};
        SDL_GPUBuffer *m_vertexBuffer{};
        SDL_GPUTransferBuffer *m_transferBuffer{};
        bool m_windowClaimed{};

        glm::vec2 m_worldSize{1.0F};
        glm::vec4 m_clearColor{};
        std::vector<SolidVertex> m_vertices{};
    };
} // namespace tgc::gpu

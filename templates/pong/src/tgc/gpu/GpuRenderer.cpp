#include "tgc/gpu/GpuRenderer.h"

#include "tgc/gpu/ShaderBinary.h"
#include "tgc/gpu/shaders/generated/solid.frag.dxil.h"
#include "tgc/gpu/shaders/generated/solid.frag.msl.h"
#include "tgc/gpu/shaders/generated/solid.frag.spv.h"
#include "tgc/gpu/shaders/generated/solid.vert.dxil.h"
#include "tgc/gpu/shaders/generated/solid.vert.msl.h"
#include "tgc/gpu/shaders/generated/solid.vert.spv.h"

#include <SDL3/SDL.h>

#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include <array>
#include <cstddef>
#include <cstring>

namespace tgc::gpu {
    namespace {
        constexpr SDL_GPUShaderFormat supportedShaderFormats{
            SDL_GPU_SHADERFORMAT_SPIRV |
                SDL_GPU_SHADERFORMAT_DXIL |
                SDL_GPU_SHADERFORMAT_MSL,
        };
        constexpr Uint32 maxVertices{4096};

        ShaderBinary selectShaderBinary(SDL_GPUShaderFormat formats, SDL_GPUShaderStage stage) noexcept
        {
            const bool vertex{stage == SDL_GPU_SHADERSTAGE_VERTEX};

            if ((formats & SDL_GPU_SHADERFORMAT_DXIL) != 0) {
                return vertex
                           ? ShaderBinary{solid_vert_dxil, solid_vert_dxil_len, SDL_GPU_SHADERFORMAT_DXIL}
                           : ShaderBinary{solid_frag_dxil, solid_frag_dxil_len, SDL_GPU_SHADERFORMAT_DXIL};
            }
            if ((formats & SDL_GPU_SHADERFORMAT_MSL) != 0) {
                return vertex
                           ? ShaderBinary{solid_vert_msl, solid_vert_msl_len, SDL_GPU_SHADERFORMAT_MSL}
                           : ShaderBinary{solid_frag_msl, solid_frag_msl_len, SDL_GPU_SHADERFORMAT_MSL};
            }
            if ((formats & SDL_GPU_SHADERFORMAT_SPIRV) != 0) {
                return vertex
                           ? ShaderBinary{solid_vert_spv, solid_vert_spv_len, SDL_GPU_SHADERFORMAT_SPIRV}
                           : ShaderBinary{solid_frag_spv, solid_frag_spv_len, SDL_GPU_SHADERFORMAT_SPIRV};
            }
            return {};
        }
    } // namespace

    GpuRenderer::GpuRenderer() = default;

    GpuRenderer::~GpuRenderer() noexcept
    {
        shutdown();
    }

    bool GpuRenderer::init(SDL_Window *window)
    {
        m_window = window;

#ifndef NDEBUG
        constexpr bool debugMode{true};
#else
        constexpr bool debugMode{false};
#endif

        m_device = SDL_CreateGPUDevice(supportedShaderFormats, debugMode, nullptr);
        if (m_device == nullptr) {
            SDL_Log("SDL_CreateGPUDevice failed: %s", SDL_GetError());
            return false;
        }

        if (!SDL_ClaimWindowForGPUDevice(m_device, m_window)) {
            SDL_Log("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
            shutdown();
            return false;
        }
        m_windowClaimed = true;

        SDL_Log("SDL_GPU driver: %s", SDL_GetGPUDeviceDriver(m_device));

        if (!createPipeline()) {
            shutdown();
            return false;
        }

        const SDL_GPUBufferCreateInfo vertexBufferInfo{
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = maxVertices * static_cast<Uint32>(sizeof(SolidVertex)),
            .props = 0,
        };
        m_vertexBuffer = SDL_CreateGPUBuffer(m_device, &vertexBufferInfo);
        if (m_vertexBuffer == nullptr) {
            SDL_Log("SDL_CreateGPUBuffer failed: %s", SDL_GetError());
            shutdown();
            return false;
        }

        const SDL_GPUTransferBufferCreateInfo transferBufferInfo{
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = maxVertices * static_cast<Uint32>(sizeof(SolidVertex)),
            .props = 0,
        };
        m_transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferBufferInfo);
        if (m_transferBuffer == nullptr) {
            SDL_Log("SDL_CreateGPUTransferBuffer failed: %s", SDL_GetError());
            shutdown();
            return false;
        }

        m_vertices.reserve(maxVertices);
        return true;
    }

    void GpuRenderer::shutdown() noexcept
    {
        if (m_device == nullptr) {
            return;
        }

        SDL_WaitForGPUIdle(m_device);

        if (m_transferBuffer != nullptr) {
            SDL_ReleaseGPUTransferBuffer(m_device, m_transferBuffer);
            m_transferBuffer = nullptr;
        }
        if (m_vertexBuffer != nullptr) {
            SDL_ReleaseGPUBuffer(m_device, m_vertexBuffer);
            m_vertexBuffer = nullptr;
        }
        if (m_pipeline != nullptr) {
            SDL_ReleaseGPUGraphicsPipeline(m_device, m_pipeline);
            m_pipeline = nullptr;
        }
        if (m_windowClaimed) {
            SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
            m_windowClaimed = false;
        }

        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
        m_window = nullptr;
    }

    SDL_GPUShader *GpuRenderer::createShader(SDL_GPUShaderStage stage) const noexcept
    {
        const ShaderBinary binary{selectShaderBinary(SDL_GetGPUShaderFormats(m_device), stage)};
        if (binary.code == nullptr) {
            SDL_Log("No compatible embedded shader format is available");
            return nullptr;
        }

        const SDL_GPUShaderCreateInfo shaderInfo{
            .code_size = binary.size,
            .code = binary.code,
            .entrypoint = nullptr,
            .format = binary.format,
            .stage = stage,
            .num_samplers = 0,
            .num_storage_textures = 0,
            .num_storage_buffers = 0,
            .num_uniform_buffers = stage == SDL_GPU_SHADERSTAGE_VERTEX ? 1U : 0U,
            .props = 0,
        };
        return SDL_CreateGPUShader(m_device, &shaderInfo);
    }

    bool GpuRenderer::createPipeline() noexcept
    {
        SDL_GPUShader *vertexShader{createShader(SDL_GPU_SHADERSTAGE_VERTEX)};
        if (vertexShader == nullptr) {
            SDL_Log("Vertex shader creation failed: %s", SDL_GetError());
            return false;
        }

        SDL_GPUShader *fragmentShader{createShader(SDL_GPU_SHADERSTAGE_FRAGMENT)};
        if (fragmentShader == nullptr) {
            SDL_Log("Fragment shader creation failed: %s", SDL_GetError());
            SDL_ReleaseGPUShader(m_device, vertexShader);
            return false;
        }

        const SDL_GPUVertexBufferDescription vertexBufferDescription{
            .slot = 0,
            .pitch = sizeof(SolidVertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        };
        const std::array vertexAttributes{
            SDL_GPUVertexAttribute{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = offsetof(SolidVertex, position),
            },
            SDL_GPUVertexAttribute{
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = offsetof(SolidVertex, color),
            },
        };

        const SDL_GPUColorTargetDescription colorTarget{
            .format = SDL_GetGPUSwapchainTextureFormat(m_device, m_window),
            .blend_state = {},
        };

        SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.vertex_shader = vertexShader;
        pipelineInfo.fragment_shader = fragmentShader;
        pipelineInfo.vertex_input_state = SDL_GPUVertexInputState{
            .vertex_buffer_descriptions = &vertexBufferDescription,
            .num_vertex_buffers = 1,
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = static_cast<Uint32>(vertexAttributes.size()),
        };
        pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
        pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
        pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
        pipelineInfo.rasterizer_state.enable_depth_clip = true;
        pipelineInfo.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
        pipelineInfo.target_info.color_target_descriptions = &colorTarget;
        pipelineInfo.target_info.num_color_targets = 1;

        m_pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);
        SDL_ReleaseGPUShader(m_device, fragmentShader);
        SDL_ReleaseGPUShader(m_device, vertexShader);

        if (m_pipeline == nullptr) {
            SDL_Log("SDL_CreateGPUGraphicsPipeline failed: %s", SDL_GetError());
            return false;
        }
        return true;
    }

    void GpuRenderer::beginFrame(glm::vec2 worldSize, glm::vec4 clearColor)
    {
        m_worldSize = worldSize;
        m_clearColor = clearColor;
        m_vertices.clear();
    }

    glm::vec2 GpuRenderer::worldToNdc(glm::vec2 position) const noexcept
    {
        return {
            position.x / (m_worldSize.x * 0.5F),
            position.y / (m_worldSize.y * 0.5F),
        };
    }

    void GpuRenderer::drawRectangle(glm::vec2 position, float rotation, glm::vec2 size, glm::vec2 scale, glm::vec4 color)
    {
        if (m_vertices.size() + 6 > maxVertices) {
            return;
        }

        const glm::vec2 halfSize{0.5F * size * scale};
        const float cosine{glm::cos(rotation)};
        const float sine{glm::sin(rotation)};
        const auto transformPoint = [&](glm::vec2 local) {
            const glm::vec2 rotated{
                local.x * cosine - local.y * sine,
                local.x * sine + local.y * cosine,
            };
            return worldToNdc(position + rotated);
        };

        const std::array corners{
            transformPoint({-halfSize.x, -halfSize.y}),
            transformPoint({halfSize.x, -halfSize.y}),
            transformPoint({halfSize.x, halfSize.y}),
            transformPoint({-halfSize.x, halfSize.y}),
        };
        constexpr std::array<Uint32, 6> indices{0, 1, 2, 0, 2, 3};

        for (const Uint32 index : indices) {
            const glm::vec2 point{corners[index]};
            m_vertices.push_back(SolidVertex{
                .position = {point.x, point.y, 0.0F},
                .color = {color.r, color.g, color.b},
            });
        }
    }

    bool GpuRenderer::endFrame() noexcept
    {
        const Uint32 dataSize{static_cast<Uint32>(m_vertices.size() * sizeof(SolidVertex))};
        if (!m_vertices.empty()) {
            void *mapped{SDL_MapGPUTransferBuffer(m_device, m_transferBuffer, true)};
            if (mapped == nullptr) {
                SDL_Log("SDL_MapGPUTransferBuffer failed: %s", SDL_GetError());
                return false;
            }
            std::memcpy(mapped, m_vertices.data(), dataSize);
            SDL_UnmapGPUTransferBuffer(m_device, m_transferBuffer);
        }

        SDL_GPUCommandBuffer *commandBuffer{SDL_AcquireGPUCommandBuffer(m_device)};
        if (commandBuffer == nullptr) {
            SDL_Log("SDL_AcquireGPUCommandBuffer failed: %s", SDL_GetError());
            return false;
        }

        if (!m_vertices.empty()) {
            SDL_GPUCopyPass *copyPass{SDL_BeginGPUCopyPass(commandBuffer)};
            if (copyPass == nullptr) {
                SDL_Log("SDL_BeginGPUCopyPass failed: %s", SDL_GetError());
                SDL_CancelGPUCommandBuffer(commandBuffer);
                return false;
            }

            const SDL_GPUTransferBufferLocation source{
                .transfer_buffer = m_transferBuffer,
                .offset = 0,
            };
            const SDL_GPUBufferRegion destination{
                .buffer = m_vertexBuffer,
                .offset = 0,
                .size = dataSize,
            };
            SDL_UploadToGPUBuffer(copyPass, &source, &destination, true);
            SDL_EndGPUCopyPass(copyPass);
        }

        SDL_GPUTexture *swapchainTexture{};
        Uint32 textureWidth{};
        Uint32 textureHeight{};
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, m_window, &swapchainTexture, &textureWidth, &textureHeight)) {
            SDL_Log("SDL_WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
            SDL_CancelGPUCommandBuffer(commandBuffer);
            return false;
        }
        if (swapchainTexture == nullptr) {
            SDL_CancelGPUCommandBuffer(commandBuffer);
            return true;
        }

        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = swapchainTexture;
        colorTarget.clear_color = SDL_FColor{m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a};
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass *renderPass{SDL_BeginGPURenderPass(commandBuffer, &colorTarget, 1, nullptr)};
        if (renderPass == nullptr) {
            SDL_Log("SDL_BeginGPURenderPass failed: %s", SDL_GetError());
            if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
                SDL_Log("SDL_SubmitGPUCommandBuffer after render-pass failure failed: %s", SDL_GetError());
            }
            return false;
        }

        const float worldAspect{m_worldSize.x / m_worldSize.y};
        const float textureAspect{static_cast<float>(textureWidth) / static_cast<float>(textureHeight)};
        SDL_GPUViewport viewport{0.0F, 0.0F, static_cast<float>(textureWidth), static_cast<float>(textureHeight), 0.0F, 1.0F};
        if (textureAspect > worldAspect) {
            viewport.w = viewport.h * worldAspect;
            viewport.x = (static_cast<float>(textureWidth) - viewport.w) * 0.5F;
        } else {
            viewport.h = viewport.w / worldAspect;
            viewport.y = (static_cast<float>(textureHeight) - viewport.h) * 0.5F;
        }
        SDL_SetGPUViewport(renderPass, &viewport);

        if (!m_vertices.empty()) {
            SDL_BindGPUGraphicsPipeline(renderPass, m_pipeline);
            const SDL_GPUBufferBinding binding{
                .buffer = m_vertexBuffer,
                .offset = 0,
            };
            SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);

            const glm::mat4 identity{1.0F};
            SDL_PushGPUVertexUniformData(commandBuffer, 0, &identity, sizeof(identity));
            SDL_DrawGPUPrimitives(renderPass, static_cast<Uint32>(m_vertices.size()), 1, 0, 0);
        }

        SDL_EndGPURenderPass(renderPass);
        if (!SDL_SubmitGPUCommandBuffer(commandBuffer)) {
            SDL_Log("SDL_SubmitGPUCommandBuffer failed: %s", SDL_GetError());
            return false;
        }
        return true;
    }
} // namespace tgc::gpu

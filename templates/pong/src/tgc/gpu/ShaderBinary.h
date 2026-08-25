#pragma once

#include <SDL3/SDL_gpu.h>

#include <cstddef>

namespace tgc::gpu {
    struct ShaderBinary {
        const unsigned char *code{};
        std::size_t size{};
        SDL_GPUShaderFormat format{};
    };
} // namespace tgc::gpu

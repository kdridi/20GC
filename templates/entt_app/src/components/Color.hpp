#pragma once

#include <array>
#include <cstdint>

namespace components {
    struct Color {
        std::array<uint8_t, 4> data{};

        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
            : data({r, g, b, a}) {}
    };
} // namespace components

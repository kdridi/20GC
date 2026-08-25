#pragma once

namespace components {
    struct Vector {
        double x, y;

        Vector() noexcept
            : x{}, y{} {}

        Vector(double x, double y)
            : x{x}, y{y} {}
    };
} // namespace components

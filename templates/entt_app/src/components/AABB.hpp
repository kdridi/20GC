#pragma once

#include <components/Vector.hpp>

#include <algorithm>

namespace components {
    struct AABB {
        Vector minp, maxp;

        AABB(double x, double y, double width, double height) noexcept
            : minp{x, y}, maxp{x, y}
        {
            growth(Vector{x + width, y + height});
        }

        double x() const { return minp.x; }
        double y() const { return minp.y; }
        double width() const { return maxp.x - minp.x; }
        double height() const { return maxp.y - minp.y; }

        AABB &growth(const Vector &_v)
        {
            minp.x = std::min(minp.x, _v.x);
            minp.y = std::min(minp.y, _v.y);
            maxp.x = std::max(maxp.x, _v.x);
            maxp.y = std::max(maxp.y, _v.y);
            return *this;
        }
    };
} // namespace components

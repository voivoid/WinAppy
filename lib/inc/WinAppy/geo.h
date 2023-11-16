#pragma once

#include <cstddef>

namespace winappy
{
using coord = int;
using index = size_t;

struct point
{
    coord x;
    coord y;
};

inline point operator+(point p, coord c)
{
    return { p.x + c, p.y + c };
}

struct rect_size
{
    size_t width;
    size_t height;
};

struct rectangle
{
    point left_top;
    point right_bottom;

    point get_left_top() const
    {
        return left_top;
    }

    point get_right_bottom() const
    {
        return right_bottom;
    }

    point get_left_bottom() const
    {
        return { left_top.x, right_bottom.y };
    }

    point get_right_top() const
    {
        return { right_bottom.x, left_top.y };
    }
};

template <typename Destination, typename Source>
Destination numeric_cast(Source src)
{
    return static_cast<Destination>(src);
}

}  // namespace winappy
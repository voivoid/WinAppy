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
    static rectangle make(int left, int top, int right, int bottom)
    {
        return { .left_top{ left, top }, .right_bottom{ right, bottom } };
    }

    static rectangle make(int x, int y, rect_size s)
    {
        return { .left_top{ x, y }, .right_bottom{ x + static_cast<int>(s.width), y + static_cast<int>(s.height) } };
    }

    static rectangle make(point left_top, point right_bottom)
    {
        return { left_top, right_bottom };
    }

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

    int left() const
    {
        return left_top.x;
    }

    int right() const
    {
        return right_bottom.x;
    }

    int top() const
    {
        return left_top.y;
    }

    int bottom() const
    {
        return right_bottom.y;
    }

    rectangle inflate(int diff_left, int diff_top, int diff_right, int diff_bottom)
    {
        return make(left() + diff_left, top() + diff_top, right() - diff_right, bottom() - diff_bottom);
    }

    rectangle inflate(int diff)
    {
        return inflate(diff, diff, diff, diff);
    }
};

template <typename Destination, typename Source>
Destination numeric_cast(Source src)
{
    return static_cast<Destination>(src);
}

}  // namespace winappy
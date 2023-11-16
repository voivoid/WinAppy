#pragma once

#include "WinAppy/geo.h"

namespace winappy
{
class DC;
namespace gdi
{
struct API
{
    static void move_to(DC& dc, int x, int y);
    static void move_to(DC& dc, int x, int y, int& prev_x, int& prev_y);
    static void move_to(DC& dc, point new_pos);
    static void move_to(DC& dc, point new_pos, point& prev_pos);

    static void line_to(DC& dc, int x, int y);
    static void line_to(DC& dc, point pos);

    static void rectangle(DC& dc, int left, int top, int right, int bottom);
    static void rectangle(DC& dc, const winappy::rectangle& rect);
};
}  // namespace gdi

}  // namespace winappy
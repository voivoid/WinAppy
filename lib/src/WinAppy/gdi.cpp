#include "stdafx.h"

#include "WinAppy/dc.h"
#include "WinAppy/gdi.h"

namespace winappy
{
namespace gdi
{


void API::move_to(DC& dc, int x, int y)
{
    ::MoveToEx(dc.handle(), x, y, nullptr);
}

void API::move_to(DC& dc, int x, int y, int& prev_x, int& prev_y)
{
    POINT prev;
    ::MoveToEx(dc.handle(), x, y, &prev);
    prev_x = prev.x;
    prev_y = prev.y;
}

void API::move_to(DC& dc, point new_pos)
{
    API::move_to(dc, new_pos.x, new_pos.y);
}

void API::move_to(DC& dc, point new_pos, point& prev_pos)
{
    API::move_to(dc, new_pos.x, new_pos.y, prev_pos.x, prev_pos.y);
}

void API::line_to(DC& dc, int x, int y)
{
    ::LineTo(dc.handle(), x, y);
}

void API::line_to(DC& dc, point pos)
{
    API::line_to(dc, pos.x, pos.y);
}

void API::rectangle(DC& dc, int left, int top, int right, int bottom)
{
    ::Rectangle(dc.handle(), left, top, right, bottom);
}

void API::rectangle(DC& dc, const winappy::rectangle& rect)
{
    API::rectangle(dc, rect.left_top.x, rect.left_top.y, rect.right_bottom.x, rect.right_bottom.y);
}

}  // namespace gdi
}  // namespace winappy
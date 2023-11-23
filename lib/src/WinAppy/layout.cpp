#include "stdafx.h"

#include "WinAppy/control.h"
#include "WinAppy/layout.h"
#include "WinAppy/window_api.h"

namespace winappy
{

std::optional<LRESULT> Layout::try_reflect_notification(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    const HWND control_wnd = reinterpret_cast<HWND>(lParam);
    if (!control_wnd) return {};

    const auto& controls = get_controls();
    auto iter            = controls.find(control_wnd);
    if (iter == controls.end()) return {};

    return iter->second->try_reflect_notification(msg, wParam, lParam);
}

void Layout::set_padding(const int padding_in_pixels)
{
    m_padding = padding_in_pixels;
}

int Layout::get_padding() const
{
    return m_padding;
}

void Layout::add_control(Control& control)
{
    m_controls.emplace(control.handle(), &control);
}

void Layout::remove_control(Control& control)
{
    m_controls.erase(control.handle());
}

void Layout::clear_controls()
{
    m_controls.clear();
}

namespace layouts
{

void Fill::set_control(Control& control)
{
    if (!get_controls().empty()) { clear_controls(); }

    add_control(control);
}
void Fill::clear_control()
{
    clear_controls();
}

void Fill::update(const int new_width, const int new_height)
{
    const auto& controls = get_controls();
    if (controls.empty()) return;
    assert(controls.size() == 1);
    auto& control = *controls.begin()->second;

    auto rect = rectangle::make(0, 0, new_width, new_height).inflate(get_padding());

    Window::API::try_move_window(control, rect);
    Window::API::try_show_window(control, SW_SHOW);
}

void Horizontal::push_control(Control& /*control*/) {}

void Horizontal::pop_control(Control& /*control*/) {}

void Horizontal::pop_control() {}

void Horizontal::clear_control() {}

void Vertical::push_control(Control& /*control*/) {}

void Vertical::pop_control(Control& /*control*/) {}

void Vertical::pop_control() {}

void Vertical::clear_control() {}

}  // namespace layouts

}  // namespace winappy
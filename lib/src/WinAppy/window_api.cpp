#include "stdafx.h"

#include "WinAppy/dc.h"
#include "WinAppy/window_api.h"

#include <Windows.h>

namespace winappy
{
result<> Window::API::try_destroy_window(Window& w)
{
    WINAPPY_RETURN_LAST_ERROR_IF_FALSE(::DestroyWindow(w.handle()));
    return WINAPPY_SUCCESS;
}

result<> Window::API::try_update_window(Window& w)
{
    WINAPPY_RETURN_LAST_ERROR_IF_FALSE(::UpdateWindow(w.handle()));
    return WINAPPY_SUCCESS;
}

result<bool> Window::API::try_show_window(Window& w, const int show_mode)
{
    const BOOL res = ::ShowWindow(w.handle(), show_mode);
    if (::GetLastError() != 0) WINAPPY_RETURN_LAST_ERROR("try_show_window failed due to ::ShowWindow error");

    return { res };
}

result<WNDPROC> Window::API::try_update_wnd_proc(Window& w, const WNDPROC wnd_proc)
{
    ::SetLastError(0);

    const auto prev_value = SubclassWindow(w.handle(), wnd_proc);
    if (::GetLastError() != 0) WINAPPY_RETURN_LAST_ERROR("try_update_wnd_proc failed due to ::SubclassWindow error");

    return { prev_value };
}

result<ATOM> Window::API::try_register_class_ex(const UINT style, const WNDPROC wnd_proc, const LPCWSTR name, const std::optional<Window::ClassOpts>& opts)
{
    assert(wnd_proc);
    assert(name);

    const bool has_opts = opts.has_value();
    const WNDCLASSEX wcx{ .cbSize        = sizeof(WNDCLASSEX),
                          .style         = style,
                          .lpfnWndProc   = wnd_proc,
                          .cbClsExtra    = has_opts ? opts->class_extra_bytes : 0,
                          .cbWndExtra    = has_opts ? opts->wnd_extra_bytes : 0,
                          .hInstance     = has_opts ? opts->instance : ::GetModuleHandle(nullptr),
                          .hIcon         = has_opts ? opts->icon : ::LoadIcon(nullptr, IDI_APPLICATION),
                          .hCursor       = has_opts ? opts->cursor : ::LoadCursor(nullptr, IDC_ARROW),
                          .hbrBackground = has_opts ? opts->background : GetStockBrush(WHITE_BRUSH),
                          .lpszMenuName  = has_opts ? opts->menu : 0,
                          .lpszClassName = name,
                          .hIconSm       = has_opts ? opts->icon_small : ::LoadIcon(nullptr, IDI_APPLICATION) };

    ATOM window_class = ::RegisterClassEx(&wcx);
    if (window_class == 0) WINAPPY_RETURN_LAST_ERROR("try_register_class_ex failed due to ::RegisterClassEx error");

    return { window_class };
}

result<> Window::API::try_unregister_class(const atoms::arg window_class, const HINSTANCE instance)
{
    WINAPPY_RETURN_LAST_ERROR_IF_FALSE(::UnregisterClass(window_class, instance));
    return {};
}

result<HWND> Window::API::try_create_window(const atoms::arg window_class,
                                            const DWORD style,
                                            const DWORD ex_style,
                                            const LPCWSTR window_name,
                                            const std::optional<Window::CreationOpts>& opts)
{
    assert(window_name);

    const bool has_opts = opts.has_value();
    HWND wnd            = ::CreateWindowEx(ex_style,
                                window_class,
                                window_name,
                                style,
                                has_opts ? opts->x : CW_USEDEFAULT,
                                has_opts ? opts->y : CW_USEDEFAULT,
                                has_opts ? opts->width : CW_USEDEFAULT,
                                has_opts ? opts->height : CW_USEDEFAULT,
                                has_opts && opts->parent ? opts->parent->handle() : nullptr,
                                has_opts ? opts->menu : nullptr,
                                has_opts ? opts->instance : ::GetModuleHandle(nullptr),
                                has_opts ? opts->param : nullptr);

    WINAPPY_RETURN_LAST_ERROR_IF_NULL(wnd);

    return { wnd };
}

Window Window::API::get_desktop_window()
{
    auto wnd = ::GetDesktopWindow();
    ENSURE(wnd);
    return Window(wnd);
}

result<DC> Window::API::try_begin_paint(Window& w, PAINTSTRUCT& ps)
{
    HDC dc = ::BeginPaint(w.handle(), &ps);
    WINAPPY_RETURN_LAST_ERROR_IF_NULL(dc);

    return { { dc } };
}

void Window::API::end_paint(Window& w, PAINTSTRUCT& ps)
{
    [[maybe_unused]] const BOOL result = ::EndPaint(w.handle(), &ps);
    assert(result);  // result should be always non-zero
}

result<rectangle> Window::API::try_get_client_rect(const Window& w)
{
    RECT rect;
    WINAPPY_RETURN_LAST_ERROR_IF_FALSE(::GetClientRect(w.handle(), &rect));

    return { { .left_top = { rect.left, rect.top }, .right_bottom = { rect.right, rect.bottom } } };
}

result<size_t> Window::API::try_get_id(const Window& w)
{
    LONG_PTR id = ::GetWindowLongPtr(w.handle(), GWLP_ID);
    if (id == 0 && ::GetLastError() != 0) WINAPPY_RETURN_LAST_ERROR("try_get_id failed due to ::GetWindowLongPtr(GWLP_ID) error");

    return { static_cast<size_t>(id) };
}

result<HINSTANCE> Window::API::try_get_instance(const Window& w)
{
    LONG_PTR instance = ::GetWindowLongPtr(w.handle(), GWLP_HINSTANCE);
    if (instance == 0 && ::GetLastError() != 0) WINAPPY_RETURN_LAST_ERROR("try_get_instance failed due to ::GetWindowLongPtr(GWLP_HINSTANCE) error");

    return { reinterpret_cast<HINSTANCE>(instance) };
}

result<WNDPROC> Window::API::try_set_wndproc(Window& w, WNDPROC wnd_proc)
{
    LONG_PTR prev_wnd_proc = ::SetWindowLongPtr(w.handle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc));
    if (prev_wnd_proc == 0 && ::GetLastError() != 0) WINAPPY_RETURN_LAST_ERROR("try_set_wndproc failed due to ::GetWindowLongPtr(GWLP_WNDPROC) erro");

    return { reinterpret_cast<WNDPROC>(prev_wnd_proc) };
}

result<> Window::API::try_set_window_pos(Window& w, const rectangle& rect, const UINT flags)
{
    const auto width  = rect.right_bottom.x - rect.left_top.x;
    const auto height = rect.right_bottom.y - rect.left_top.y;

    BOOL success = ::SetWindowPos(w.handle(), nullptr, rect.left_top.x, rect.left_top.y, width, height, flags);

    if (!success) WINAPPY_RETURN_LAST_ERROR("try_set_window_pos failed due to SetWindowPos error");

    return WINAPPY_SUCCESS;
}

result<> Window::API::try_move_window(Window& w, const rectangle& rect, const bool repaint)
{
    const auto width  = rect.right_bottom.x - rect.left_top.x;
    const auto height = rect.right_bottom.y - rect.left_top.y;

    BOOL success = ::MoveWindow(w.handle(), rect.left_top.x, rect.left_top.y, width, height, repaint);

    if (!success) WINAPPY_RETURN_LAST_ERROR("try_move_window failed due to ::MoveWindow error");

    return WINAPPY_SUCCESS;
}

}  // namespace winappy
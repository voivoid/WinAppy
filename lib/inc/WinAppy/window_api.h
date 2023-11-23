#pragma once

#include "WinAppy/atom.h"
#include "WinAppy/error.h"
#include "WinAppy/geo.h"
#include "WinAppy/window.h"

#include <optional>

#include <windef.h>

namespace winappy
{

class DC;

struct Window::API
{
    Window get_desktop_window();

    static result<DC> try_begin_paint(Window& w, PAINTSTRUCT& ps);
    static void end_paint(Window& w, PAINTSTRUCT& ps);

    static result<rectangle> try_get_client_rect(const Window& w);

    static result<> try_destroy_window(Window& w);
    static result<> try_update_window(Window& w);
    static result<> try_set_window_pos(Window& w, const rectangle& rect, UINT flags);
    static result<> try_move_window(Window& w, const rectangle& rect, bool repaint = true);
    static result<bool> try_show_window(Window& w, int show_mode);


    static result<HWND>
        try_create_window(atoms::arg window_class, DWORD style, DWORD ex_style, LPCWSTR window_name, const std::optional<Window::CreationOpts>& opts = {});

    static result<WNDPROC> try_update_wnd_proc(Window& w, WNDPROC wnd_proc);
    static result<ATOM> try_register_class_ex(UINT style, WNDPROC wnd_proc, LPCWSTR name, const std::optional<Window::ClassOpts>& opts = {});
    static result<> try_unregister_class(atoms::arg window_class, HINSTANCE = nullptr);

    static result<size_t> try_get_id(const Window& w);
    static result<HINSTANCE> try_get_instance(const Window& w);
    static result<WNDPROC> try_set_wndproc(Window& w, WNDPROC wnd_proc);
};

}  // namespace winappy

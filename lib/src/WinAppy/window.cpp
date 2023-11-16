#include "stdafx.h"

#include "WinAppy/atom.h"
#include "WinAppy/dc.h"
#include "WinAppy/window.h"
#include "WinAppy/window_api.h"

#include "WinAppy/scope_exit.h"

#include <functional>

#include <Windows.h>

namespace winappy
{

Window::Window() : m_wnd(nullptr) {}

Window::Window(const HWND wnd) : m_wnd(wnd)
{
    if (wnd) ENSURE(::IsWindow(wnd));
}

SubclassedWindow::SubclassedWindow(const HWND wnd) : Window(wnd) {}
SubclassedWindow::SubclassedWindow() : Window() {}

SubclassedWindow::~SubclassedWindow()
{
    if (handle_ref())
    {
        const BOOL destroyed = ::DestroyWindow(handle());
        ENSURE(destroyed);
    }
}

LRESULT SubclassedWindow::wnd_proc_impl(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    auto result = wnd_proc_subclassed(msg, wParam, lParam);
    if (!result) { result = ::CallWindowProc(m_superclass_wnd_proc, handle(), msg, wParam, lParam); }

    if (msg == WM_NCDESTROY) { handle_ref() = nullptr; }

    return *result;
}

result<> SubclassedWindow::try_create(const atoms::arg window_class,
                                      const DWORD style,
                                      const DWORD ex_style,
                                      const LPCWSTR window_name,
                                      const std::optional<Window::CreationOpts>& window_opts)
{
    assert(handle_ref() == nullptr);

    const auto wnd = Window::API::try_create_window(window_class, style, ex_style, window_name, window_opts);
    WINAPPY_RETURN_IF_ERROR(wnd);

    handle_ref() = *wnd;

    const WNDPROC wnd_proc = m_thunk.generate_plain_call_callback(this, &SubclassedWindow::wnd_proc_impl);
    auto old_wnd_proc      = Window::API::try_set_wndproc(*this, wnd_proc);
    WINAPPY_RETURN_IF_ERROR(old_wnd_proc);

    m_superclass_wnd_proc = *old_wnd_proc;

    return WINAPPY_SUCCESS;
}

CustomWindow::~CustomWindow()
{
    if (handle_ref())
    {
        const BOOL destroyed = ::DestroyWindow(handle());
        ENSURE(destroyed);
    }
}

result<> CustomWindow::try_register_class_and_create(const LPCWSTR class_name,
                                                     const UINT class_style,
                                                     const LPCWSTR window_name,
                                                     const DWORD window_style,
                                                     const DWORD window_ex_style,
                                                     const std::optional<Window::ClassOpts>& class_opts,
                                                     const std::optional<Window::CreationOpts>& window_opts)
{
    assert(handle_ref() == nullptr);
    assert(window_name);
    assert(class_name);

    const auto [ init_wnd_proc, optimized_wnd_proc ] = m_thunk.generate_virtual_call_callbacks(this, &handle_ref());

    const auto atom = Window::API::try_register_class_ex(class_style, init_wnd_proc, class_name, class_opts);
    WINAPPY_RETURN_IF_ERROR(atom);

    const auto wnd = Window::API::try_create_window(*atom, window_style, window_ex_style, window_name, window_opts);
    WINAPPY_RETURN_IF_ERROR(wnd);

    assert(*wnd == handle());

    WINAPPY_RETURN_IF_ERROR(API::try_update_wnd_proc(*this, optimized_wnd_proc));

    return WINAPPY_SUCCESS;
}

LRESULT CALLBACK CustomWindow::wnd_proc(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    std::optional<LRESULT> result;
    const HWND hwnd = handle();

    try
    {
        switch (msg)
        {
            case WM_CREATE:
            {
                result = on_create();
                break;
            }
            case WM_NCCREATE:
            {
                result = on_pre_create();
                break;
            }
            case WM_DESTROY:
            {
                result = on_destroy();
                break;
            }
            case WM_NCDESTROY:
            {
                result       = on_post_destroy();
                handle_ref() = nullptr;
                break;
            }
            case WM_COMMAND:
            {
                result = on_command();
                break;
            }
            case WM_PAINT:
            {
                result = on_paint();
                break;
            }
        }
    }
    catch (const std::exception&)
    {
        // TODO: log exception
    }

    if (result) return *result;
    return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

std::optional<LRESULT> CustomWindow::on_paint()
{
    PAINTSTRUCT ps;

    auto dc = Window::API::try_begin_paint(*this, ps);
    assert(dc);

    if (dc)
    {
        WINAPPY_ON_SCOPE_EXIT(([ this, &ps ] { Window::API::end_paint(*this, ps); }));
        paint_proc(*dc);
    }
    else
    {
        // TODO: log failure
    }

    return 0;
}

}  // namespace winappy
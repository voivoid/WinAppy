#include "stdafx.h"

#include "WinAppy/atom.h"
#include "WinAppy/dc.h"
#include "WinAppy/layout.h"
#include "WinAppy/log.h"
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
        [[maybe_unused]] const BOOL destroyed = ::DestroyWindow(handle());
        assert(destroyed);
    }
}

LRESULT SubclassedWindow::wnd_proc_impl(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    auto result = wnd_proc_subclassed(msg, wParam, lParam);
    if (!result) { result = ::CallWindowProc(m_superclass_wnd_proc, handle(), msg, wParam, lParam); }

    if (msg == WM_NCDESTROY) { handle_ref() = nullptr; }

    return *result;
}

result<> SubclassedWindow::try_create(const atoms::arg window_class, const DWORD style, const LPCWSTR text, Window& parent)
{
    assert(handle_ref() == nullptr);

    const auto wnd = Window::API::try_create_window(window_class, style, 0, text, { { .parent = &parent } });
    WINAPPY_RETURN_IF_ERROR(wnd);

    handle_ref() = *wnd;

    const auto wnd_proc = m_thunk.generate_plain_call_callback(this, &SubclassedWindow::wnd_proc_impl);
    WINAPPY_RETURN_IF_ERROR(wnd_proc);

    auto old_wnd_proc = Window::API::try_set_wndproc(*this, *wnd_proc);
    WINAPPY_RETURN_IF_ERROR(old_wnd_proc);

    m_superclass_wnd_proc = *old_wnd_proc;

    return WINAPPY_SUCCESS;
}

CustomWindow::~CustomWindow()
{
    if (handle_ref())
    {
        [[maybe_unused]] const BOOL destroyed = ::DestroyWindow(handle());
        assert(destroyed);
    }
}

result<> CustomWindow::try_register_class_and_create_window(const LPCWSTR class_name_to_register,
                                                            const LPCWSTR window_name,
                                                            const UINT class_style,
                                                            const DWORD window_style,
                                                            const DWORD window_ex_style,
                                                            const std::optional<Window::ClassOpts>& class_opts,
                                                            const std::optional<Window::CreationOpts>& window_opts)
{
    assert(handle_ref() == nullptr);
    assert(window_name);
    assert(class_name_to_register);

    const auto wndprocs = m_thunk.generate_virtual_call_callbacks(this, &handle_ref());
    WINAPPY_RETURN_IF_ERROR(wndprocs);

    const auto atom = Window::API::try_register_class_ex(class_style, wndprocs->initial, class_name_to_register, class_opts);
    WINAPPY_RETURN_IF_ERROR(atom);

    const auto wnd = Window::API::try_create_window(*atom, window_style, window_ex_style, window_name, window_opts);
    WINAPPY_RETURN_IF_ERROR(wnd);

    assert(*wnd == handle());

    WINAPPY_RETURN_IF_ERROR(API::try_update_wnd_proc(*this, wndprocs->optimized));

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
            case WM_COMMAND:
            {
                result = try_reflect_notification(msg, wParam, lParam);

                if (!result) result = on_command();
                break;
            }
            case WM_PAINT:
            {
                result = on_paint();
                break;
            }
            case WM_SIZE:
            {
                const auto new_width  = LOWORD(lParam);
                const auto new_height = HIWORD(lParam);
                result                = on_size(new_width, new_height);
                update_layout(new_width, new_height);
                break;
            }
            case WM_PARENTNOTIFY:
            {
                const auto event = LOWORD(wParam);
                switch (event)
                {
                    case WM_CREATE: handle_child_creation(reinterpret_cast<HWND>(lParam)); break;
                    case WM_DESTROY: handle_child_destruction(reinterpret_cast<HWND>(lParam)); break;
                }
                result = on_parent_notify();
                break;
            }
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
            case WM_CLOSE:
            {
                result = on_close();
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
        }
    }
    catch (const std::exception& ex)
    {
        WINAPPY_ERROR_MESSAGE << "CustomWindow::wnd_proc has caught an exception; Exception details: " << ex.what();
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
        WINAPPY_ON_SCOPE_EXIT
        {
            Window::API::end_paint(*this, ps);
        };

        paint_proc(*dc);
    }
    else { WINAPPY_ERROR_MESSAGE << "CustomWindow::on_paint got a unexpected BeginPaint error"; }

    return 0;
}

Layout* CustomWindow::set_layout(Layout& new_layout)
{
    auto old_layout = m_layout;
    m_layout        = &new_layout;
    return old_layout;
}

Layout* CustomWindow::get_layout() const
{
    return m_layout;
}

void CustomWindow::clear_layout()
{
    m_layout = nullptr;
}

void CustomWindow::update_layout(const int new_width, const int new_height)
{
    if (m_layout) { m_layout->update(new_width, new_height); }
}

void CustomWindow::handle_child_creation(const HWND /*child*/) {}

void CustomWindow::handle_child_destruction(const HWND /*child*/) {}

std::optional<LRESULT> CustomWindow::try_reflect_notification(const UINT msg, const WPARAM wParam, const LPARAM lParam)
{
    if (!m_layout) return {};

    return m_layout->try_reflect_notification(msg, wParam, lParam);
}

}  // namespace winappy
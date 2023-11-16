#pragma once

#include "WinAppy/atom.h"
#include "WinAppy/error.h"
#include "WinAppy/thunk.h"

#include <optional>

#include <windef.h>

namespace winappy
{
class DC;
class Window
{
  public:
    Window(HWND m_wnd);

    struct API;
    struct ClassOpts
    {
        int class_extra_bytes = 0;
        int wnd_extra_bytes   = 0;
        HINSTANCE instance    = {};
        HICON icon            = {};
        HICON icon_small      = {};
        HCURSOR cursor        = {};
        HBRUSH background     = {};
        LPCWSTR menu          = {};
    };
    struct CreationOpts
    {
        int x              = CW_USEDEFAULT;
        int y              = CW_USEDEFAULT;
        int width          = CW_USEDEFAULT;
        int height         = CW_USEDEFAULT;
        Window* parent     = {};
        HMENU menu         = {};
        HINSTANCE instance = {};
        LPVOID param       = {};
    };

  protected:
    Window();

    // ensures the handle is set
    HWND handle() const
    {
        ENSURE(m_wnd);
        return m_wnd;
    }

    // the handle may be not set
    HWND& handle_ref()
    {
        return m_wnd;
    }

  private:
    HWND m_wnd;
};

class SubclassedWindow : public Window
{
  public:
    SubclassedWindow(HWND wnd);
    ~SubclassedWindow();

    result<> try_create(atoms::arg window_class, DWORD style, DWORD ex_style, LPCWSTR window_name, const std::optional<Window::CreationOpts>& window_opts = {});

  protected:
    SubclassedWindow();

  private:
    LRESULT CALLBACK wnd_proc_impl(const UINT msg, const WPARAM wParam, const LPARAM lParam);

    virtual std::optional<LRESULT> CALLBACK wnd_proc_subclassed(const UINT /*msg*/, const WPARAM /*wParam*/, const LPARAM /*lParam*/)
    {
        return {};
    };


  private:
    Thunk m_thunk;
    WNDPROC m_superclass_wnd_proc = nullptr;
};

class CustomWindow : public Window
{
    virtual LRESULT CALLBACK wnd_proc(const UINT msg, const WPARAM wParam, const LPARAM lParam);
    virtual void paint_proc(DC&){};

  public:
    ~CustomWindow();

    result<> try_register_class_and_create(LPCWSTR class_name,
                                           UINT class_style,
                                           LPCWSTR window_name,
                                           DWORD window_style                                     = WS_OVERLAPPEDWINDOW,
                                           DWORD window_ex_style                                  = WS_EX_OVERLAPPEDWINDOW,
                                           const std::optional<Window::ClassOpts>& class_opts     = {},
                                           const std::optional<Window::CreationOpts>& window_opts = {});

  private:
    // clang-format off
    virtual std::optional<LRESULT> on_pre_create() { return {}; }  // WM_NCCREATE
    virtual std::optional<LRESULT> on_create() { return {};}       // WM_CREATE    
    virtual std::optional<LRESULT> on_paint();                     // WM_PAINT;
    virtual std::optional<LRESULT> on_command() { return {};};     // WM_COMMAND;
    virtual std::optional<LRESULT> on_destroy() { return 0;}       // WM_DESTROY;
    virtual std::optional<LRESULT> on_post_destroy() { return 0;}  // WM_NCDESTROY;
    // clang-format on

  private:
    Thunk m_thunk;
};

}  // namespace winappy
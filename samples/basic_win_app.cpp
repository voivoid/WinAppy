#include "WinAppy/button.h"
#include "WinAppy/dc.h"
#include "WinAppy/gdi.h"
#include "WinAppy/msg_loop.h"
#include "WinAppy/window.h"
#include "WinAppy/window_api.h"

#include "WinAppy/log.h"

#include <Windows.h>

#include <cassert>

namespace
{
class MyWindow : public winappy::CustomWindow
{
  public:
    MyWindow() : m_btn(*this) {}

  private:
    virtual std::optional<LRESULT> on_create() override
    {
        m_btn.create(L"PRESS ME!");
        m_btn.on_click([ this ]() { ::MessageBoxW(handle(), L"", L"", MB_OK); });
        return 0;
    }

    virtual void paint_proc(winappy::DC& /*dc*/) override
    {
        /*const auto rect = winappy::Window::API::try_get_client_rect(*this);
        assert(rect);

        using GDI = winappy::gdi::API;

        GDI::rectangle(dc, *rect);

        GDI::move_to(dc, rect->get_left_top());
        GDI::line_to(dc, rect->get_right_bottom());

        GDI::move_to(dc, rect->get_right_top());
        GDI::line_to(dc, rect->get_left_bottom());*/
    }

    virtual std::optional<LRESULT> on_destroy() override
    {
        ::PostQuitMessage(0);
        return 0;
    }

  private:
    winappy::Controls::Button m_btn;
};
}  // namespace

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    WINAPPY_WARNING_MESSAGE << "Hello, world" << 10 << 20;
    MyWindow wnd;
    const auto created = wnd.try_register_class_and_create(L"CoolWindowClass", CS_HREDRAW | CS_VREDRAW, L"CoolWindow");
    assert(created);

    using WinApi = winappy::Window::API;

    WinApi::try_update_window(wnd);
    WinApi::try_show_window(wnd, SW_SHOW);

    winappy::run_message_loop();

    return 0;
}
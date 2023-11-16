#include "stdafx.h"

#include "WinAppy/button.h"
#include "WinAppy/window_api.h"

namespace winappy
{
namespace Controls
{
Button::Button(Window& parent) : m_parent(parent) {}

result<> Button::create(const LPCWSTR text)
{
    const auto instance = Window::API::try_get_instance(m_parent);
    WINAPPY_RETURN_IF_ERROR(instance);

    const auto created = try_create(L"BUTTON",
                                    WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                    0,
                                    text,
                                    { Window::CreationOpts{ .x = 10, .y = 10, .width = 100, .height = 100, .parent = &m_parent, .instance = *instance } });
    WINAPPY_RETURN_IF_ERROR(created);

    return WINAPPY_SUCCESS;
}

void Button::set_text(const LPCWSTR text)
{
    const LRESULT success = ::SendMessage(handle(), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
    assert(success);
}

std::wstring Button::get_text() const
{
    std::wstring text_buff(4, '\0');

    size_t chars_copied = 0;
    while (true)
    {
        chars_copied = ::SendMessage(handle(), WM_GETTEXT, text_buff.size(), reinterpret_cast<LPARAM>(text_buff.data()));
        if (chars_copied != text_buff.size() - 1) { break; }

        text_buff.resize(text_buff.size() * 2);
    }

    text_buff.resize(chars_copied);  // drop zero terminator
    return text_buff;
}

void Button::on_click(std::function<void(void)>) {}

}  // namespace Controls
}  // namespace winappy
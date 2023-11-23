#include "stdafx.h"

#include "WinAppy/button.h"
#include "WinAppy/error.h"
#include "WinAppy/window_api.h"

namespace winappy
{
namespace Controls
{
Button::Button(Window& parent) : m_parent(parent) {}

result<> Button::create(const LPCWSTR text)
{
    const auto created = try_create(L"BUTTON", WS_CHILD | BS_DEFPUSHBUTTON, text, m_parent);
    WINAPPY_RETURN_IF_ERROR(created);

    return WINAPPY_SUCCESS;
}

void Button::set_text(const LPCWSTR text)
{
    [[maybe_unused]] const LRESULT success = ::SendMessage(handle(), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text));
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

void Button::on_click(std::function<void(void)> handler)
{
    m_on_click_handler = std::move(handler);
}

std::optional<LRESULT> Button::try_reflect_notification(const UINT msg, const WPARAM wParam, [[maybe_unused]] const LPARAM lParam)
{
    switch (msg)
    {
        case WM_COMMAND:
        {
            assert(LOWORD(wParam) == get_id());
            assert(reinterpret_cast<HWND>(lParam) == handle());

            switch (HIWORD(wParam))
            {
                case BN_CLICKED: on_click(); return 0;
            }
            break;
        }
    }
    return {};
}

void Button::on_click()
{
    if (m_on_click_handler) m_on_click_handler();
}

}  // namespace Controls
}  // namespace winappy
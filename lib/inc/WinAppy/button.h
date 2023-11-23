#pragma once

#include "WinAppy/control.h"

#include <functional>
#include <optional>
#include <string>

namespace winappy
{

class Window;
namespace Controls
{

class Button : public Control
{
  public:
    Button(Window& parent);

    result<> create(LPCWSTR text);

    void set_text(LPCWSTR text);
    std::wstring get_text() const;

    void on_click(std::function<void(void)>);

  private:
    std::optional<LRESULT> try_reflect_notification(UINT msg, WPARAM wParam, LPARAM lParam) override;

    void on_click();

  private:
    Window& m_parent;

    std::function<void(void)> m_on_click_handler;
};
}  // namespace Controls
}  // namespace winappy
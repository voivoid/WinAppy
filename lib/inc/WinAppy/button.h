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
    Window& m_parent;
};
}  // namespace Controls
}  // namespace winappy
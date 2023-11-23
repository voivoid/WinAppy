#pragma once

#include "WinAppy/error.h"
#include "WinAppy/window.h"

namespace winappy
{
class Layout;
class Control : public SubclassedWindow
{
    friend class Layout;

  public:
    result<size_t> get_id() const;

  private:
    virtual std::optional<LRESULT> try_reflect_notification(UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};
}  // namespace winappy
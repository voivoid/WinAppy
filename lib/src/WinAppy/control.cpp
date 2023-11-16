#include "stdafx.h"

#include "WinAppy/control.h"
#include "WinAppy/window_api.h"

namespace winappy
{
result<size_t> Control::get_id() const
{
    return Window::API::try_get_id(*this);
}
}  // namespace winappy
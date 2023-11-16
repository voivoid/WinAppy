#pragma once

#include "WinAppy/error.h"

#include <windef.h>

namespace winappy
{
namespace gdi
{
struct API;
}

class DC
{
    friend struct gdi::API;

  public:
    DC(HDC dc);

  private:
    HDC handle() const
    {
        ENSURE(m_dc);
        return m_dc;
    }

  private:
    HDC m_dc;
};
}  // namespace winappy
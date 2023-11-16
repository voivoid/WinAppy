#pragma once

#include "WinAppy/error.h"
#include "WinAppy/window.h"

namespace winappy
{
class Control : public SubclassedWindow
{
  public:
    result<size_t> get_id() const;
};
}  // namespace winappy
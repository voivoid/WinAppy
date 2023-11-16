#pragma once

#include <cstdint>
#include <optional>

#include "winappy/error.h"

namespace winappy
{
class clipboard
{
  public:
    ~clipboard();
    clipboard(const clipboard&) = delete;
    clipboard(clipboard&&);
    clipboard& operator=(clipboard&) = delete;
    clipboard& operator=(clipboard&&);

    static result<clipboard> open();
    static std::optional<std::uint32_t> get_clipboard_seq_number();

  private:
    clipboard();

  private:
    bool m_opened = true;
};
}  // namespace winappy
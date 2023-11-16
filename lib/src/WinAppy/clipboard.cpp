#include "winappy/clipboard.h"

#include <cassert>
#include <windows.h>

namespace winappy
{

clipboard::clipboard() = default;

clipboard::clipboard(clipboard&& rhs)
{
    rhs.m_opened = false;
}

clipboard& clipboard::operator=(clipboard&& rhs)
{
    rhs.m_opened = false;
    return *this;
}

result<clipboard> clipboard::open()
{
    const auto opened = ::OpenClipboard(NULL);
    if (!opened) WINAPPY_RETURN_LAST_ERROR("::OpenClipboard failed");

    return clipboard{};
}

clipboard::~clipboard()
{
    if (m_opened)
    {
        BOOL closed = ::CloseClipboard();
        assert(closed);
    }
}

std::optional<std::uint32_t> clipboard::get_clipboard_seq_number()
{
    const auto seq_num = GetClipboardSequenceNumber();
    if (seq_num == 0) return {};

    return seq_num;
}

}  // namespace winappy
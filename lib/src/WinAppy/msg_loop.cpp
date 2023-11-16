#include "stdafx.h"

#include "WinAppy/msg_loop.h"

#include "WinAppy/error.h"

namespace winappy
{
int run_message_loop()
{
    MSG msg{};
    BOOL ret{};
    while (ret = ::GetMessage(&msg, nullptr, 0, 0), ret > 0)
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    ENSURE(ret != -1);

    return static_cast<int>(msg.wParam);
}
}  // namespace winappy
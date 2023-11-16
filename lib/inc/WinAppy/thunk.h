#pragma once

#include <memory>
#include <utility>

#include <windef.h>

namespace winappy
{
class Thunk
{
  public:
    // This function returns WNDPROC pointers. The first WNDPROC implementation: 1) initializes wnd_ptr 2) does a trampoline virtual call.
    // Since wnd_ptr needs to be initialized only once there is no need to execute 'mov' instructions every time WNDPROC is called.
    // So a second WNDPROC is provided and the only thing it does are virtual calls. Thus there is no more redundant initialization overhead.
    // Subclass your window to use second WNDPROC as soon as the window is created.
    template <typename T>
    std::pair<WNDPROC, WNDPROC> generate_virtual_call_callbacks(const T* window, HWND* wnd_ptr);

    template <typename T>
    WNDPROC generate_plain_call_callback(const T* window, LRESULT (T::*f)(UINT, WPARAM, LPARAM));

  private:
    struct free_trampoline
    {
        void operator()(void*) const;
    };
    std::unique_ptr<void, free_trampoline> m_trampoline;
};
}  // namespace winappy
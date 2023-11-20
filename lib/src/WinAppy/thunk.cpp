#include "stdafx.h"

#include "WinAppy/error.h"
#include "WinAppy/log.h"
#include "WinAppy/thunk.h"
#include "WinAppy/window.h"

#include <windows.h>

namespace
{
constexpr std::byte operator"" _b(unsigned long long b)
{
    assert(b <= 255);
    return std::byte(b);
}
}  // namespace

namespace winappy
{
void Thunk::free_trampoline::operator()(void* const p) const
{
    const BOOL mem_freed = ::VirtualFree(p, 0, MEM_RELEASE);
    if (!mem_freed)
    {
        const auto err = ::GetLastError();
        WINAPPY_ERROR_MESSAGE << "::VirtualFree failed in Thunk::free_trampoline::operator() with " << err << " error code";
    }
}

template <typename T>
result<WNDPROC> Thunk::generate_plain_call_callback(const T* const window, LRESULT (T::*f)(UINT, WPARAM, LPARAM))
{
#ifdef _M_X64
    static const std::byte function_code[] = {
        // clang-format off
        0x48_b, 0xB8_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,  // movabs rax, &f
        0x48_b, 0xB9_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,  // movabs rcx, &window
        0xff_b, 0xE0_b                                                                   // jmp rax
        // clang-format on
    };

    std::byte* code = reinterpret_cast<std::byte*>(::VirtualAlloc(NULL, sizeof(function_code), MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    WINAPPY_RETURN_LAST_ERROR_IF_ZERO(code);

    m_trampoline.reset(code);

    memcpy(code, function_code, sizeof(function_code));
    memcpy(code + 2, &f, sizeof(void*));
    memcpy(code + 12, &window, sizeof(void*));

    WINAPPY_THROW_LAST_ERROR_IF_FALSE(::FlushInstructionCache(::GetCurrentProcess(), NULL, 0));

    return { reinterpret_cast<WNDPROC>(code) };
#else
#    error unsupported platform
#endif
}

template <typename T>
result<Thunk::generated_wndprocs> Thunk::generate_virtual_call_callbacks(const T* const window, HWND* const wnd_ptr)
{
#ifdef _M_X64
    static const std::byte function_code[] = {
        // clang-format off
        0x48_b, 0xB8_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,  // movabs rax, &wnd_ptr
        0x48_b, 0x89_b, 0x08_b,                                                          // mov QWORD PTR [rax],rcx
        
        0x48_b, 0xB9_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b,  // movabs rcx, &window
        0x48_b, 0x8B_b, 0x01_b,                                                          // mov rax,[rcx]
        0xff_b, 0x20_b                                                                   // jmp [rax] ( virtual call )
        // clang-format on
    };

    std::byte* code = reinterpret_cast<std::byte*>(::VirtualAlloc(NULL, sizeof(function_code), MEM_COMMIT, PAGE_EXECUTE_READWRITE));
    WINAPPY_RETURN_LAST_ERROR_IF_ZERO(code);

    m_trampoline.reset(code);

    memcpy(code, function_code, sizeof(function_code));
    memcpy(code + 2, &wnd_ptr, sizeof(void*));
    memcpy(code + 15, &window, sizeof(void*));

    WINAPPY_RETURN_LAST_ERROR_IF_FALSE(::FlushInstructionCache(::GetCurrentProcess(), NULL, 0));

    return { { reinterpret_cast<WNDPROC>(code), reinterpret_cast<WNDPROC>(code + 13) } };
#else
#    error unsupported platform
#endif
}

}  // namespace winappy

template winappy::result<WNDPROC> winappy::Thunk::generate_plain_call_callback(const SubclassedWindow*, LRESULT (SubclassedWindow::*)(UINT, WPARAM, LPARAM));
template winappy::result<winappy::Thunk::generated_wndprocs> winappy::Thunk::generate_virtual_call_callbacks(const CustomWindow*, HWND*);
#pragma once

#include "WinAppy/macros.h"

#include <utility>

namespace winappy
{

template <typename F>
struct scope_exit_action
{
    scope_exit_action(F f) : m_f(std::move(f)) {}

    ~scope_exit_action()
    {
        m_f();
    }

    F m_f;
};

struct scope_exit_helper
{
    template <typename F>
    scope_exit_action<F> operator%(F f)
    {
        return { std::move(f) };
    }
};

#define WINAPPY_ON_SCOPE_EXIT auto WINAPPY_UNIQUE_NAME(on_scope_exit_) = scope_exit_helper{} % [ & ]


}  // namespace winappy
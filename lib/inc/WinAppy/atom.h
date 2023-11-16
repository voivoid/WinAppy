#pragma once

#include <cstdint>
#include <optional>

#include "winappy/error.h"

#include <minwindef.h>


namespace winappy
{

namespace atoms
{

namespace global
{

result<ATOM> add_atom(LPCWSTR string);
result<ATOM> add_atom(std::uint16_t n);
result<> delete_atom(ATOM atom);
std::optional<ATOM> find_atom(LPCWSTR string);
std::optional<std::wstring> get_atom_string(const ATOM atom);

}  // namespace global

result<ATOM> add_atom(LPCWSTR string);
result<ATOM> add_atom(std::uint16_t n);
result<> delete_atom(ATOM atom);
std::optional<ATOM> find_atom(LPCWSTR string);
std::optional<std::wstring> get_atom_string(const ATOM atom);

struct arg
{
    arg(LPCWSTR str) : m_str(str) {}
    arg(ATOM atom) : m_str((LPCWSTR)MAKEINTATOM(atom)) {}

    operator LPCWSTR() const
    {
        return m_str;
    }

    LPCWSTR m_str;
};

}  // namespace atoms
}  // namespace winappy
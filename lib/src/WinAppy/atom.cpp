#include "winappy/atom.h"

#include "WinAppy/error.h"

#include <windows.h>

namespace winappy
{
namespace atoms
{
namespace details
{

template <bool global = false>
result<ATOM> add_atom(const LPCWSTR string)
{
    assert(string);

    constexpr auto* add_atom = global ? ::GlobalAddAtomW : ::AddAtomW;
    ATOM ATOM                = add_atom(string);
    if (ATOM == 0) WINAPPY_RETURN_LAST_ERROR(global ? "add_atom failed due to ::GlobalAddAtomW error" : "add_atom failed due to ::AddAtomW error");

    return { ATOM };
}

template <bool global = false>
result<ATOM> add_atom(const std::uint16_t n)
{
    if (n == 0 || n >= MAXINTATOM) throw std::invalid_argument("invalid integer atom value");  // TODO: get rid of exception

    return add_atom<global>(MAKEINTATOM(n));
}

template <bool global = false>
result<> delete_atom(const ATOM ATOM)
{
    constexpr auto* delete_atom = global ? ::GlobalDeleteAtom : ::DeleteAtom;
    const bool deleted          = delete_atom(ATOM) == 0;
    if (!deleted) WINAPPY_RETURN_LAST_ERROR(global ? "delete_atom failed due to ::GlobalDeleteAtom error" : "delete_atom failed due to ::DeleteAtom error");

    return WINAPPY_SUCCESS;
}

template <bool global = false>
std::optional<ATOM> find_atom(const LPCWSTR string)
{
    assert(string);

    constexpr auto* find_atom = global ? ::GlobalFindAtomW : ::FindAtomW;
    ATOM ATOM                 = find_atom(string);
    if (ATOM == 0) return {};

    return { ATOM };
}

enum class get_atom_error
{
    not_found,
    not_enough_buffer
};

template <bool global = false>
std::optional<std::wstring> get_atom_string(const ATOM ATOM)
{
    constexpr auto* get_atom_name = global ? ::GlobalGetAtomNameW : ::GetAtomNameW;

    WCHAR buffer[ 256 ];
    const auto chars_copied = get_atom_name(ATOM, buffer, _countof(buffer));
    if (chars_copied == 0) return {};

    return std::wstring(buffer, chars_copied);
}

}  // namespace details

result<> init_atom_table(const std::uint32_t buckets)
{
    const bool initialized = ::InitAtomTable(buckets) != 0;
    if (!initialized) WINAPPY_RETURN_LAST_ERROR("init_atom_table failed due to ::InitAtomTable error");

    return WINAPPY_SUCCESS;
}

namespace global
{

result<ATOM> add_atom(const LPCWSTR string)
{
    return details::add_atom<true>(string);
}

result<ATOM> add_atom(const std::uint16_t n)
{
    return details::add_atom<true>(n);
}

result<> delete_atom(const ATOM ATOM)
{
    return details::delete_atom<true>(ATOM);
}

std::optional<ATOM> find_atom(const LPCWSTR string)
{
    return details::find_atom<true>(string);
}

std::optional<std::wstring> get_atom_string(const ATOM ATOM)
{
    return details::get_atom_string<true>(ATOM);
}

}  // namespace global

result<ATOM> add_atom(const LPCWSTR string)
{
    return details::add_atom(string);
}

result<ATOM> add_atom(const std::uint16_t n)
{
    return details::add_atom(n);
}

result<> delete_atom(const ATOM ATOM)
{
    return details::delete_atom(ATOM);
}

std::optional<ATOM> find_atom(const LPCWSTR string)
{
    return details::find_atom(string);
}

std::optional<std::wstring> get_atom_string(const ATOM ATOM)
{
    return details::get_atom_string(ATOM);
}

}  // namespace atoms
}  // namespace winappy

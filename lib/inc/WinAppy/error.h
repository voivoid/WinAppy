#pragma once

#include <cassert>
#include <expected>
#include <string>
#include <system_error>
#include <type_traits>

#include <windows.h>

namespace winappy
{

struct error
{
    std::error_code code;
    std::string description;
};

std::error_code get_last_error_code();
std::unexpected<error> make_last_error(const char* const description);

template <typename T = void>
using result = std::expected<T, error>;

template <typename T>
struct is_result_t : std::false_type
{
};

template <typename T>
struct is_result_t<result<T>> : std::true_type
{
};

}  // namespace winappy

#define WINAPPY_RETURN_IF_ERROR(expected)                                                                                                                      \
    if constexpr (winappy::is_result_t<std::remove_cvref_t<decltype(expected)>>::value)                                                                        \
    {                                                                                                                                                          \
        if (!(expected)) { return std::unexpected(std::move((expected).error())); }                                                                            \
    }                                                                                                                                                          \
    else                                                                                                                                                       \
    {                                                                                                                                                          \
        if (auto result = (expected); !result.has_value()) { return std::unexpected(std::move(result.error())); }                                              \
    }

#define WINAPPY_SUCCESS                                                                                                                                        \
    result<> {}
#define WINAPPY_RETURN_LAST_ERROR(descr) return winappy::make_last_error(descr);
#define WINAPPY_RETURN_LAST_ERROR_IF_FALSE(expr)                                                                                                               \
    if (!(expr)) WINAPPY_RETURN_LAST_ERROR(#expr " failed")
#define WINAPPY_RETURN_LAST_ERROR_IF_ZERO WINAPPY_RETURN_LAST_ERROR_IF_FALSE
#define WINAPPY_RETURN_LAST_ERROR_IF_NULL WINAPPY_RETURN_LAST_ERROR_IF_ZERO


#define WINAPPY_THROW_LAST_ERROR()
#define WINAPPY_THROW_LAST_ERROR_IF_FALSE(expr)                                                                                                                \
    if (!(expr)) { WINAPPY_THROW_LAST_ERROR(); }

#define WINAPPY_THROW_LAST_ERROR_IF_ZERO WINAPPY_THROW_LAST_ERROR_IF_FALSE
#define WINAPPY_THROW_LAST_ERROR_IF_NULL WINAPPY_THROW_LAST_ERROR_IF_ZERO



#if defined(ENSURE) && !defined(ENSURE_REDEFINED)
#    error "ENSURE is already defined"
#else
#    ifdef NDEBUG
#        define ENSURE(expr)                                                                                                                                   \
            {                                                                                                                                                  \
                if (!(expr)) { throw std::runtime_error(#expr " failed"); }                                                                                    \
            };
#    else
#        define ENSURE assert
#    endif
#endif

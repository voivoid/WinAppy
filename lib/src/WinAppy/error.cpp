#include "winappy/error.h"

namespace winappy
{

std::error_code get_last_error_code()
{
    const auto error = ::GetLastError();
    return std::error_code(error, std::system_category());
}

std::unexpected<error> make_last_error(const char* const description)
{
    error err{ .code = get_last_error_code(), .description = description };
    return std::unexpected(std::move(err));
}

}  // namespace winappy
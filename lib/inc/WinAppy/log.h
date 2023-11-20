#pragma once

#include "WinAppy/macros.h"

#include <sstream>

namespace winappy
{

enum class trace_level : int
{
    debug = 0,
    warning,
    error
};

void log_message(trace_level level, LPCWSTR msg);
void debug_message(LPCWSTR msg);
void warning_message(LPCWSTR msg);
void error_message(LPCWSTR msg);

void set_trace_level(trace_level level);
trace_level get_trace_level();

#define WINAPPY_DEBUG_LEVEL winappy::trace_level::debug
#define WINAPPY_WARNING_LEVEL winappy::trace_level::warning
#define WINAPPY_ERROR_LEVEL winappy::trace_level::error

class logger
{
  public:
    logger(trace_level log_level) : m_log_level(log_level){};
    ~logger()
    {
        m_ss << L'\n';
        log_message(m_log_level, m_ss.str().c_str());
    }

    template <typename T>
    logger& operator<<(const T& t)
    {
        if (!is_empty()) m_ss << ' ';
        m_ss << t;
        return *this;
    }

    bool is_empty()
    {
        return m_ss.rdbuf()->in_avail();
    }

  private:
    trace_level m_log_level;
    std::wstringstream m_ss;
};

#define WINAPPY_LOG_MESSAGE(LEVEL) winappy::logger(LEVEL)

#define WINAPPY_DEBUG_MESSAGE WINAPPY_LOG_MESSAGE(WINAPPY_DEBUG_LEVEL)
#define WINAPPY_WARNING_MESSAGE WINAPPY_LOG_MESSAGE(WINAPPY_WARNING_LEVEL)
#define WINAPPY_ERROR_MESSAGE WINAPPY_LOG_MESSAGE(WINAPPY_ERROR_LEVEL)

}  // namespace winappy
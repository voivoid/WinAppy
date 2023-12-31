#include "stdafx.h"

#include "WinAppy/error.h"
#include "WinAppy/log.h"

#include <Windows.h>

#include <atomic>

namespace winappy
{
namespace
{
std::atomic<trace_level> g_current_level = trace_level::warning;
}

void set_trace_level(const trace_level level)
{
    g_current_level = level;
}

trace_level get_trace_level()
{
    return g_current_level;
}

void log_message(const trace_level level, const LPCWSTR msg)
{
    if (g_current_level <= level) { ::OutputDebugStringW(msg); }
}

void debug_message(const LPCWSTR msg)
{
    log_message(trace_level::debug, msg);
}

void warning_message(const LPCWSTR msg)
{
    log_message(trace_level::warning, msg);
}

void error_message(const LPCWSTR msg)
{
    log_message(trace_level::error, msg);
}
}  // namespace winappy
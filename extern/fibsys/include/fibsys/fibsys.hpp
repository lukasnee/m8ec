#pragma once

#include "thread.hpp"
#include "ticks.hpp"

namespace fibsys {

using Thread = cpp_freertos::Thread;

std::size_t get_uptime_ticks();
std::size_t get_uptime_ms();

} // namespace fibsys

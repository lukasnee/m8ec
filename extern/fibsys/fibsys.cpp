#include "fibsys/fibsys.hpp"

namespace fibsys {

std::size_t get_uptime_ticks() { return static_cast<std::size_t>(cpp_freertos::Ticks::GetTicks()); }

std::size_t get_uptime_ms() {
    return static_cast<std::size_t>(cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks()));
}
} // namespace fibsys

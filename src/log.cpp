#include "log.hpp"

bool
init_logging(void)
{
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] %^[%l]%$ (%!) [%s:%#] %v");
    
    return true;
}

#include "log.hpp"

bool
init_logging(void)
{
    spdlog::set_pattern("[%H:%M:%S.%e] %^[%l]%$ (%!) [%s:%#] %v");
    
    return true;
}

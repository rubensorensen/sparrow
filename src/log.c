#include "log.h"

static char _log_time_buffer[256];

const char *
_get_log_time_string(void)
{
    get_clock_human_readable(_log_time_buffer, ARRAY_SIZE(_log_time_buffer), "%H:%M:%S");
    return _log_time_buffer;
}


#if defined(linux) || defined(__unix__) || defined(__APPLE__)
#include <string.h>

b32
_check_terminal_supports_ansi_escape_codes(void)
{
    static b32 queried = false;
    static b32 supported = false;

    if (queried) {
        return supported;
    }
    queried = true;

    const char* term = getenv("TERM");
    if (term != NULL && strcmp(term, "dumb") != 0) {
        supported = true;
    } else {
        supported = false;
    }

    return supported;
}

#elif defined(_WIN32) // @TODO: Not implemented

b32
_check_terminal_supports_ansi_escape_codes(void)
{
    return false;
}

#else

b32
_check_terminal_supports_ansi_escape_codes(void)
{
    return false;
}
#endif

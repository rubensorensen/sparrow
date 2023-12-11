#include "core.h"

#include <time.h>

void
get_clock_human_readable(char *buf, size_t buf_size, char *format)
{
    time_t rawtime;
    time(&rawtime);
    struct tm foo;
    struct tm *mytm;
    mytm = localtime_r(&rawtime, &foo);
    strftime(buf, buf_size, format, mytm);
}

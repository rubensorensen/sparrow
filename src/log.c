#include "log.h"

#if defined(linux) || defined(__unix__)
#include <unistd.h>

b32
check_terminal_supports_ansi_escape_codes(void)
{
    static b32 queried = false;
    static b32 supported = false;

    if (queried) {
        return supported;
    } else {
        queried = true;
    }

    int fd[2];
    if (pipe(fd) < 0) {
        return false;
    }

    pid_t pid = fork();
    if (pid < 0) {
        return false;
    }

    if (pid == 0) {
        // Child
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) < 0) {
            return false;
        }
        close(fd[1]);
        if (execl("/bin/tput", "/bin/tput", "colors", NULL) < 0) {
            return false;
        }
    }

    close(fd[1]);
    char buf[100];
    while (read(fd[0], buf, sizeof(buf)) != 0) {};
    if (atoi(buf) == 256) {
        supported = true;
        return true;
    }

    return supported;
}

#elif defined(_WIN32)
// TODO: Not implemented
b32
check_terminal_supports_ansi_escape_codes(void)
{
    return false;
}

#elif defined(__APPLE__)
// TODO: Not implemented
b32
check_terminal_supports_ansi_escape_codes(void)
{
    return false;
}

#else
b32
check_terminal_supports_ansi_escape_codes(void)
{
    return false;
}
#endif

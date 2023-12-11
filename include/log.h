#ifndef LOG_H

#include <stdio.h>
#include <stdlib.h>

// TODO: FILENAME macro has not been tested on Win32.
//       If it leads to compilation issues, just replace it with
//       # define FILENAME __FILE__
//       It will contain the full path instead of just the filename, but it
//       may prove useful anyway
#if defined(_WIN32)
#DEFINE FILEPATH __FILE__
#define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) // https://stackoverflow.com/a/8488201
#define FILELINE __LINE__
#elif defined(linux)
#include <libgen.h>
#define FILEPATH __FILE__
#define FILENAME basename(__FILE__)
#define FILELINE __LINE__
#else
#define FILENAME __FILE__
#define FILELINE __LINE__
#endif

// Ansi color escape codes - Regular
#define ANSI_BLACK   "\033[0;30m"
#define ANSI_RED     "\033[0;31m"
#define ANSI_GREEN   "\033[0;32m"
#define ANSI_YELLOW  "\033[0;33m"
#define ANSI_BLUE    "\033[0;34m"
#define ANSI_MAGENTA "\033[0;35m"
#define ANSI_CYNAN   "\033[0;36m"
#define ANSI_WHITE   "\033[0;37m"

// Ansi color escape codes - Bold
#define ANSI_B_BLACK   "\033[1;30m"
#define ANSI_B_RED     "\033[1;31m"
#define ANSI_B_GREEN   "\033[1;32m"
#define ANSI_B_YELLOW  "\033[1;33m"
#define ANSI_B_BLUE    "\033[1;34m"
#define ANSI_B_MAGENTA "\033[1;35m"
#define ANSI_B_CYAN    "\033[1;36m"
#define ANSI_B_WHITE   "\033[1;37m"

// Ansi color escape codes - Underlined
#define ANSI_U_BLACK   "\033[4;30m"
#define ANSI_U_RED     "\033[4;31m"
#define ANSI_U_GREEN   "\033[4;32m"
#define ANSI_U_YELLOW  "\033[4;33m"
#define ANSI_U_BLUE    "\033[4;34m"
#define ANSI_U_MAGENTA "\033[4;35m"
#define ANSI_U_CYAN    "\033[4;36m"
#define ANSI_U_WHITE   "\033[4;37m"

// Ansi color escape codes - High Intensity
#define ANSI_HI_BLACK   "\033[0;90m"
#define ANSI_HI_RED     "\033[0;91m"
#define ANSI_HI_GREEN   "\033[0;92m"
#define ANSI_HI_YELLOW  "\033[0;93m"
#define ANSI_HI_BLUE    "\033[0;94m"
#define ANSI_HI_MAGENTA "\033[0;95m"
#define ANSI_HI_CYAN    "\033[0;96m"
#define ANSI_HI_WHITE   "\033[0;97m"

//Ansi color escape codes - High Intensity Bold
#define ANSI_HI_B_BLACK   "\033[1;90m"
#define ANSI_HI_B_RED     "\033[1;91m"
#define ANSI_HI_B_GREEN   "\033[1;92m"
#define ANSI_HI_B_YELLOW  "\033[1;93m"
#define ANSI_HI_B_BLUE    "\033[1;94m"
#define ANSI_HI_B_MAGENTA "\033[1;95m"
#define ANSI_HI_B_CYAN    "\033[1;96m"
#define ANSI_HI_B_WHITE   "\033[1;97m"


#define ANSI_NONE ""
#define ANSI_RESET "\033[0m"

#define LOG_BASE(sink,                          \
                 error_name_string, ...) {      \
        fprintf(sink, "[%s] %s:%d - ",          \
                error_name_string,              \
                FILENAME, FILELINE);            \
        fprintf(sink, __VA_ARGS__);             \
        fprintf(sink, "\n");                    \
    }

#define COLORED_LOG_BASE(sink, log_type_string,                 \
                         log_type_color,                        \
                         file_name_color,                       \
                         file_line_color, ...) {                \
        fprintf(sink, "[%s%s%s] %s%s%s:%s%d%s - ",              \
                log_type_color, log_type_string, ANSI_RESET,    \
                file_name_color, FILENAME, ANSI_RESET,          \
                file_line_color, FILELINE, ANSI_RESET);         \
        fprintf(sink, __VA_ARGS__);                             \
        fprintf(sink, "\n");                                    \
    }

#define LOG_TRACE(...) LOG_BASE(stdout, "TRACE", __VA_ARGS__)
#define LOG_INFO(...)  LOG_BASE(stdout, "INFO", __VA_ARGS__)
#define LOG_SUCCESS(...)  LOG_BASE(stdout, "SUCCESS", __VA_ARGS__)
#define LOG_WARNING(...)  LOG_BASE(stderr, "WARNING", __VA_ARGS__)
#define LOG_ERROR(...) LOG_BASE(stderr, "ERROR", __VA_ARGS__)
#define LOG_FATAL(...) {                        \
        LOG_BASE(stderr, "FATAL", __VA_ARGS__); \
        exit(EXIT_FAILURE);                     \
    }

#define COLORED_LOG_TRACE(...)   COLORED_LOG_BASE(stdout, "TRACE", \
                                                  ANSI_WHITE,            /* Log type color    */ \
                                                  ANSI_HI_WHITE,         /* File name color   */ \
                                                  ANSI_HI_WHITE,         /* Line number color */ \
                                                  __VA_ARGS__)
#define COLORED_LOG_INFO(...)    COLORED_LOG_BASE(stdout, "INFO", \
                                                  ANSI_B_WHITE,          /* Log type color    */ \
                                                  ANSI_HI_WHITE,         /* File name color   */ \
                                                  ANSI_HI_WHITE,         /* Line number color */ \
                                                  __VA_ARGS__)
#define COLORED_LOG_SUCCESS(...) COLORED_LOG_BASE(stdout, "SUCCESS", \
                                                  ANSI_B_GREEN,          /* Log type color    */ \
                                                  ANSI_HI_B_GREEN,       /* File name color   */ \
                                                  ANSI_HI_B_GREEN,       /* Line number color */ \
                                                  __VA_ARGS__)
#define COLORED_LOG_WARNING(...) COLORED_LOG_BASE(stdout, "WARNING", \
                                                  ANSI_B_YELLOW,         /* Log type color    */ \
                                                  ANSI_HI_B_YELLOW,      /* File name color   */ \
                                                  ANSI_HI_YELLOW,        /* Line number color */ \
                                                  __VA_ARGS__)
#define COLORED_LOG_ERROR(...)   COLORED_LOG_BASE(stdout, "ERROR", \
                                                  ANSI_B_RED,            /* Log type color    */ \
                                                  ANSI_HI_B_RED,         /* File name color   */ \
                                                  ANSI_HI_RED,           /* Line number color */ \
                                                  __VA_ARGS__)
#define COLORED_LOG_FATAL(...)   {              \
        COLORED_LOG_BASE(stdout, "FATAL",       \
                         ANSI_B_RED,    /* Log type color    */ \
                         ANSI_HI_B_RED, /* File name color   */ \
                         ANSI_HI_RED,   /* Line number color */ \
                         __VA_ARGS__);          \
        exit(EXIT_FAILURE);                     \
            }
#define LOG_H
#endif

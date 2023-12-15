#ifndef CORE_H

#include <stdint.h>
#include <stddef.h>

#define UNUSED(x) ((void)x)

// Basic types
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef double f64;
typedef float  f32;

typedef s32 b32;

#define true 1
#define false 0

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#if defined(__unix__)
#include <libgen.h>
#define FILEPATH __FILE__
#define FILENAME basename(__FILE__)
#define FILELINE __LINE__

#elif defined(_WIN32)
// @TODO: FILENAME macro has not been tested on Win32.
//        If it leads to compilation issues, just replace it with
//        # define FILENAME __FILE__
//        It will contain the full path instead of just the filename, but it
//        may prove useful anyway
#DEFINE FILEPATH __FILE__
#define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__) // https://stackoverflow.com/a/8488201
#define FILELINE __LINE__

#else
#define FILENAME __FILE__
#define FILELINE __LINE__
#endif

// Uses strftime under the hood, so refer to the documentation of strftime for format options
void get_clock_human_readable(char *buf, size_t buf_size, char *format);

#define CORE_H
#endif

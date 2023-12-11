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

void get_clock_human_readable(char *buf, size_t buf_size, char *format);

#define CORE_H
#endif

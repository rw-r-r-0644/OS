#pragma once

#include <sys/cdefs.h>
#include <stdarg.h>
#include <string.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int snprintf(char*, size_t, const char* __restrict, ...);
int vprintf(const char* __restrict, va_list);

#ifdef __cplusplus
}
#endif

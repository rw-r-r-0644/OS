#pragma once

#include <sys/cdefs.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int vsnprintf(char *buffer, u32 buffer_len, const char *fmt, va_list va);
int snprintf(char* buffer, u32 buffer_len, const char *fmt, ...);

int vprintf(const char *fmt, va_list va);
void printf(const char *fmt, ...);

int putchar (int c);
int puts (char *s);

char * itoa(int value, char * str, int base);

 // (internal functions)
u32 int_to_ascii(int value, u32 radix, u32 uppercase, u32 unsig, char *buffer, u32 zero_pad);

#ifdef __cplusplus
}
#endif
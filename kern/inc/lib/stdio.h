#pragma once

#include <sys/cdefs.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int vsnprintf(char *buffer, uint32_t buffer_len, const char *fmt, va_list va);
int snprintf(char* buffer, uint32_t buffer_len, const char *fmt, ...);

int vprintf(const char *fmt, va_list va);
void printf(const char *fmt, ...);

int putchar (int c);
int puts (char *s);

char * itoa(int value, char * str, int base);

 // (internal functions)
uint32_t int_to_ascii(int value, uint32_t radix, uint32_t uppercase, uint32_t unsig, char *buffer, uint32_t zero_pad);

#ifdef __cplusplus
}
#endif
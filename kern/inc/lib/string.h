#pragma once

#include <sys/cdefs.h>
#include <stddef.h>
#include <lib/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memchr(const void *s, int c, size_t n);
void *memcpy(void *s1, const void *s2, size_t n);
uint16_t *memcpy16(uint16_t *s1, const uint16_t *s2, size_t n);
void *memset(void *s, int c, size_t n);
uint16_t *memset16(uint16_t *s, uint16_t v, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

int strcmp(const char *s1, const char *s2);
char *strcat(char *s1, const char *s2);
char *strcpy(char *s1, const char *s2);
char *strncat(char *s1, const char *s2, size_t n);
char *strncpy(char *s1, const char *s2, size_t n);
char *strrchr(char * s, int c);
size_t strlen(const char *s);

// Utils
void reverse(char *s);
 
#ifdef __cplusplus
}
#endif

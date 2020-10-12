#pragma once

#include <sys/cdefs.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort();

int atoi(const char *s);

void free (void *ptr);
void * malloc(size_t size);
void * calloc(size_t num, size_t size);

#ifdef __cplusplus
}
#endif

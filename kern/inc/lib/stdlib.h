#pragma once

#include <sys/cdefs.h>
#include <stddef.h>

#include <mem/heap.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort();

int atoi(const char *s);

// Allocation functions
void * calloc(size_t num, size_t size); // Warning: Implemented badly, needs rewrite

#ifdef __cplusplus
}
#endif
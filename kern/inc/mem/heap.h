#pragma once

#include <lib/stdint.h>
#include <lib/stdbool.h>

extern uintptr_t heap_start;
extern uintptr_t heap_end;

/* malloc: general-purpose storage allocator */
void* malloc(unsigned nbytes);

/* free: put block ap in free list */
void free(void* ap);
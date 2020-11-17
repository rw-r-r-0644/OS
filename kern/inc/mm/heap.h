#pragma once
#include <lib/stdint.h>
#include <lib/stdbool.h>

/* heapalloc: allocate memory from the kernel heap. */
void *
kmalloc(size_t size);

/* heapfree: free memory to the kernel heap. */
void
kfree(void *ptr);

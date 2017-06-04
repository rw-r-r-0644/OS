#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void *kmalloc(size_t size, bool align, u32 *phys_addr);
void *kcalloc(size_t size, bool align, u32 *phys_addr);

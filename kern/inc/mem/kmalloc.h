#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void *kmalloc(size_t size, bool align);
void *kcalloc(size_t size, bool align);

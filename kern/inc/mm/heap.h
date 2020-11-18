#pragma once
#include <lib/stdint.h>
#include <lib/stdbool.h>

void *
kmalloc(unsigned long size);

void
kfree(void *ptr);

#pragma once
#include <lib/stdint.h>
#include <boot/multiboot2.h>

void *
multiboot2_get_tag(uint32_t type);

void
multiboot2_load_memory_map();

unsigned long
multiboot2_get_memory_end();

void
multiboot2_reserve_memory();

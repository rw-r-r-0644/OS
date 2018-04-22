#pragma once

#include <boot/mboot.h>

// Store pointers to the start of modules
extern u8* mboot_modules[40];

//! Map multiboot modules to memory
void mboot_map_mods(multiboot_info_t * mboot_info);
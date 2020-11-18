#pragma once
#include <x86/paging.h>

#define ERR_PTR ((void *)-1)


void *
ksbrk(unsigned long size);

void *
map_region(unsigned long physaddr, unsigned long size, unsigned long flags);

void
unmap_region(void *virtaddr, unsigned long size);

void
mm_init();

#pragma once

typedef struct cma_region cma_region;

struct cma_region
{
	unsigned long start;
	unsigned long size;
};

unsigned long
frm_alloc();

void
frm_free(unsigned long pa);

int
cma_alloc(unsigned long size, cma_region *region);

void
cma_free(cma_region *region);

void
pmm_add_memory(unsigned long start, unsigned long end);

void
pmm_reserve_memory(unsigned long start, unsigned long end);

void
pmm_init();

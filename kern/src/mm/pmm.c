/*
 * pmm.c
 *
 * Physical memory management.
 * Provides allocation of page frames
 * and contiguous memory ranges using
 * a buddy allocator.
 */
#include <mm/pmm.h>
#include <mm/mm.h>
#include <lib/bitset.h>
#include <kernel/panic.h>
#include <boot/multiboot2_lib.h>


/* The number of buddy allocators.
 * N buddies provide an unitary allocation
 * size of at most 2^N * PAGE_SIZE. */
#define N_BUDDIES		7

/* The size of the page frame allocator stack.
 * Pages in the allocator stack are allocated in
 * O(1), so a larger stack might provide quicker
 * frame allocation (but uses more memory). */
#define FRMSTACK_SIZE	256


/* bitset buddies for the buddy allocator */
bitset_t *
buddies[N_BUDDIES];


/* page frame allocation stack */
unsigned long
frm_stack[FRMSTACK_SIZE],
frm_stack_size = 0;


/* buddies_setrange
 * Set a range of memory in all the buddy allocators. */
static void
buddies_setrange(unsigned long start, unsigned long end)
{
	unsigned long b, rem = 0;
	for (b = 0; b < N_BUDDIES; b++)
	{
		bitset_setrange(buddies[b], start, end + rem);
		rem = (end & 1) && bitset_test(buddies[b], end);
		start >>= 1;
		end >>= 1;
	}
}


/* buddies_clrrange
 * Clear a range of memory in all the buddy allocators. */
static void
buddies_clrrange(unsigned long start, unsigned long end)
{
	unsigned long b, rem = 0;
	for (b = 0; b < N_BUDDIES; b++)
	{
		bitset_clrrange(buddies[b], start, end + rem);
		rem = end & 1;
		start >>= 1;
		end >>= 1;
	}
}


/* frm_alloc
 * Allocate a page frame. */
unsigned long
frm_alloc()
{
	unsigned long b, index, page;

	/* check for pages in the stacking allocator */
	if (frm_stack_size)
		return frm_stack[--frm_stack_size];

	/* find a free page, in the first buddy allocator */
	if (!bitset_findset(buddies[0], &index))
		panic("frame allocator out of memory!");
	page = index << PAGE_SHIFT;

	/* clear all the required buddy allocator entries */
	for (b = 0; b < N_BUDDIES; index >>= 1, b++)
		bitset_clr(buddies[b], index);

	return page;
}


/* frm_free
 * Free a page frame. */
void
frm_free(unsigned long pa)
{
	unsigned long b, index = pa >> PAGE_SHIFT;

	/* push the page in the stacking allocator,
	 * if there's space */
	if (frm_stack_size < FRMSTACK_SIZE) {
		frm_stack[frm_stack_size++] = pa;
		return;
	}

	/* commit the page to the buddies allocators */
	for (b = 0; b < N_BUDDIES; index >>= 1, b++) {
		bitset_set(buddies[b], index);
		if (!bitset_test(buddies[b], index ^ 1))
			break;
	}
}


/* cma_alloc
 * Allocate a region of contiguous memory. */
int
cma_alloc(unsigned long size, cma_region *region)
{
	unsigned long b, pages, entries, index;

	size = _ALIGN2_UP(size, PAGE_SIZE);
	pages = size >> PAGE_SHIFT;

	/* select buddy allocator for allocation size */
	for (b = 0; (b < N_BUDDIES) && (pages > (1ul << b)); b++);

	/* find enough available entries */
	entries = (pages + (1 << b) - 1) >> b;
	if (!bitset_findsetrange(buddies[b], entries, &index))
		return -1;

	/* clear all allocated entries */
	buddies_clrrange(index << b, pages);

	/* return allocated region */
	region->start = index << PAGE_SHIFT;
	region->size = size;
	return 0;
}


/* cma_free
 * Free a region of contiguous memory. */
void
cma_free(cma_region *region)
{
	/* mark the range as available */
	buddies_setrange(
		_SHLDIV_DOWN(region->start, PAGE_SHIFT),
		_SHLDIV_UP(region->start + region->size, PAGE_SHIFT)
	);

	region->start = (unsigned long)ERR_PTR;
	region->size = 0;
}


/* pmm_add_memory
 * Add an available memory range to the allocator. */
void
pmm_add_memory(unsigned long start, unsigned long end)
{
	buddies_setrange(
		_SHLDIV_DOWN(start, PAGE_SHIFT),
		_SHLDIV_UP(end, PAGE_SHIFT)
	);
}


/* pmm_reserve_memory
 * Mark a memory range as reserved. */
void
pmm_reserve_memory(unsigned long start, unsigned long end)
{
	buddies_clrrange(
		_SHLDIV_DOWN(start, PAGE_SHIFT),
		_SHLDIV_UP(end, PAGE_SHIFT)
	);
}


/* pmm_init
 * Initialize physical memory management. */
void
pmm_init()
{
	unsigned long b, memend, pages;

	/* find the number of available pages */
	memend = multiboot2_get_memory_end();
	pages = _SHLDIV_UP(memend, PAGE_SHIFT);

	/* initialize buddy allocators */
	for (b = 0; b < N_BUDDIES; b++)
		buddies[b] = bitset_init(_SHLDIV_UP(pages, b), 0);

	/* load the memory map from multiboot2 */
	multiboot2_load_memory_map();
}

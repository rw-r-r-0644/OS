/*
 * heap.c
 *
 * Kernel memory heap.
 * Based on the K&R heap implementation.
 */
#include <mm/heap.h>
#include <lib/stdint.h>
#include <kernel/utils.h>
#include <utils/deadlock.h>
#include <stdio.h>


/* block header */
typedef struct blkhdr blkhdr;
struct blkhdr
{
	/* next free block */
	blkhdr *ptr;

	/* size of this block */
	unsigned int size;
} _PACKED;


/* initial base block */
static blkhdr base = { &base, 0 };

/* free block list */
static blkhdr *freep = &base;


/* preallocated boot heap */
extern blkhdr
__bootheap_start[],
__bootheap_end[];

int
__bootheap_used = 0;



/* heapexpand
 *   nu: the number of heap blocks to allocate
 *
 * Allocates more virtual kernel memory for the heap.
 * Before vmm initialization, a preallocated boot heap
 * area is used instead.
 * 
 * TODO: implement vmm heap allocation
 */
static blkhdr *
heapexpand(unsigned int nu)
{
	blkhdr *up;

	if (&__bootheap_start[__bootheap_used + nu] <= __bootheap_end)
	{
		/* allocate the new blocks from boot heap */
		up = &__bootheap_start[__bootheap_used];
		up->size = nu;

		/* increment used boot heap */
		__bootheap_used += nu;
	}
	else
	{
		printf("heapexpand: error: out of boot memory.\n");
		deadlock();
	}

	heapfree(up + 1);
	return freep;
}


/* heapalloc
 *   size: the amount of memory to allocate
 * 
 * Allocates the first block of sufficient size from
 * the free block list.
 */
void *
heapalloc(size_t size)
{
	blkhdr *p, *prevp = freep;
	unsigned int nunits;

	/* compute the required number of blocks (+1 header block) */
	nunits = 1 + (size + sizeof(blkhdr) - 1) / sizeof(blkhdr);

	/* search for a big enough block */
	for (p = prevp->ptr; p->size < nunits; prevp = p, p = p->ptr)
	{
		if (p != freep)
			continue;

		/* wrapped around free list, try
		 * to allocate more kernel memory */
		if ((p = heapexpand(nunits)) == NULL)
			return NULL;
	}

	/* start next search here next time */
	freep = prevp;

	/* check if block should be split */
	if (p->size == nunits)
	{
		/* block matches the requested size */
		prevp->ptr = p->ptr;
	}
	else
	{
		/* split the block and allocate tail end */
		p->size -= nunits;
		p += p->size;
		p->size = nunits;
	}

	/* return memory past the block header */
	return (void*)(p + 1);
}


/* heapfree
 *   ptr: pointer to the memory to deallocate
 *
 * Puts freed block in the free block list;
 * keeps blocks ordered by address
 */
void
heapfree(void *ptr)
{
	blkhdr *bp, *p;

	/* get to block header */
	bp = (blkhdr*)ptr - 1;

	/* find the previous block */
	for (p = freep; !(bp > p && bp < p->ptr); p = p->ptr)
	{
		/* freed block at start or end of arena */
		if (p >= p->ptr && (bp > p || bp < p->ptr))
			break;
	}

	if ((bp + bp->size) == p->ptr)
	{
		/* join to next block */
		bp->size += p->ptr->size;
		bp->ptr = p->ptr->ptr;
	}
	else
		bp->ptr = p->ptr;

	if ((p + p->size) == bp)
	{
		/* join to previous block */
		p->size += bp->size;
		p->ptr = bp->ptr;
	}
	else
		p->ptr = bp;

	/* start free blocks search here next time */
	freep = p;
}


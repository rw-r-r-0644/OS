/*
 * heap.c
 * Kernel heap
 */

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/kmalloc.h>
#include <mem/page_frame.h>

#include <lib/stdint.h>
#include <lib/stdbool.h>
#include <lib/utils.h>

#include <utils/logger.h>

typedef struct _blkhdr {
	struct _blkhdr * ptr; /* next free block */
	unsigned size; /* size of this block */
} _packed blkhdr_t;

static blkhdr_t base; /* empty list to get started */
static blkhdr_t *freep = NULL; /* start of free list */

uintptr_t heap_start;
uintptr_t heap_end;

#define NALLOC 1024 /* minimum #units to request */

/* morecore: ask system for more memory */
static blkhdr_t* morecore(unsigned nu)
{
    if (nu < NALLOC)
        nu = NALLOC;
    blkhdr_t* up = (blkhdr_t *)kmalloc(nu * sizeof(blkhdr_t), false);
    up->size = nu;
    free((void *)(up+1));
    return freep;
}

/* morecore: ask system for more memory
static blkhdr_t* morecore(unsigned nu)
{
	char *cp, *sbrk(int);
	blkhdr_t* up;

	size_t size = ((nu * sizeof(blkhdr_t)) & 0xFFFFF000) + 0x1000;

	if(!heap_start)
		heap_end = heap_start = ((uintptr_t)&kernel_end & 0xFFFFF000) + size;
	if(paging_enabled())
		identity_map(kernel_pd, heap_end, heap_end + size, 3);
	cp = (char *)heap_end;
	heap_end += size;

	up = (blkhdr_t *) cp;
	up->size = size / sizeof(blkhdr_t);
	free((void *)(up+1));
	return freep;
}
*/

/* memalign: aligned storage allocator */
void *memalign(size_t alignment, size_t size)
{
	return NULL;
}

/* malloc: general-purpose storage allocator */
void* malloc(unsigned nbytes)
{
	blkhdr_t *p, *prevp;
	blkhdr_t* morecore(unsigned); /* used to get another large block from OS */
	unsigned nunits;
	nunits = (nbytes + sizeof(blkhdr_t) - 1) / sizeof(blkhdr_t) + 1;
	/* round up to allocate in units of sizeof(blkhdr_t) */
	if ((prevp = freep) == NULL) { /* no free list yet */
		base.ptr = freep = prevp = &base;
		base.size = 0;
	}
	for (p = prevp->ptr;; prevp = p, p = p->ptr) {
		if (p->size >= nunits) { /* big enough */
			if (p->size == nunits) /* exactly */
				prevp->ptr = p->ptr;
			else { /* allocate tail end */
				p->size -= nunits;
				p += p->size;
				p->size = nunits;
			}
			freep = prevp; /* start next search here next time */
			return (void*)(p + 1); /* point past the blkhdr_t */
		}
		if (p == freep) /* wrapped around free list */
			if ((p = morecore(nunits)) == NULL)
				return NULL; /* none left */
	}
}

/* free: put block ap in free list */
void free(void* ap) /* keeps blocks ordered by address */
{
	blkhdr_t *bp, *p;
	bp = (blkhdr_t*)ap - 1; /* point to block blkhdr_t */
	for (p = freep; !(bp > p && bp < p->ptr); p = p->ptr)
		if (p >= p->ptr && (bp > p || bp < p->ptr))
			break;
	/* freed block at start or end of arena */
	if (bp + bp->size == p->ptr) { /* join to next block */
		bp->size += p->ptr->size;
		bp->ptr = p->ptr->ptr;
	}
	else
		bp->ptr = p->ptr;
	if (p + p->size == bp) { /* join to previous block */
		p->size += bp->size;
		p->ptr = bp->ptr;
	}
	else
		p->ptr = bp;
	freep = p;
}
/* resources.c
 * 
 * I/O device resources management.
 * Keeps track of device resource allocations to
 * ensure two drivers do not claim the same resource.
 * Heavily inspired by Linux I/O resource management.
 */
#include <mm/mm.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <kernel/resources.h>
#include <lib/stdlib.h>

static resource *
mem = NULL;


/* check_conflict
 * Check if the specified area conflicts with the given resource */
static int
check_conflict(resource *r, unsigned long start, unsigned long n)
{
	return
		((start >= r->start) &&
		 (start < (r->start + r->n))) ||
		(((start + n) > r->start) &&
		 ((start + n) <= (r->start + r->n)));
}


/* iomemrequest
 * Request an mmio region for the device */
resource *
iomemrequest(unsigned long start, unsigned long n, const char *name)
{
	resource *r, **p = &mem;

	/* check for resource conflicts */
	for (r = mem; r; r = r->next)
	{
		if (check_conflict(r, start, n))
			return ERR_PTR;
		if (r->start < start)
			p = &r->next;
	}

	/* ensure resource memory is reserved */
	pmm_reserve_memory(start, start + n - 1);

	/* allocate resource */
	r = malloc(sizeof(resource));
	r->type = IORESOURCE_MEM;
	r->start = start;
	r->n = n;
	r->name = name;

	/* insert in resource list */
	r->next = *p;
	*p = r;

	return r;
}


/* iorelease
 * Release allocated resource */
void
iorelease(resource *r)
{
	resource *prev;

	if (r->type == IORESOURCE_MEM)
	{
		/* unmap if mapped */
		iounmap(r);

		/* remove from list */
		for (prev = mem;
			 prev && prev->next != r;
			 prev = prev->next);
		if (prev)
			prev->next = r->next;
	}

	free(r);
}


/* ioremap
 * Map specified resource to kernel memory */
void *
ioremap(resource *r)
{
	if (r->type != IORESOURCE_MEM)
		return ERR_PTR;
	r->mapping = map_region(r->start, r->n, MAP_IODEV);
	return r->mapping;
}


/* iounmap
 * Unmap specified resource from kernel memory */
void
iounmap(resource *r)
{
	if ((r->type != IORESOURCE_MEM) || (r->mapping == ERR_PTR))
		return;
	unmap_region(r->mapping, r->n);
}

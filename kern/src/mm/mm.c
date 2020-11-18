/*
 * pmm.c
 *
 * Kernel memory heap.
 * Based on the K&R heap implementation.
 */
#include <mm/mm.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <x86/paging.h>
#include <kernel/utils.h>


int
__mm_is_init = 0;


/* ksbrk
 * Allocate more space in the kernel memory
 * area, and map it to physical pages. */
void *
ksbrk(unsigned long size)
{
	unsigned long mapped;
	unsigned char *ptr;

	size = _ALIGN2_UP(size, PAGE_SIZE);

	/* allocate virtual memory */
	ptr = vmm_alloc_kern(size);
	if (ptr == ERR_PTR)
		return ptr;

	/* map the requested area */
	for (mapped = 0; mapped < size; mapped += 0x1000)
		map_page(frm_alloc(), ptr + mapped, MAP_KERNEL);

	return ptr;
}


/* map_region
 * Map a region of memory to kernel's virtual memory */
void *
map_region(unsigned long physaddr, unsigned long size, unsigned long flags)
{
	unsigned long physpg, offset, i;
	unsigned char *ptr;

	/* align adresses */
	physpg = physaddr & PAGE_MASK;
	offset = physaddr & ~PAGE_MASK;
	size = _ALIGN2_UP(size + offset, PAGE_SIZE);

	/* allocate virtual memory */
	ptr = vmm_alloc_kern(size);
	if (ptr == ERR_PTR)
		return ptr;

	/* map pages */
	for (i = 0; i < size; i += PAGE_SIZE)
		map_page(physpg + i, ptr + i, flags);

	return (void *)(ptr + offset);
}


/* unmap_region
 * Unmap a region of memory from kernel's virtual memory */
void
unmap_region(void *virtaddr, unsigned long size)
{
	unsigned long virtpg, offset, i;

	/* align adresses */
	virtpg = (unsigned long)virtaddr & PAGE_MASK;
	offset = (unsigned long)virtaddr & ~PAGE_MASK;
	size = _ALIGN2_UP(size + offset, PAGE_SIZE);

	/* unmap pages */
	for (i = 0; i < size; i += PAGE_SIZE)
		unmap_page((void *)(virtpg + i));
}


/* mm_init
 * Initializes memory management. */
void
mm_init()
{
	pmm_init();
	paging_finalize();
	vmm_init();

	__mm_is_init = 1;
}

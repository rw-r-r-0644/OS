/*
 * vmm.c
 * 
 * Basic virtual memory management.
 * Right now this allocator only provides
 * simple placement allocations.
 */
#include <kernel/panic.h>
#include <mm/layout.h>
#include <mm/mm.h>

typedef struct vm_area vm_area;
struct vm_area
{
	unsigned long start;
	unsigned long end;
};

vm_area kernelmem_area =
{
	.start = KERNEL_END,
	.end = 0xFF800000,
};


/* prevent calls before vmm initialization
 * from succeding; otherwise might lead to
 * hard to discover bugs */
static int
vmm_initialized = 0;


/* vmm_alloc_kern
 * Allocates virtual memory in the kernel address space.
 * The returned memory is NOT mapped. */
void *
vmm_alloc_kern(unsigned long size)
{
	void *ptr;
	size = _ALIGN2_UP(size, PAGE_SIZE);

	if (!vmm_initialized)
		return ERR_PTR;

	/* ensure there's memory available after the kernel */
	if ((kernelmem_area.end - kernelmem_area.start) < size)
		panic("Out of kernel virtual memory!");

	/* move the start of available kernel virtual memory */
	ptr = (void *)kernelmem_area.start;
	kernelmem_area.start += size;

	return ptr;
}


/* vmm_init
 * Initializes simple virtual memory allocation. */
void
vmm_init()
{
	vmm_initialized = 1;
}

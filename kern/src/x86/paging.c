/*
* paging.c
* 
* x86 memory paging
* 
* Adress ranges used by the driver:
*   0xFEC00000 - 0xFFC00000: reserved for paging driver use
*   0xFFC00000 - 0xFFFFF000: page tables area
*   0xFFFFF000 -  MEMORYEND: page directory
*/

#include <kernel/utils.h>
#include <lib/string.h>
#include <x86/paging.h>
#include <mm/layout.h>
#include <mm/pmm.h>
#include <mm/mm.h>

#define PDE_PRESENT		(1 << 0)
#define PDE_WRITABLE	(1 << 1)
#define PDE_USER		(1 << 2)
#define PDE_WTCACHE		(1 << 3)
#define PDE_NOCACHE		(1 << 4)
#define PDE_ACCESSED	(1 << 5)
#define PDE_PAGESIZE	(1 << 7)

#define PDE_DEFAULT		(PDE_PRESENT)


static unsigned long * const
pagedir = (unsigned long * const)0xFFFFF000;


/* drvpagetbl is used for mappings required
 * by this driver (vmm cannot be used as it
 * might not have been initialized) */
static unsigned long _ALIGNED(PAGE_SIZE)
drvpagetbl[1024] = {0};


/* Boot page directory.
 * 
 * The kernel is mapped to the higher half of 
 * the address space.
 *
 * 64Mib, or 8 4Mib pages, starting at 0x00000000
 * are pre mapped to 0xC0000000 for kernel boot.
 * After proper paging initialization, the unneeded
 * memory will be unmapped.
 *
 * If the kernel ever grows bigger than 64Mib, this might
 * need to be adjusted, but it's probably going to be
 * plenty for quite a while. */
#define BOOT_PREMAPPED	(0x2000000)
unsigned long _ALIGNED(PAGE_SIZE)
bootpagedir[1024] = {
	/* identity map kernel at boot */
	[0x000 / 4] = 0x00000081, [0x004 / 4] = 0x00400081,
	[0x008 / 4] = 0x00800081, [0x00C / 4] = 0x00C00081,
	[0x010 / 4] = 0x01000081, [0x014 / 4] = 0x01400081,
	[0x018 / 4] = 0x01800081, [0x01C / 4] = 0x01C00081,

	/* map kernel to higher half at boot */
	[0xC00 / 4] = 0x00000081, [0xC04 / 4] = 0x00400081,
	[0xC08 / 4] = 0x00800081, [0xC0C / 4] = 0x00C00081,
	[0xC10 / 4] = 0x01000081, [0xC14 / 4] = 0x01400081,
	[0xC18 / 4] = 0x01800081, [0xC1C / 4] = 0x01C00081,

	/* setup drvpagetbl */
	[0xFF8 / 4] = (unsigned long)drvpagetbl - KERNEL_VIRT_OFFSET + PDE_PRESENT,

	/* the page directory last entry maps to
	 * itself for easy access to page tables */
	[0xFFC / 4] = (unsigned long)bootpagedir - KERNEL_VIRT_OFFSET + PDE_PRESENT,
};


static inline void flush_tlb(void * virtaddr)
{
   asm volatile("invlpg (%0)" ::"r" (virtaddr) : "memory");
}

static inline void flush_pagetable_tlb(void * virtaddr)
{
	unsigned char *va = (unsigned char *)virtaddr;
	unsigned long i;

	for (i = 0; i < 1024; va += PAGE_SIZE, i++)
		flush_tlb(va);
}

static inline unsigned long *get_pagetable(unsigned long index)
{
	return ((unsigned long *)0xFFC00000) + (1024 * index);
}


/* read_physaddr
 * Read from physical memory to the passed memory buffer
 * 
 * Warning: Slow, use infrequently and only
 * during early boot stages */
void
read_physaddr(unsigned long physaddr, void *buffer, unsigned long size)
{
	unsigned char * const virtmapping = (unsigned char * const)0xFF800000;
	unsigned long pa, copy, offset, done = 0;

	/* get first page phyaddr */
	pa = physaddr & PAGE_MASK;
	
	while (done < size)
	{
		/* map current pa */
		drvpagetbl[0] = pa | MAP_KERNEL;
		flush_tlb(virtmapping);

		/* compute page offset and size */
		offset = done ? 0 : (physaddr - pa);
		copy = _MIN(size - done, PAGE_SIZE - offset);

		/* actually copy the data */
		memcpy(
			(unsigned char *)buffer + done,
			(unsigned char *)virtmapping + offset,
			copy
		);

		/* go to next page */
		done += copy;
		pa += PAGE_SIZE;
	}

	/* unmap page */
	drvpagetbl[0] = 0;
	flush_tlb(virtmapping);
}


unsigned long
get_physaddr(void *virtaddr)
{
	unsigned long pdindex, ptindex, *pagetbl;
	pdindex = (unsigned long)virtaddr >> 22;
	ptindex = (unsigned long)virtaddr >> 12 & 0x03FF;

	/* the pde is not present, page can't be mapped */
	if ((pagedir[pdindex] & PDE_PRESENT) == 0)
		return (unsigned long)ERR_PTR;

	/* handle 4mib pages */
	if (pagedir[pdindex] & PDE_PAGESIZE)
		return (pagedir[pdindex] & ~0x3FFFFF) +
			((unsigned long)virtaddr & 0x3FFFFF);

	pagetbl = get_pagetable(pdindex);

	/* check if page is present in pagetable */
	if ((pagetbl[ptindex] & PAGE_PRESENT) == 0)
		return (unsigned long)ERR_PTR;

	return (pagetbl[ptindex] & PAGE_MASK) +
		((unsigned long)virtaddr & ~PAGE_MASK);
}


void
map_page(unsigned long physaddr, void *virtaddr, unsigned long flags)
{
	unsigned long pdindex, ptindex, *pagetbl;
	pdindex = (unsigned long)virtaddr >> 22;
	ptindex = (unsigned long)virtaddr >> 12 & 0x03FF;

	/* get address of pagetable */
	pagetbl = get_pagetable(pdindex);

	/* pagetable not present - allocate, map and flush tlb */
	if ((pagedir[pdindex] & PDE_PRESENT) == 0)
	{
		pagedir[pdindex] = frm_alloc() | PDE_DEFAULT;
		flush_tlb(pagetbl);
	}

	/* map the page in the pagetable and flush tlb */
	pagetbl[ptindex] = (physaddr & PAGE_MASK) | (flags & 0xFFF) | PAGE_PRESENT;
	flush_tlb(virtaddr);
}


void
unmap_page(void *virtaddr)
{
	unsigned long pdindex, ptindex, *pagetbl;
	pdindex = (unsigned long)virtaddr >> 22;
	ptindex = (unsigned long)virtaddr >> 12 & 0x03FF;

	/* pagetable not present - page must not be mapped? */
	if ((pagedir[pdindex] & PDE_PRESENT) == 0)
		return;

	/* get address of pagetable */
	pagetbl = get_pagetable(pdindex);

	/* unmap the page in the pagetable and flush tlb */
	pagetbl[ptindex] = 0;
	flush_tlb(virtaddr);
}


void
paging_finalize()
{
	unsigned long i, j, k, pt_pa, *pt_va = (unsigned long *)0xFFBFF000;

	/* unmap kernel identity mapping */
	for (i = 0; i < BOOT_PREMAPPED; i += 0x400000)
	{
		bootpagedir[i >> 22] = 0;
		flush_pagetable_tlb((void *)i);
	}

	/* remap kernel with 4k pages */
	for (i = 0; i < BOOT_PREMAPPED; i += 0x400000)
	{
		/* allocate new page table */
		pt_pa = frm_alloc();
		drvpagetbl[1023] = pt_pa | PAGE_PRESENT;
		flush_tlb(pt_va);

		/* map pages in pagetable */
		for (j = 0, k = i; j < 1024; j++, k += PAGE_SIZE)
		{
			pt_va[j] = ((k >= KERNEL_PA_START) && (k < KERNEL_PA_END)) ?
				(k | PAGE_PRESENT) : 0;
		}

		/* swap the pagetable in pagedir */
		pagedir[(i + KERNEL_VIRT_OFFSET) >> 22] = pt_pa | PDE_DEFAULT;
		flush_pagetable_tlb((void *)i);
	}
}

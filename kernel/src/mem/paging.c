#include <mem/paging.h>
#include <mem/kmalloc.h>
#include <mem/heap.h>
#include <mem/page_frame.h>

#include <utils/logger.h>
#include <utils/utils.h>

#include <lib/stdbool.h>
#include <lib/stdint.h>
#include <lib/string.h>

u32* kernel_pd;

void switch_pd(u32* pd)
{
	asm volatile("mov %0, %%cr3":: "r"(pd));
}
/*
u32* clone_pt(u32* old_pt)
{
	u32* new_pt = create_pt();

	for (int i = 0; i < 1024; i++)
	{
		if (old_pt[i] & 0x1)
			continue;
	}
}

u32* clone_pd(u32* old_pd)
{
	log_info("Staring pagedir copy");
	
	u32* new_pd;
	kcalloc(4096, true, &new_pd);
	
	// Add base entries from kernel's pd
	for (int i = 0; i < 1024; i++)
	{
		// Only copy the page table if it's in memory
		if (old_pd[i] & 0x1)
		{
			if (kernel_pd[i] == old_pd[i])	// Use the same pointer
				new_pd[i] = old_pd[i];
			else							// Copy the table
				new_pd[i] = (u32)clone_pt(old_pd[i] & 0xFFFFF000) & 0x3;
				
			//new_pd[i] = (kernel_pd[i] == old_pd[i]) ? old_pd[i] : (clone_pt(old_pd[i] & 0xFFFFF000) & 0x3);
		}
	}
	
	return new_pd;
}
*/

void paging_map(u32* pd, uintptr_t phys, uintptr_t virt, unsigned attrib)
{
	phys = ALIGN_DOWN(phys, 0x1000);
	virt = ALIGN_DOWN(virt, 0x1000);

	u32 *pde = &pd[virt >> 22]; // get pagedir entry

	// get the pagetable
	u32 *pt = (u32*)(*pde & 0xFFFFF000);
	if (!pt) // if not existing, create it
		*pde = (u32)kcalloc(4096, true) | 3;

	u32 *pte = &pt[virt >> 12 & 0x03FF]; // get pagetable entry
	
	if (*pte & 0x1)	// page already present
		log_warning("remapping existing page %08X", virt);
 	
	*pte = (phys & 0xFFFFF000) | (attrib & 0xFFF) | 0x01; // map the page
}

void paging_map_range(u32* pd, uintptr_t phys, uintptr_t virt, size_t size, unsigned attrib)
{
	for(size_t i = 0; i < size; i+= 0x1000)
		paging_map(pd, phys + i, virt + i, attrib);
}

void paging_map_virt_range(u32* pd, uintptr_t virt, size_t size, unsigned attrib)
{	
	for(size_t i = 0; i < size; i+= 0x1000)
		paging_map(pd, find_frame(), virt + i, attrib);
}

void paging_unmap(u32* pd, uintptr_t virt)
{
	virt = ALIGN_DOWN(virt, 0x1000);

	u32 *pde = &pd[virt >> 22]; // get pagedir entry
	u32 pti = virt >> 12 & 0x03FF; // get pagetable index

	u32* pt = (u32*)(*pde & 0xFFFFF000); // get the pagetable

	if (!pt || !(pt[pti] & 0x1)) // the page/pagetable doesn't exist
		log_warning("unmapping unexisting page %08X", virt);
	
	pt[pti] = 0; // unmap the page
}

void paging_unmap_range(u32* pd, uintptr_t virt, size_t size)
{
	for(size_t i = 0; i < size; i += 0x1000)
		paging_unmap(pd, virt);
}

void paging_enable()
{
	// Enable paging by oring cr0 with pagin enable bit
	asm volatile(	"mov %cr0, %eax\n\t" \
					"or $0x80000000, %eax\n\t" \
					"mov %eax, %cr0\n\t"	);
}

bool paging_enabled()
{
	u32 cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	return !!(cr0 & 0x80000000);
}

void paging_init()
{	
	// Create kernel's page directory
	kernel_pd = kcalloc(4096, true);
	
	// Switch to kernel's page directory
	switch_pd(kernel_pd);
}
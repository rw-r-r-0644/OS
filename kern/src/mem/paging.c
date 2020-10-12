#include <mem/paging.h>
#include <mem/heap.h>
#include <mem/page_frame.h>

#include <utils/logger.h>
#include <kernel/utils.h>

#include <lib/stdbool.h>
#include <lib/stdint.h>
#include <lib/string.h>

void switch_pd(uint32_t* pd)
{
	asm volatile("mov %0, %%cr3":: "r"(pd));
}
/*
uint32_t* clone_pt(uint32_t* old_pt)
{
	uint32_t* new_pt = create_pt();

	for (int i = 0; i < 1024; i++)
	{
		if (old_pt[i] & 0x1)
			continue;
	}
}

uint32_t* clone_pd(uint32_t* old_pd)
{
	log_info("Staring pagedir copy");
	
	uint32_t* new_pd;
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
				new_pd[i] = (uint32_t)clone_pt(old_pd[i] & 0xFFFFF000) & 0x3;
				
			//new_pd[i] = (kernel_pd[i] == old_pd[i]) ? old_pd[i] : (clone_pt(old_pd[i] & 0xFFFFF000) & 0x3);
		}
	}
	
	return new_pd;
}
*/

void paging_map(uint32_t* pd, uintptr_t phys, uintptr_t virt, unsigned attrib)
{
/*	phys = _align2_down(phys, 0x1000);
	virt = _align2_down(virt, 0x1000);

	uint32_t *pde = &pd[virt >> 22]; // get pagedir entry

	// get the pagetable
	uint32_t *pt = (uint32_t*)(*pde & 0xFFFFF000);
	if (!pt) // if not existing, create it
		*pde = (uint32_t)kcalloc(4096, true) | 3;

	uint32_t *pte = &pt[virt >> 12 & 0x03FF]; // get pagetable entry
	
	if (*pte & 0x1)	// page already present
		log_warning("remapping existing page %08X", virt);
 	
	*pte = (phys & 0xFFFFF000) | (attrib & 0xFFF) | 0x01; // map the page*/
}

void paging_map_range(uint32_t* pd, uintptr_t phys, uintptr_t virt, size_t size, unsigned attrib)
{
	for(size_t i = 0; i < size; i+= 0x1000)
		paging_map(pd, phys + i, virt + i, attrib);
}

void paging_map_virt_range(uint32_t* pd, uintptr_t virt, size_t size, unsigned attrib)
{	
	for(size_t i = 0; i < size; i+= 0x1000)
		paging_map(pd, find_frame(), virt + i, attrib);
}

void paging_unmap(uint32_t* pd, uintptr_t virt)
{
	virt = _align2_down(virt, 0x1000);

	uint32_t *pde = &pd[virt >> 22]; // get pagedir entry
	uint32_t pti = virt >> 12 & 0x03FF; // get pagetable index

	uint32_t* pt = (uint32_t*)(*pde & 0xFFFFF000); // get the pagetable

	if (!pt || !(pt[pti] & 0x1)) // the page/pagetable doesn't exist
		log_warning("unmapping unexisting page %08X", virt);
	
	pt[pti] = 0; // unmap the page
}

void paging_unmap_range(uint32_t* pd, uintptr_t virt, size_t size)
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
	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	return !!(cr0 & 0x80000000);
}
/*
void paging_init()
{	
	// Create kernel's page directory
	kernel_pd = kcalloc(4096, true);
	
	// Switch to kernel's page directory
	switch_pd(kernel_pd);
}
*/

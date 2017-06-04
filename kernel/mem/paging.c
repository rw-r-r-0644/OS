#include <mem/paging.h>
#include <mem/kmalloc.h>

#include <utils/common.h>
#include <utils/logger.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

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

// Sometimes it may be needed...
void invlpg(u32 addr)
{
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

int map_page(u32* pd, void* physaddr, void* virtualaddr, u32 attrib)
{
	u32 pdindex = (u32)virtualaddr >> 22;
	u32 ptindex = (u32)virtualaddr >> 12 & 0x03FF;

	u32* pt = (u32*)(pd[pdindex] & 0xFFFFF000);
	if (!pt)
	{
		// Create the page table
		kcalloc(4096, true, (u32*)&pt);	// 4kb aligned, physical
		
		// Add the page table to the page directory
		pd[pdindex] = (u32)pt | 3;		// Supervisor, rw, present
	}
	
	if (pt[ptindex] & 0x1) // Page already present
		return -1;
 	
	pt[ptindex] = ((u32)physaddr) | (attrib & 0xFFF) | 0x01; // Present
		
	return 0;
}

int map_pages(u32* pd, void* physaddr_start, void* physaddr_end, void* virtualaddr_start, void* virtualaddr_end, u32 attrib)
{
	int ret = 0;
	
	u32 phys = (u32)physaddr_start;
	u32 virt = (u32)virtualaddr_start;
	
	while(  phys < (u32)physaddr_end &&		// Map until physical address end
			virt < (u32)virtualaddr_end &&	// Map until virtual address end
			ret == 0  )						// Make sure there are no errors
	{
		// Map current page
		ret += map_page(pd, (void *)phys, (void *)virt, attrib);
		
		// Each page is 4k
		phys += 0x1000;
		virt += 0x1000;
	}
	
	return ret;
}

int unmap_page(u32* pd, void* virtualaddr)
{
	u32 pdindex = (u32)virtualaddr >> 22;
	u32 ptindex = (u32)virtualaddr >> 12 & 0x03FF;
	
	u32* pt = (u32*)(pd[pdindex] & 0xFFFFF000);
	if (!pt) // It the page table doesn't exist, the page must already be unmapped
		return -1;
	
	if (!(pt[ptindex] & 0x1)) // The page is not present
		return -1;
	
	pt[ptindex] = 0; // Clear the page
	return 0;
}

int unmap_pages(u32* pd, void* virtualaddr_start, void* virtualaddr_end)
{
	int ret = 0;
	
	u32 virt = (u32)virtualaddr_start;
	
	while(  virt < (u32)virtualaddr_end &&	// Map until physical address end
			ret == 0  )						// Make sure there are no errors
	{
		// Unmap current page
		ret += unmap_page(pd, (void *)virt);
		
		// Pages are 4k
		virt += 0x1000;
	}
	
	return ret;
}

int page_set_attrib(u32* pd, void* address, u32 attrib)
{
	u32 pdindex = (u32)address >> 22;
	u32 ptindex = (u32)address >> 12 & 0x03FF;

	u32* pt = (u32*)(pd[pdindex] & 0xFFFFF000);
	if (!pt)
		return -1;
 	
	pt[ptindex] |= (attrib & 0xFFF);
	return 0;
}

void* get_physaddr(u32* pd, void* virtualaddr)
{
	u32 pdindex = (u32)virtualaddr >> 22;
	u32 ptindex = (u32)virtualaddr >> 12 & 0x03FF;

	u32* pt = (u32*)(pd[pdindex] & 0xFFFFF000);
	if (!pt)
		return 0;

	u32 page = pt[ptindex];
	if (page & 1)
		return (void*)(page >> 12);
	else
		return 0;
}

void enable_paging()
{
	// Enable paging by oring cr0 with pagin enable bit
	asm volatile(	"mov %cr0, %eax\n\t" \
					"or $0x80000000, %eax\n\t" \
					"mov %eax, %cr0\n\t"	);
}

void identity_map(u32* pd, void * start_addr, void * end_addr, u32 attr)
{
	for(u32 i = (u32)start_addr; i < (u32)end_addr; i += 0x1000)
		map_page(pd, (void *)i, (void *)i, 3);
}

void paging_init()
{	
	// Create kernel's page directory
	kcalloc(4096, true, &kernel_pd);
	
	// Identity map the kernel
	log_printf(LOG_DEBUG, "Mapping the kernel; start: 0x%p, end: 0x%p", &kernel_start, &kernel_end);
	identity_map(kernel_pd, 0, &kernel_end, 3);
	
	// Switch to kernel's page directory
	switch_pd(kernel_pd);
	
	// Enable paging
	enable_paging();
}
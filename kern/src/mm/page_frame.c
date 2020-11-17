#include <mm/page_frame.h>
#include <mm/heap.h>
#include <mm/kernel.h>

#include <lib/stdint.h>
#include <lib/stdlib.h>
#include <lib/string.h>

#include <utils/logger.h>

uint32_t * frame_mem;
uint32_t frame_size;

void set_frame(uint32_t aligned_addr, frame_status_t state)
{
	aligned_addr /= 0x1000;
	
	uint32_t idx = aligned_addr / 32;	// Each block of frames is 32 bits
	uint32_t bit = aligned_addr % 32;
	
	(state == FRAME_SET) ?	( frame_mem[idx] |= (1 << bit) ) : // Set the bit
							( frame_mem[idx] &= ~(1 << bit) ); // Clear the bit
}

void set_frames(uint32_t adress_start, uint32_t address_end, frame_status_t state)
{
	uint32_t start_aligned = adress_start & 0xFFFFF000; // Round down
	uint32_t end_aligned = (address_end & 0xFFFFF000) + 0x1000; // Round up
	
	// Set individual frames to state
	for(uint32_t i = start_aligned; i < end_aligned; i += 0x1000)
		set_frame(i, state);
}

frame_status_t test_frame(uint32_t aligned_addr)
{
	aligned_addr /= 0x1000;
	
	uint32_t idx = aligned_addr / 32;	// Each block of frames is 32 bits
	uint32_t bit = aligned_addr % 32;

	return (frame_status_t)(frame_mem[idx] & (1 << bit)); // Return bit state
}

uintptr_t find_frame()
{
	for (uint32_t i = 0; i < (frame_size / 32); i++)
	{
		if (frame_mem[i] == 0xFFFFFFFF)
			continue;
		
		for (uint32_t ii = 0; ii < 32; ii++)
		{
			if( !( frame_mem[i] & (1 << ii) ) )
				return (i * 32 + ii) * 0x1000;
		}
	}
	return 0;
}

void pageframe_init(uint32_t mem_size)
{
	log_info("Initializing, mem size = %u Mib", mem_size / 1024);
	
	frame_size = mem_size / 4;     /* 4k blocks */
	log_warning("Allocating %u bytes of memory", frame_size / 8);
	frame_mem  = calloc(frame_size / 8, 1);
	
	log_debug("frame_size = %u, frame = 0x%08x", frame_size, frame_mem);
	
	// Set the kernel memory as used
	set_frames(0x0, kernel_end, FRAME_SET);
	log_debug("set kernel physical memory as allocated");

/*	// Set the heap memory as used
	set_frames(heap_start, heap_end, FRAME_SET);
	log_debug("set heap physical memory as allocated");*/
	
	// Check if the bootloader kindly provides us with a memory map
/*	if (mbinfo->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		log_info("Setting up memory map from multiboot");
		log_debug("mmap info: len 0x%x, addr 0x%p", mbinfo->mmap_length, mbinfo->mmap_addr);
		
		uint32_t mem = mbinfo->mmap_addr;
		while(mem < (mbinfo->mmap_addr + mbinfo->mmap_length))
		{
			multiboot_memory_map_t * mmap_e = (multiboot_memory_map_t *)mem;
			
			// Set memory as used
			if (mmap_e->type == MULTIBOOT_MEMORY_RESERVED)
			{
				uint32_t end_addr = (uint32_t)mmap_e->addr + (uint32_t)mmap_e->len;
				
				if (end_addr < mmap_e->addr) // HACKY: "handle" 64 bit addresses
					end_addr = 0xFFFFEFFF;
				
				set_frames((uint32_t)mmap_e->addr, end_addr, FRAME_SET);
				log_debug("mmap: set memory from 0x%p to 0x%p as used", (uint32_t)mmap_e->addr, end_addr);
			}
			
			mem += mmap_e->size + sizeof(mmap_e->size);
		}
		
	}
*/
}

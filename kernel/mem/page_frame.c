#include <mem/kmalloc.h>
#include <mem/page_frame.h>

#include <mboot/mboot.h>

#include <stdint.h>
#include <string.h>
#include <utils/logger.h>

u32 * frame_mem;
u32 frame_size;

void set_frame(u32 aligned_addr, frame_status_t state)
{
	aligned_addr /= 0x1000;
	
	u32 idx = aligned_addr / 32;	// Each block of frames is 32 bits
	u32 bit = aligned_addr % 32;
	
	(state == FRAME_SET) ?	( frame_mem[idx] |= (1 << bit) ) : // Set the bit
							( frame_mem[idx] &= ~(1 << bit) ); // Clear the bit
}

void set_frames(u32 adress_start, u32 address_end, frame_status_t state)
{
	u32 start_aligned = adress_start & 0xFFFFF000; // Round down
	u32 end_aligned = (address_end & 0xFFFFF000) + 0x1000; // Round up
	
	// Set individual frames to state
	for(u32 i = start_aligned; i < end_aligned; i += 0x1000)
		set_frame(i, state);
}

frame_status_t test_frame(u32 aligned_addr)
{
	aligned_addr /= 0x1000;
	
	u32 idx = aligned_addr / 32;	// Each block of frames is 32 bits
	u32 bit = aligned_addr % 32;

	return (frame_status_t)(frame_mem[idx] & (1 << bit)); // Return bit state
}

void * find_frame()
{
	for (u32 i = 0; i < (frame_size / 32); i++)
	{
		if (frame_mem[i] == 0xFFFFFFFF)
			continue;
		
		for (u32 ii = 0; ii < 32; ii++)
		{
			if( !( frame_mem[i] & (1 << ii) ) )
				return (void *)(((i * 32) + ii) * 0x1000);
		}
	}
	return NULL;
}

void pageframe_init(u32 mem_size)
{
	log_printf(LOG_INFO, "Initializing, mem size = %u Mib", mem_size / 1024);
	
	frame_size = mem_size / 4;     /* 4k blocks */
	log_printf(LOG_WARNING, "Allocating %u bytes of memory", frame_size / 8);
	frame_mem  = kmalloc(frame_size / 8 /* blocks are 1 bit and kmalloc allocates size in bytes */, false, NULL);
	memset(frame_mem, 0, frame_size / 8);
	
	log_printf(LOG_DEBUG, "frame_size = %u, frame = 0x%08x", frame_size, frame_mem);
	
	// Set the kernel memory as used
	set_frames(0x0, 0x100000, FRAME_SET);
	log_debug("set kernel physical memory as allocated");
	
	// Check if the bootloader kindly provides us with a memory map
	if (mbinfo->flags & MULTIBOOT_INFO_MEM_MAP)
	{
		log_info("Setting up memory map from multiboot");
		log_debug("mmap info: len 0x%x, addr 0x%p", mbinfo->mmap_length, mbinfo->mmap_addr);
		
		u32 mem = mbinfo->mmap_addr;
		while(mem < (mbinfo->mmap_addr + mbinfo->mmap_length))
		{
			multiboot_memory_map_t * mmap_e = (multiboot_memory_map_t *)mem;
			
			// Set memory as used
			if (mmap_e->type == MULTIBOOT_MEMORY_RESERVED)
			{
				u32 end_addr = (u32)mmap_e->addr + (u32)mmap_e->len;
				
				if (end_addr < mmap_e->addr) // HACKY: "handle" 64 bit addresses
					end_addr = 0xFFFFEFFF;
				
				set_frames((u32)mmap_e->addr, end_addr, FRAME_SET);
				log_debug("mmap: set memory from 0x%p to 0x%p as used", (u32)mmap_e->addr, end_addr);
			}
			
			mem += mmap_e->size + sizeof(mmap_e->size);
		}
		
	}
		
	//u32 mmap_length;
	//u32 mmap_addr;
	
}
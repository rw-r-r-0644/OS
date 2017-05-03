#include <mem/kmalloc.h>
#include <mem/page_frame.h>

#include <stdint.h>
#include <string.h>
#include <utils/logger.h>

u32 * frame_mem;
u32 frame_size;

void set_frame(u32 * aligned_addr, frame_status_t state)
{
	aligned_addr = (u32*)((u32)aligned_addr / 0x1000);
	
	u32 idx = (u32)aligned_addr / 32;	// Each block of frames is 32 bits
	u32 bit = (u32)aligned_addr % 32;
	
	(state == FRAME_SET) ?	( frame_mem[idx] |= (1 << bit) ) : // Set the bit
							( frame_mem[idx] &= ~(1 << bit) ); // Clear the bit
}

frame_status_t test_frame(u32 * aligned_addr)
{
	aligned_addr = (u32*)((u32)aligned_addr / 0x1000);
	
	u32 idx = (u32)aligned_addr / 32;	// Each block of frames is 32 bits
	u32 bit = (u32)aligned_addr % 32;

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
	
	// Set the kernel as "NOT FREE"
	for(u32 i = 0; i < 0x100000; i += 0x1000)
		set_frame((u32 *)i, FRAME_SET);
	
	log_printf(LOG_DEBUG, "set kernel physical memory as allocated");
}
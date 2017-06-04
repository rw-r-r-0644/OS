#include <mem/kmalloc.h>
#include <utils/logger.h>
#include <string.h>

/*
 * This should be computed at link time, but an hardcoded
 * value is fine for now. Remember that our kernel starts
 * at 1M as defined on the Makefile
*/

u32 placement_address = 0x10000;

// At this early in boot we can assume that allocated pages will never need to be free'd
void *kmalloc(size_t size, bool align, u32 *phys_addr)
{
    // Pages are 4K aligned (0x1000)
    if (align == true && (placement_address & 0xFFFFF000))
	{
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
	
    // Save also the physical address
    if (phys_addr)
		*phys_addr = placement_address;

    u32 ret = placement_address;
    placement_address += size; // Increment the free memory pointer
	
    return (void *)ret;
}

void *kcalloc(size_t size, bool align, u32 *phys_addr)
{
	return memset(kmalloc(size, align, phys_addr), 0, size);
}
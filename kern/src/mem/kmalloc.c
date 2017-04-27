#include <mem/kmalloc.h>
#include <utils/logger.h>
#include <lib/string.h>
#include <lib/assert.h>

// The usabe memory area goes from 0x10000 to 0x9fc00
uint32_t placement_address = 0x10000;

// At this early in boot we can assume that allocated pages will never need to be free'd
void *kmalloc(size_t size, bool align)
{
    assert(placement_address < 0x9fc00);

    // Pages are 4K aligned (0x1000)
    if (align == true && (placement_address & 0xFFFFF000))
	{
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    uint32_t ret = placement_address;
    placement_address += size; // Increment the free memory pointer
	
    return (void *)ret;
}

void *kcalloc(size_t size, bool align)
{
	return memset(kmalloc(size, align), 0, size);
}
/*
 * heap.c
 * IT'S BAD
*/

#include <mem/heap.h>
#include <mem/paging.h>
#include <mem/page_frame.h>
#include <mem/kmalloc.h>

#include <utils/logger.h>
#include <stdint.h>
#include <stdbool.h>

heap_t kernel_heap;

void heap_create(heap_t * heap, u32 start, u32 end, u32 attrib)
{	
	start &= 0xFFFFF000; // Make sure adresses are page aligned
	end   &= 0xFFFFF000;
	
	log_printf(LOG_DEBUG, "h=%08x, start=%08x, end=%08x, s=%08x", heap, start, end, end - start);
	
	// Setup heap attributes (executable, userspace, ...)
	for (u32 i = start; i < end; i += 0x1000)
	{
		void * addr = find_frame();
		map_page(kernel_pd, addr, (void *)i, attrib);
		set_frame(addr, FRAME_SET);
		//log_printf(LOG_DEBUG, "frame phys=%08x,virt=%08x", addr, i);
	}
	
	heap->heap_magic	= HEAP_MAGIC;
	heap->start			= start;
	heap->end			= end;
	heap->attrib		= attrib;
	
	// Place an initial memory header
	heap_header_t * initial_h = (void *)heap->start;
	initial_h->magic	= MAGIC;
	initial_h->free		= true;
	initial_h->mem		= heap->start + sizeof(heap_header_t);
	initial_h->size		= heap->end - initial_h->mem;
}

int heap_expand(heap_t * heap, u32 expansion_bytes)
{
	u32 new_size = (heap->end + expansion_bytes) & 0xFFFFF000;
	
	// Make sure the pages after the heap are free and we can use them
	for (u32 i = heap->end; i < new_size; i += 0x1000)
		if(get_physaddr(kernel_pd, (void *)i))
			return -1;
	
	// Allocate the pages
	for (u32 i = heap->end; i < new_size; i += 0x1000)
	{
		void * addr = find_frame();
		map_page(kernel_pd, addr, (void *)i, heap->attrib);
		set_frame(addr, FRAME_SET);
	}
	
	// Create a new heap header for the new space
	heap_header_t * h;
	h = (heap_header_t *)heap->end;
	h->magic = MAGIC;
	h->free  = true;
	h->mem   = heap->end + sizeof(heap_header_t);
	h->size  = expansion_bytes - sizeof(heap_header_t);
	
	// Expans the heap
	heap->end += expansion_bytes;
	
	return 0;
}

int heap_reduce(heap_t * heap, u32 reduce_bytes)
{
	u32 new_size = (heap->end - reduce_bytes) & 0xFFFFF000;
	if (new_size < HEAP_MIN_SIZE)
		return -1;
	
	// Free up the pages
	for (u32 i = new_size; i < heap->end; i += 0x1000)
	{
		void * addr = get_physaddr(kernel_pd, (void *)i);
		unmap_page(kernel_pd, (void *)i);
		set_frame(addr, FRAME_CLEAR);
	}
	
	heap->end -= reduce_bytes;
	
	return 0;
}

void * heap_alloc(heap_t * heap, u32 size)
{
	if (size <= 0)
		return NULL;
	
	heap_header_t * h;
	for (u32 i = heap->start; i < (heap->end - sizeof(heap_header_t)); i++)
	{
		h = (heap_header_t *)i;
		if (h->magic == MAGIC)	// Valid header
		{
			if ( (h->free  == true)  &&	// Free
				 (h->size  >= size) )	// Enough space
			{
				if (h->size == size) // Easy
				{
					h->free = false;
					return (void *)h->mem;
				}
				else if ( (h->size - size) > (sizeof(heap_header_t) + 1) )
				{
					// Create a new header for the remaining space
					heap_header_t * new = (heap_header_t *)(h->mem + size);
					new->magic	= MAGIC;
					new->free	= true;
					new->mem	= h->mem  + size + sizeof(heap_header_t);
					new->size	= h->size - size - sizeof(heap_header_t);
					
					// Fix the old header
					h->free = false;
					h->size	= size;
					
					return (void *)h->mem;
				}
			}
			else
			{
				// That's a valid header, go faster
				i += h->size;
			}
		}
	}
	return NULL;
}

void heap_free(heap_t * heap, void * addr)
{
	heap_header_t * h;
	h = (heap_header_t *)((u32)addr - sizeof(heap_header_t));
	if (h->magic == MAGIC)
	{
		h->free = true;
		
		// Try to join this header with another
		heap_header_t * next = (heap_header_t *)(h->mem + h->size);
		
		if ((u32)next > (heap->end - sizeof(heap_header_t)))
		{
			// Going any further would trigger a page fault, return
			return;
		}
		
		if ( (next->magic == MAGIC) &&
			 (next->free  == true) )
		{
			// We can join the two headers!
			log_printf(LOG_DEBUG, "Joining header with the next one :D");
			
			// Expand out header
			h->size += next->size + sizeof(heap_header_t);
			
			// Clean out the old header
			next->magic = 0;
			
			// Return
			return;
		}
	}
}
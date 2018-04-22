/*
 * mm.c
 * Memory Manager
 */

#include <mem/mm.h>
#include <mem/page_frame.h>
#include <mem/paging.h>
#include <mem/kernel.h>
#include <boot/mboot.h>
#include <lib/stdlib.h>
#include <utils/utils.h>
#include <utils/logger.h>

// Base Mappings
mem_seg_t s_paging = {.type = MAP_DYNAMIC, .name = "paging", .size = 0x400000, .next = NULL};
mem_seg_t s_stack = {.type = MAP_DYNAMIC, .name = "stack", .size = 0x80000, .next = &s_paging};
mem_seg_t s_heap = {.type = MAP_STATIC, .name = "heap", .size = 0xA00000, .next = &s_stack};
mem_seg_t s_kernel = {.type = MAP_STATIC | MAP_KERN | MAP_FIXED, .name = "kernel", .next = &s_heap};
mem_seg_t s_video = {.type = MAP_STATIC | MAP_FIXED, .name = "video", .pstart = 0xB8000, .vstart = 0xB8000, .size = 0xFA0, .next = &s_kernel};
mem_seg_t s_kmalloc = {.type = MAP_STATIC | MAP_FIXED, .name = "kmalloc", .pstart = 0x10000, .vstart = 0x10000, .size = 0x8FC00, .next = &s_video};

// Start of the list 
mem_seg_t *s_start = &s_kmalloc;

// Current used offset from kernel end
size_t kernel_voffset = 0x1000;

void mm_print_segments()
{
	log_info("memory segments:\n");
	int i = 0;
	mem_seg_t *s = s_start;
	while(s)
	{
		log_info("[%X] \"%s\":  \tvirt=%08X, phys=%08X, size=%08X", i++, s->name, s->vstart, s->pstart, s->size);
		s = s->next;
	}
}

// Add a new memory segment
mem_seg_t *mm_create_segment(char *name, uintptr_t phys, uintptr_t virt, uintptr_t size, map_type_t type)
{
	mem_seg_t *last = s_start;
	while(last->next) last = last->next;

	mem_seg_t *new = malloc(sizeof(mem_seg_t));

	if((type & MAP_VIRT) == 0)
	{
		kernel_voffset = ALIGN_UP(kernel_voffset, 0x1000);
		new->vstart = kernel_end + kernel_voffset;
		kernel_voffset += size;
	}

	if(new->type & MAP_PHYS)
		paging_map_range(kernel_pd, new->pstart, new->vstart, new->size, PG_KERN);
	else
		paging_map_virt_range(kernel_pd, new->vstart, new->size, PG_KERN);

	last->next = new;
	new->next = NULL;

	return new;
}

// Initialize memory management
void mm_init()
{
	// Initialize physical memory allocator
	pageframe_init(mbinfo->mem_lower + mbinfo->mem_upper);

	// Initialize paging
	paging_init();

	// Create and map all the sections in virtual memory
	mem_seg_t *s = s_start;
	while(s)
	{
		if(s->type & MAP_KERN)
		{
			s->pstart = kernel_start;
			s->vstart = kernel_start;
			s->size = kernel_end - kernel_start;
		}
		
		if((s->type & MAP_VIRT) == 0)
		{
			kernel_voffset = ALIGN_UP(kernel_voffset, 0x1000);
			s->vstart = kernel_end + kernel_voffset;
			kernel_voffset += s->size;
		}

		if(s->type & MAP_PHYS)
			paging_map_range(kernel_pd, s->pstart, s->vstart, s->size, PG_KERN);
		else
			paging_map_virt_range(kernel_pd, s->vstart, s->size, PG_KERN);

		s = s->next;
	}

	// Enable paging
	paging_enable();
}
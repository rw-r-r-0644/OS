/*
 * mm.h
 * Memory Manager
 */

#pragma once
#include <lib/stdint.h>

typedef enum
{
	MAP_STATIC  = (1 << 0),				// The mapping should be consistent between processes
	MAP_DYNAMIC = (1 << 1),				// The mapping uses different physical memory for each process, but is copied when cloning processes
	MAP_VIRT = (1 << 2),				// Use specified virtual memory area
	MAP_PHYS = (1 << 3),				// Use specified physical memory area
	MAP_FIXED = MAP_VIRT | MAP_PHYS,	// Use specified virtual and physical memory area
	MAP_KERN = (1 << 4),				// Uses kernel memory area and physical memory
} map_type_t;

typedef struct _mem_seg
{
	map_type_t type;		// Section type
	char *name;				// Section name
	uintptr_t size;			// Size
	uintptr_t pstart;		// Physical memory
	uintptr_t vstart;		// Virtual memory
	struct _mem_seg *next;	// Next segment
} mem_seg_t;

// Initial memory segments
extern mem_seg_t s_paging;
extern mem_seg_t s_stack;
extern mem_seg_t s_heap;
extern mem_seg_t s_kernel;

// Dump segment info on the screen
void mm_print_segments();

// Add a new memory segment
mem_seg_t *mm_create_segment(char *name, uintptr_t phys, uintptr_t virt, uintptr_t size, map_type_t type);

// Initialize memory management
void mm_init();
#pragma once

#include <stdint.h>

typedef enum
{
	FRAME_SET = 1,
	FRAME_CLEAR = 0
} frame_status_t;

// Change a memory bit status
void set_frame(u32 aligned_addr, frame_status_t state);

// Change multiple memory bits status
void set_frames(u32 adress_start, u32 address_end, frame_status_t state);

// Check a memory bit status
frame_status_t test_frame(u32 aligned_addr);

// Find a free frame
uintptr_t find_frame();

// Initialize page frame allocation
void pageframe_init(u32 mem_size);
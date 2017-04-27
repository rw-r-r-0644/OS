#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	u32 heap_magic;
	u32 start;
	u32 end;
	u32 attrib;
} heap_t;

typedef struct
{
	u32 magic	: 24;
	bool free	: 8;
	u32 mem		: 32;
	u32 size	: 32;
} heap_header_t;

#define HEAP_MIN_SIZE 0x100

#define HEAP_MAGIC	0x48454150 // 32 bits
#define MAGIC		0xCAFE42   // 24 bits

// Default kernel's heap
extern heap_t kernel_heap;

// Create a new heap
void heap_create(heap_t * heap, u32 start, u32 end, u32 attrib);

// Expand the heap
int heap_expand(heap_t * heap, u32 expansion_bytes);

// Reduce the size of the heap
int heap_reduce(heap_t * heap, u32 reduce_bytes);

// alloc()
void * heap_alloc(heap_t * heap, u32 size);

// free()
void heap_free(heap_t * heap, void * addr);
#pragma once
#include <lib/stdint.h>
#include <lib/stdbool.h>

#define PG_KERN		3

// Kernel's page table
extern uint32_t* kernel_pd;

// Create a page directory
uint32_t* create_pd();

// Switch to a page directory
void switch_pd(uint32_t* pd);

// Paging functions
void paging_map(uint32_t* pd, uintptr_t phys, uintptr_t virt, unsigned attrib);
void paging_map_range(uint32_t* pd, uintptr_t phys, uintptr_t virt, size_t size, unsigned attrib);
void paging_map_virt_range(uint32_t* pd, uintptr_t virt, size_t size, unsigned attrib);
void paging_unmap(uint32_t* pd, uintptr_t virt);
void paging_unmap_range(uint32_t* pd, uintptr_t virt, size_t size);

// Enable paging
void paging_enable();

// Check if paging is enabled
bool paging_enabled();

// Initialize paging
void paging_init();
#pragma once

#include <stdint.h>

// Kernel's page table
u32* kernel_pd;

// Create a page directory
u32* create_pd();

// Switch to a page directory
void switch_pd(u32* pd);

// Map a page
int map_page(u32* pd, void* physaddr, void* virtualaddr, u32 attrib);

// Map a memory range
int map_pages(u32* pd, void* physaddr_start, void* physaddr_end, void* virtualaddr_start, void* virtualaddr_end, u32 attrib);

// Unmap a page
int unmap_page(u32* pd, void* virtualaddr);

// Unmap a memory range
int unmap_pages(u32* pd, void* virtualaddr_start, void* virtualaddr_end);

// Set a page's attributes
int page_set_attrib(u32* pd, void* address, u32 attrib);

// Get physical address from virtual address
void* get_physaddr(u32* pd, void* virtualaddr);

// Initialize paging
void paging_init();
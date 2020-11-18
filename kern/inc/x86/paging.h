#pragma once

#define PAGE_SIZE		0x1000
#define PAGE_MASK		0xfffff000
#define PAGE_SHIFT		12

#define PAGE_PRESENT	(1 << 0)
#define PAGE_WRITABLE	(1 << 1)
#define PAGE_USER		(1 << 2)
#define PAGE_WTCACHE	(1 << 3)
#define PAGE_NOCACHE	(1 << 4)
#define PAGE_ACCESSED	(1 << 5)
#define PAGE_DIRTY		(1 << 6)
#define PAGE_GLOBAL		(1 << 8)

#define MAP_KERNEL		(PAGE_PRESENT | PAGE_WRITABLE)
#define MAP_IODEV		(PAGE_PRESENT | PAGE_WRITABLE)


void
read_physaddr(unsigned long physaddr, void *buffer, unsigned long size);

unsigned long
get_physaddr(void *virtaddr);

void
map_page(unsigned long physaddr, void *virtaddr, unsigned long flags);

void
unmap_page(void *virtaddr);

void
paging_finalize();

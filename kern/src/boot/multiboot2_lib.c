/*
 * multiboot2_lib.c 
 * 
 * Multiboot2 helper functions
 */

#include <stddef.h>
#include <lib/stdint.h>
#include <lib/stdlib.h>
#include <boot/multiboot2.h>
#include <kernel/utils.h>
#include <x86/paging.h>
#include <mm/pmm.h>

unsigned long
multiboot2_info_pa;


/* multiboot2_info_size
 * Get total size of multiboot2 info. */
static uint32_t
multiboot2_info_size()
{
	static uint32_t size = 0;

	if (!size)
	{
		read_physaddr(
			multiboot2_info_pa,
			&size,
			sizeof(uint32_t)
		);
	}

	return size;
}


/* multiboot2_get_tag
 * Find and read a multiboot2 tag. */
void *
multiboot2_get_tag(uint32_t type)
{
	size_t offset = 8;
	struct multiboot_tag tag;
	void *res = NULL;

	while (offset < multiboot2_info_size())
	{
		offset = _ALIGN2_UP(offset, 8);

		/* read tag header */
		read_physaddr(
			multiboot2_info_pa + offset,
			&tag,
			sizeof(tag)
		);

		/* match tag type to the request */
		if (tag.type != type)
		{
			offset += tag.size;
			continue;
		}

		/* tag found, read it to heap */
		res = malloc(tag.size);
		read_physaddr(
			multiboot2_info_pa + offset,
			res,
			tag.size
		);

		break;
	}

	return res;
}


/* multiboot2_load_memory_map
 * Parse memory map from multiboot2, and
 * provide informations to the pmm. */
void
multiboot2_load_memory_map()
{
	unsigned long round, offset, start, end;
	struct multiboot_tag_mmap *b_mmap;
	multiboot_memory_map_t *entry;

	/* parse multiboot mmap entries */
	b_mmap = multiboot2_get_tag(MULTIBOOT_TAG_TYPE_MMAP);
	for (round = 0; round < 2; round++)
	{
		for (offset = offsetof(struct multiboot_tag_mmap, entries);
			 offset < b_mmap->size;
			 offset += b_mmap->entry_size)
		{
			entry = (multiboot_memory_map_t *)((unsigned long)b_mmap + offset);
			start = entry->addr,
			end = start + entry->len - 1;

			/* ensure the entry is part of address space */
			if (start > ~0ul)
				continue;
			if (end > ~0ul)
				end = ~0ul;

			/* add available memory on round 0,
			 * reserved memory on round 1 */
			if (round ^ (entry->type == MULTIBOOT_MEMORY_AVAILABLE))
				pmm_add_memory((unsigned long)start, (unsigned long)end);
		}
	}
	free(b_mmap);
}

/* multiboot2_get_memory_end
 * Get the last valid physical address */
unsigned long
multiboot2_get_memory_end()
{
	struct multiboot_tag_mmap *b_mmap;
	multiboot_memory_map_t *entry;
	unsigned long offset;
	uint64_t end = 0;

	/* find last entry end adress */
	b_mmap = multiboot2_get_tag(MULTIBOOT_TAG_TYPE_MMAP);
	for (offset = offsetof(struct multiboot_tag_mmap, entries);
		 offset < b_mmap->size;
		 offset += b_mmap->entry_size)
	{
		entry = (multiboot_memory_map_t *)((unsigned long)b_mmap + offset);
		if (entry->addr > ~0ul)
			continue;
		if ((entry->addr + entry->len - 1) > end)
			end = entry->addr + entry->len - 1;
	}
	free(b_mmap);

	return (end > ~0ul) ? ~0ul : (unsigned long)end;
}


/* multiboot2_reserve_memory
 * Reserve memory used by the multiboot2 info. */
void
multiboot2_reserve_memory()
{
	/* reserve memory used by multiboot2 info */
	pmm_reserve_memory(
		(uintptr_t)multiboot2_info_pa,
		multiboot2_info_size()
	);

	/* TODO: reserve modules memory ? */
}



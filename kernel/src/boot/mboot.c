#include <boot/mboot.h>
#include <boot/mboot_mods.h>
#include <mem/mm.h>
#include <utils/logger.h>

/* Multiboot Informations */
multiboot_info_t *mbinfo;

/*  Map multiboot modules for paging */
u8* mboot_modules[40];

void mboot_map_mods(multiboot_info_t * mboot_info)
{
	if (mboot_info->mods_count > 40)
		mboot_info->mods_count = 40;
	
	for (u32 i = 0; i < mboot_info->mods_count; i++)
	{
		multiboot_module_t *mod = (multiboot_module_t *)(mboot_info->mods_start + sizeof(multiboot_module_t) * i);		
		mem_seg_t *s = mm_create_segment("Multiboot Module", mod->mod_start, 0, (mod->mod_end - mod->mod_start), MAP_STATIC | MAP_PHYS);
		mboot_modules[i] = (u8*)s->vstart;
	}
}
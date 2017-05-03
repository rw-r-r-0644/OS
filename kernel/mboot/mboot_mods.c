/* 
 * Map multiboot modules for paging
*/

#include <mboot/mboot_mods.h>
#include <mboot/mboot.h>

#include <mem/paging.h>

#include <utils/logger.h>

u8* mboot_modules[40];

void mboot_map_mods(multiboot_info_t * mboot_info)
{
	if (mboot_info->mods_count > 40)
		mboot_info->mods_count = 40;
	
	for (u32 i = 0; i < mboot_info->mods_count; i++)
	{
		multiboot_module_t * mboot_mod = (multiboot_module_t *)(mboot_info->mods_start + sizeof(multiboot_module_t) * i);
		
		log_printf(LOG_INFO, "Mapping mod %u, start = 0x%p, end = 0x%p ", i, mboot_mod->mod_start, mboot_mod->mod_end);
		identity_map(kernel_pd, (void *)mboot_mod->mod_start, (void *)mboot_mod->mod_end, 3);
		
		mboot_modules[i] = (u8*)mboot_mod->mod_start;
	}
}
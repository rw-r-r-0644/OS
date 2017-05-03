#include <mboot/mboot.h>

#define MULTIBOOT_MAGIC (MULTIBOOT_HEADER_MAGIC)
#define MULTIBOOT_FLAGS (MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO)

/* Multiboot Header */
multiboot_header_t mbheader __attribute__((section (".multiboot"))) = 
{
	.magic			= MULTIBOOT_MAGIC,
	.flags			= MULTIBOOT_FLAGS,
	.checksum		= -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS),
	
	.header_addr	= 0,
	.load_addr		= 0,
	.load_end_addr	= 0,
	.bss_end_addr	= 0,
	.entry_addr		= 0,
	
	.mode_type		= 0,
	.width			= 0,
	.height			= 0,
	.depth			= 0,
};

/* Multiboot Informations */
multiboot_info_t * mbinfo;
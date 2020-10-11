#define ASM_FILE        1
#include "boot/multiboot2.h"

.section .multiboot

# Multiboot header
.align  MULTIBOOT_HEADER_ALIGN
multiboot_header:
	.long MULTIBOOT2_HEADER_MAGIC
	.long MULTIBOOT_ARCHITECTURE_I386
	.long multiboot_header_end - multiboot_header
	.long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))

	# Information request tag
	.align MULTIBOOT_TAG_ALIGN
	information_request_tag_start:
		.short MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST
		.short 0
		.long information_request_tag_end - information_request_tag_start
		.long MULTIBOOT_TAG_TYPE_MMAP
	information_request_tag_end:

	# Module align tag
	.align MULTIBOOT_TAG_ALIGN
	module_align_tag_start:
		.short MULTIBOOT_HEADER_TAG_MODULE_ALIGN
		.short 0
		.long module_align_tag_end - module_align_tag_start
	module_align_tag_end:

	# End tag
	.align MULTIBOOT_TAG_ALIGN
	end_tag_start:
		.short MULTIBOOT_HEADER_TAG_END
		.short 0
		.long end_tag_end - end_tag_start
	end_tag_end:

multiboot_header_end:

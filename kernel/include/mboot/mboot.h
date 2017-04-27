/* multiboot.h - Multiboot header file. */
/* Copyright (C) 1999,2003,2007,2008,2009	Free Software Foundation, Inc.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to
 *	deal in the Software without restriction, including without limitation the
 *	rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *	sell copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT SHALL ANY
 *	DEVELOPER OR DISTRIBUTOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *	IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <stdint.h>

/* How many bytes from the start of the file we search for the header. */
#define MULTIBOOT_SEARCH					8192

/* The magic field should contain this. */
#define MULTIBOOT_HEADER_MAGIC				0x1BADB002

/* This should be in %eax. */
#define MULTIBOOT_BOOTLOADER_MAGIC			0x2BADB002

/* The bits in the required part of flags field we don't support. */
#define MULTIBOOT_UNSUPPORTED				0x0000fffc

/* Alignment of multiboot modules. */
#define MULTIBOOT_MOD_ALIGN					0x00001000

/* Alignment of the multiboot info structure. */
#define MULTIBOOT_INFO_ALIGN				0x00000004

/* Flags set in the 'flags' member of the multiboot header. */

/* Align all boot modules on i386 page (4KB) boundaries. */
#define MULTIBOOT_PAGE_ALIGN				0x00000001

/* Must pass memory information to OS. */
#define MULTIBOOT_MEMORY_INFO				0x00000002

/* Must pass video information to OS. */
#define MULTIBOOT_VIDEO_MODE				0x00000004

/* This flag indicates the use of the address fields in the header. */
#define MULTIBOOT_AOUT_KLUDGE				0x00010000

/* Flags to be set in the 'flags' member of the multiboot info structure. */

/* is there basic lower/upper memory information? */
#define MULTIBOOT_INFO_MEMORY				0x00000001
/* is there a boot device set? */
#define MULTIBOOT_INFO_BOOTDEV				0x00000002
/* is the command-line defined? */
#define MULTIBOOT_INFO_CMDLINE				0x00000004
/* are there modules to do something with? */
#define MULTIBOOT_INFO_MODS					0x00000008

/* These next two are mutually exclusive */

/* is there a symbol table loaded? */
#define MULTIBOOT_INFO_AOUT_SYMS			0x00000010
/* is there an ELF section header table? */
#define MULTIBOOT_INFO_ELF_SHDR				0X00000020

/* is there a full memory map? */
#define MULTIBOOT_INFO_MEM_MAP				0x00000040

/* Is there drive info? */
#define MULTIBOOT_INFO_DRIVE_INFO			0x00000080

/* Is there a config table? */
#define MULTIBOOT_INFO_CONFIG_TABLE			0x00000100

/* Is there a boot loader name? */
#define MULTIBOOT_INFO_BOOT_LOADER_NAME		0x00000200

/* Is there a APM table? */
#define MULTIBOOT_INFO_APM_TABLE								0x00000400

/* Is there video information? */
#define MULTIBOOT_INFO_VIDEO_INFO							 0x00000800

struct multiboot_header
{
	/* Must be MULTIBOOT_MAGIC - see above. */
	u32 magic;

	/* Feature flags. */
	u32 flags;

	/* The above fields plus this one must equal 0 mod 2^32. */
	u32 checksum;

	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	u32 header_addr;
	u32 load_addr;
	u32 load_end_addr;
	u32 bss_end_addr;
	u32 entry_addr;

	/* These are only valid if MULTIBOOT_VIDEO_MODE is set. */
	u32 mode_type;
	u32 width;
	u32 height;
	u32 depth;
};

/* The symbol table for a.out. */
struct multiboot_aout_symbol_table
{
	u32 tabsize;
	u32 strsize;
	u32 addr;
	u32 reserved;
};
typedef struct multiboot_aout_symbol_table multiboot_aout_symbol_table_t;

/* The section header table for ELF. */
struct multiboot_elf_section_header_table
{
	u32 num;
	u32 size;
	u32 addr;
	u32 shndx;
};
typedef struct multiboot_elf_section_header_table multiboot_elf_section_header_table_t;

struct multiboot_mmap_entry
{
	u32 size;
	u64 addr;
	u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE				1
#define MULTIBOOT_MEMORY_RESERVED				2
	u32 type;
} __attribute__((packed));
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

struct multiboot_mod_list
{
	/* the memory used goes from bytes 'mod_start' to 'mod_end-1' inclusive */
	u32 mod_start;
	u32 mod_end;

	/* Module command line */
	u32 cmdline;

	/* padding to take it to 16 bytes (must be zero) */
	u32 pad;
};
typedef struct multiboot_mod_list multiboot_module_t;

struct multiboot_info
{
	/* Multiboot info version number */
	u32 flags;

	/* Available memory from BIOS */
	u32 mem_lower;
	u32 mem_upper;

	/* "root" partition */
	u32 boot_device;

	/* Kernel command line */
	u32 cmdline;

	/* Boot-Module list */
	u32 mods_count;
	u32 mods_start;

	union
	{
		multiboot_aout_symbol_table_t aout_sym;
		multiboot_elf_section_header_table_t elf_sec;
	} u;

	/* Memory Mapping buffer */
	u32 mmap_length;
	u32 mmap_addr;

	/* Drive Info buffer */
	u32 drives_length;
	u32 drives_addr;

	/* ROM configuration table */
	u32 config_table;

	/* Boot Loader Name */
	char * boot_loader_name;

	/* APM table */
	u32 apm_table;

	/* Video */
	u32 vbe_control_info;
	u32 vbe_mode_info;
	u16 vbe_mode;
	u16 vbe_interface_seg;
	u16 vbe_interface_off;
	u16 vbe_interface_len;
};
typedef struct multiboot_info multiboot_info_t;
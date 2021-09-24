ENTRY(_loader)
OUTPUT_FORMAT(elf32-i386)

/* The kernel will live at 3GB + 1MB in the virtual
 * address space, which will be mapped to 1MB in the
 * physical address space. */

__bootheap_size = 0x80000;

SECTIONS
{
	. = 0xC0100000;
	__kernel_start = .;

	.text ALIGN(4K) : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
		*(.rodata*)
	}

	.rodata ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	.heap ALIGN(4K) : AT(ADDR(.heap) - 0xC0000000)
	{
		__bootheap_start = .;
		. = . + __bootheap_size;
		__bootheap_end = .;
	}

	__kernel_end = .;
}

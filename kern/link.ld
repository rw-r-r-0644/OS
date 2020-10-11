ENTRY(_loader)
OUTPUT_FORMAT(elf32-i386)

/* The kernel will live at 3GB + 1MB in the virtual
 * address space, which will be mapped to 1MB in the
 * physical address space. */

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

	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	__kernel_end = .;
}

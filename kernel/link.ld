/* Define the entry point */
ENTRY(entrypoint)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* The Multiboot bootloader should load the kernel at 1Mib */
	. = 1M;
	p_kernel_start = .;

	/* Align multiboot info and text to 4Kb */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	/* Other sections wil leventually be put here */
	
	p_kernel_end = .;
}
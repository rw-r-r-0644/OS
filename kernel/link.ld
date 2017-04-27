/* Define the entry point */
ENTRY(entrypoint)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* The Multiboot bootloader should load the kernel at 1Mib */
	. = 1M;
	kernel_start = .;

	/* Align multiboot info and text to 4Kb */
	kernel_text_start = .;
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}
	kernel_text_end = .;

	/* Read-only data. */
	kernel_rodata_start = .;
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
	kernel_rodata_end = .;

	/* Read-write data (initialized) */
	kernel_data_start = .;
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
	kernel_data_end = .;

	/* Read-write data (uninitialized) and stack */
	kernel_bss_start = .;
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	kernel_bss_end = .;

	/* Other sections wil leventually be put here */
	
    kernel_end = .;
}
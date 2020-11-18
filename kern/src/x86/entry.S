.extern kern_main
.extern enable_isrs
.extern enable_irqs

.extern __kernel_start
.extern __kernel_end

.extern bootpagedir
.extern multiboot2_info_pa

.section .bss
	# stack
	.align 16
	stack_base:
	.skip 32768 # 32KiB
	stack_top:

.equ KERNEL_VIRT_BASE,    0xC0000000                # virtual memory offset
.equ KERNEL_PD_VIRT_BASE, (KERNEL_VIRT_BASE >> 20)

.section .data
	.align 4096
	gdt_start: # GDT
		.long 0x00000000, 0x00000000    # 0x00: null segment
		.long 0x0000FFFF, 0x00CF9A00    # 0x08: code segment
		.long 0x0000FFFF, 0x00CF9200    # 0x10: data segment
	gdt_desc:  # GDT decriptor
		.word gdt_desc - gdt_start - 1      # size
		.long gdt_start - KERNEL_VIRT_BASE  # base

.section .text
	# kernel entrypoint
	.global _loader
	_loader:

	# setup GDT
	_loader.gdt_setup:
		lgdt gdt_desc - KERNEL_VIRT_BASE
		ljmp $0x08, $(_loader.gdt_reload_segments - KERNEL_VIRT_BASE)

	# reload GDT segments
	_loader.gdt_reload_segments:
		movw $0x10,%ax
		movw %ax,%ds
		movw %ax,%ss
		movw %ax,%es
		movw %ax,%fs
		movw %ax,%gs

	# setup paging
	_loader.setup_paging:
		# enable 4 Mib pages
		movl %cr4, %ecx
		orl $0x00000010,%ecx
		movl %ecx, %cr4

		# load boot page directory
		movl $(bootpagedir - KERNEL_VIRT_BASE), %ecx
		movl %ecx, %cr3

		# enable paging
		movl %cr0, %ecx
		orl $0x80000000,%ecx
		movl %ecx, %cr0

		# long jump to higher half entrypoint
		ljmp $0x08, $_highmem_entrypoint

	# higher half entrypoint
	_highmem_entrypoint:

	# setup stack
	_highmem_entrypoint.stack_setup:
		movl $stack_base, %esp

	# early kernel setup
	_highmem_entrypoint.early_kernel:
		# save multiboot info
		movl %ebx, multiboot2_info_pa

		# setup interrupts/exceptions handling
		call idt_init

	# start the kernel
	_highmem_entrypoint.start_kernel:
		call kern_main

	.global deadlock
	deadlock:
		cli
		hlt
		jmp deadlock

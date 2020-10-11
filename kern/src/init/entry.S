
.extern kern_main
.extern enable_isrs
.extern enable_irqs

.extern __kernel_start
.extern __kernel_end

.section .multiboot
    # multiboot header
    .equ MULTIBOOT_MAGIC,    0x1BADB002                             # magic number
    .equ MULTIBOOT_FLAGS,    (0x1 | 0x2)                            # flags  (0x1: align modules, 0x2: provide memory map)
    .equ MULTIBOOT_CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)   # checksum
    .align 4
    .long MULTIBOOT_MAGIC
    .long MULTIBOOT_FLAGS
    .long MULTIBOOT_CHECKSUM

.section .bss
    # stack
    .align 16
    stack_base:
    .skip 32768 # 32KiB
    stack_top:

    # base pagedir
    .align 4096
    .global kernel_pd
    kernel_pd:
    .skip 4096

.equ KERNEL_VIRT_BASE,    0xC0000000                # virtual memory offset
.equ KERNEL_PD_VIRT_BASE, (KERNEL_VIRT_BASE >> 20)

.section .data
    gdt_start: # GDT
        .long 0x00000000, 0x00000000    # 0x00: null segment
        .long 0x0000FFFF, 0x00CF9A00    # 0x08: code segment
        .long 0x0000FFFF, 0x00CF9200    # 0x10: data segment
    gdt_desc:  # GDT decriptor
        .word gdt_desc - gdt_start - 1      # size
        .long gdt_start - KERNEL_VIRT_BASE  # base

	# multiboot info
	#.global mbinfo
	#mbinfo: .long 0

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

	# compute page aligned kernel size
    _loader.get_kern_size_pgalign:
        # get kernel size (ecx = kernel size)
        movl $(__kernel_end - KERNEL_VIRT_BASE),%ecx

        # align kernel size if needed (ecx = kernel size, edx = size % 0x400000)
        movl %ecx,%edx
        andl $0x3FFFFF,%edx
        cmpl $0,%edx
        je _loader.compute_save_pg_count
        subl %edx,%ecx
        addl $0x400000,%ecx

	# compute the number of needed pages and save them in esi for later
    _loader.compute_save_pg_count:
        # get the number of needed 4mib pages (ecx = num pages)
        shrl $22,%ecx

		# save it for later unmapping
		movl %ecx,%edi

	# map kernel to higher half
    _loader.map_loop:
        decl %ecx                       # this assumes we map at least 1 page...
        movl %ecx,%edx                  #
        shll $22,%edx                   # page frame
        orl $0x83,%edx                  # present, rw, 4mib
        leal kernel_pd(,%ecx,4),%eax
        subl $KERNEL_VIRT_BASE, %eax    # get pagedir entry address
        movl %edx,(%eax)                # identity map
        addl $KERNEL_PD_VIRT_BASE, %eax # add virtual mapping offset to pagedir entry
        movl %edx,(%eax)                # map to high memory
        cmpl $0,%ecx                    # check if we finished mapping
        jne _loader.map_loop            # jump to loop if we did not

        # enable 4 Mib pages
        movl %cr4, %ecx
        orl $0x00000010,%ecx
        movl %ecx, %cr4

        # load page directory
        movl $(kernel_pd - KERNEL_VIRT_BASE), %ecx
        movl %ecx, %cr3

        # enable paging
        movl %cr0, %ecx
        orl $0x80000000,%ecx
        movl %ecx, %cr0

        # long jump to higher half entrypoint
        ljmp $0x08, $_highmem_entrypoint

	# higher half entrypoint
    _highmem_entrypoint:

	# unmap kernel identity mapping
    _highmem_entrypoint.unmap_loop:
        decl %edi
        leal kernel_pd(,%edi,4),%eax
        movl $0,(%eax)
        cmpl $0,%edi
        jne _highmem_entrypoint.unmap_loop

	# setup stack
    _highmem_entrypoint.stack_setup:
        movl $stack_base, %esp

	# early kernel setup
	_highmem_entrypoint.early_kernel:
		# save multiboot info
        #movl %ebx,mbinfo

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

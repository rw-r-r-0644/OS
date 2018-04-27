

extern mbinfo
extern kernel_early
extern kernel_main
extern isr_init

extern p_kernel_start
extern p_kernel_end

section .multiboot
	; multiboot header
	MULTIBOOT_MAGIC		equ		0x1BADB002								; magic number
	MULTIBOOT_FLAGS		equ		0x1 | 0x2								; flags (0x1: align modules, 0x2: provide memory map)
	MULTIBOOT_CHECKSUM	equ		-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)	; checksum
	align 4
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

section .bss
	; stack (32KiB)
	align 8
	stack_base:	resb 32768
	stack_top:

	align 4096
	global kernel_pd
	kernel_pd:
		resb 4096

KERNEL_VIRT_BASE		equ		0xC0000000					; virtual memory offset
KERNEL_PAGE_BASE		equ		(KERNEL_VIRT_BASE >> 22)	;
KERNEL_PAGEDIR_BASE		equ		(KERNEL_PAGE_BASE << 2)

section .data
	; gdt stub (paging is used)
	gdt_start:	; 0x00: null segment
		dd 0x00000000
		dd 0x00000000
	gdt_code:	; 0x08: code segment
		dd 0xffff0000
		dd 0x009acf00
	gdt_data:	; 0x10: data segment
		dd 0xffff0000
		dd 0x0092cf00
	gdt_end:

	; gdt descriptor
	gdt_descriptor:
		dw gdt_end - gdt_start - 1
		dd gdt_start

section .text
	; kernel entrypoint
	global _loader
	_loader:
		; get kernel size (ecx = kernel size)
		mov ecx, p_kernel_end - KERNEL_VIRT_BASE
	
		; align kernel size if needed (ecx = kernel size, edx = size % 0x400000)
		mov edx, ecx
		and edx, 0x3FFFFF
		cmp edx, 0
		je .aligned_size
		sub ecx, edx
		add ecx, 0x400000
	.aligned_size:

		; get the number of needed 4mib pages (ecx = num pages)
		shr ecx, 22
	
		; identity map the kernel and map the kernel to kernel offser
	.map_loop:
		dec ecx			; this assumes we map at least 1 page...
		mov edx, ecx	;
		shl edx, 22		; page frame
		or edx, 0x83	; present, rw, 4mib
		lea eax, [kernel_pd + ecx*4]
		sub eax, KERNEL_VIRT_BASE		; get pagedir entry address
		mov [eax], edx					; identity map
		add eax, KERNEL_PAGEDIR_BASE	; add virtual mapping offset to pagedir entry
		mov [eax], edx					; map to high memory
		cmp ecx, 0		; check if we finished mapping
		jne .map_loop	; jump to loop if we did not

		; load page directory
		mov ecx, kernel_pd - KERNEL_VIRT_BASE
		mov cr3, ecx
 
		; enable 4 Mib pages
		mov ecx, cr4
		or ecx, 0x00000010
		mov cr4, ecx
 
		; enable paging
		mov ecx, cr0
		or ecx, 0x80000000
		mov cr0, ecx
 
		; long jump to higher half entrypoint
		lea ecx, [higher_half]
		jmp ecx
	higher_half:

		; setup the GDT
		lgdt [gdt_descriptor]
		jmp 0x08:reload_segments
	reload_segments:
		mov ax, 0x10
		mov ds, ax
		mov ss, ax
		mov es, ax
		mov fs, ax
		mov gs, ax

		; setup stack
		mov esp, stack_base
		mov [mbinfo], ebx

		jmp $

		; setup interrupts/exceptions handling
		call isr_init
	 
		; remap the kernel at 0xC0000000
		call kernel_early	; Enable GDT and paging before the real kernel
	 
		; Call the kernel
		call kernel_main
		
		; We should never reach that ...but it may happen while "debugging"
		jmp $
	.end:
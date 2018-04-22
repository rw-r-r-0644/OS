; Declare constants for the multiboot header.
MBALIGN  equ  1<<0              ; align loaded modules on page boundaries
MEMINFO  equ  1<<1              ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

extern mbinfo
extern kernel_early
extern kernel_main

; Multiboot header
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .bss
	; Reserve 32 Kib of 8 byte aligned space for the stack
	align 8
	stack_bottom:
		resb 32768
	stack_top:
 
; The linker script specifies entrypoint as the entry point
section .text
	global entrypoint:function (entrypoint.end - entrypoint)
	entrypoint:
		; Setup stack
		mov esp, stack_top
		mov [mbinfo], ebx
	 
		; Early kernel entrypoint
		call kernel_early	; Enable GDT and paging before the real kernel
	 
		; Call the kernel
		call kernel_main
		
		; We should never reach that ...but it may happen while "debugging"
		jmp $
	.end:
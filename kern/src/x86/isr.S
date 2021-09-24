.section .text

.extern interrupt_handler

.macro isr num
.global isr\num
isr\num:
	pushl $0
	pushl $\num
	jmp common_interrupt_handler
.endm


.macro isr_err num
.global isr\num
isr\num:
	pushl $\num
	jmp common_interrupt_handler
.endm

common_interrupt_handler:
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %eax
	pushl %esp
	cld
	call interrupt_handler
	addl $4, %esp
	popl %eax
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	addl $8, %esp
	iret

isr 0
isr 1
isr 2
isr 3
isr 4
isr 5
isr 6
isr 7
isr_err 8
isr 9
isr_err 10
isr_err 11
isr_err 12
isr_err 13
isr_err 14
isr 15
isr 16
isr_err 17
isr 18
isr 19
isr 20
isr 21
isr 22
isr 23
isr 24
isr 25
isr 26
isr 27
isr 28
isr 29
isr_err 30
isr 31

isr 32
isr 33
isr 34
isr 35
isr 36
isr 37
isr 38
isr 39
isr 40
isr 41
isr 42
isr 43
isr 44
isr 45
isr 46
isr 47


.section .rodata

.global isr_table
isr_table:
	.long isr0,  isr1,  isr2,  isr3
	.long isr4,  isr5,  isr6,  isr7
	.long isr8,  isr9,  isr10, isr11
	.long isr12, isr13, isr14, isr15
	.long isr16, isr17, isr18, isr19
	.long isr20, isr21, isr22, isr23
	.long isr24, isr25, isr26, isr27
	.long isr28, isr29, isr30, isr31
	.long isr32, isr33, isr34, isr35
	.long isr36, isr37, isr38, isr39
	.long isr40, isr41, isr42, isr43
	.long isr44, isr45, isr46, isr47

.global isr_count
isr_count:
	.long 48

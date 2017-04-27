.extern handle_isr
.extern handle_irq

.macro ISR num
	.global isr\num
	isr\num:
		cli
		pushl $0
		pusha
		movw $0x10, %ax
		movw %ax, %ds
		movw %ax, %es
		movw %ax, %fs
		movw %ax, %gs
		pushl %esp
		pushl $\num
		call handle_isr
		addl $8, %esp
		popa
		addl $4, %esp
		iret
.endm

.macro ISR_ERR num
	.global isr\num
	isr\num:
		cli
		pusha
		movw $0x10, %ax
		movw %ax, %ds
		movw %ax, %es
		movw %ax, %fs
		movw %ax, %gs
		pushl %esp
		pushl $\num
		call handle_isr
		addl $8, %esp
		popa
		addl $4, %esp
		iret
.endm

.macro IRQ num
	.global irq\num
	irq\num:
		cli
		pushl $0
		pusha
		movw $0x10, %ax
		movw %ax, %ds
		movw %ax, %es
		movw %ax, %fs
		movw %ax, %gs
		pushl %esp
		pushl $\num
		call handle_irq
		addl $8, %esp
		popa
		addl $4, %esp
		iret
.endm


ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR_ERR 8
ISR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR 15
ISR 16
ISR_ERR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR_ERR 30
ISR 31


IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15

; Defined in isr.c
extern isr_handler
extern irq_handler

; Define some nasm macros to avoid copypasting functions 47 times (who wants to do that?)

%macro ISR 1		; ISR [isr number]
	global isr%1
	isr%1:
		push byte 0 ; Replace error code with 0
		push byte %1
		jmp isr_common_stub
%endmacro

%macro ISR_ERR 1	; ISR_ERR [isr number]
	global isr%1
	isr%1:
		push byte %1
		jmp isr_common_stub
%endmacro

%macro IRQ 2		; IRQ [isr number], [irq number]
	global irq%2
	irq%2:
		push byte %2
		push byte %1
		jmp irq_common_stub
%endmacro
	
; ISRs
ISR 0		; Divide By Zero Exception
ISR 1		; Debug Exception
ISR 2		; Non Maskable Interrupt Exception
ISR 3		; Int 3 Exception
ISR 4		; INTO Exception
ISR 5		; Out of Bounds Exception
ISR 6		; Invalid Opcode Exception
ISR 7		; Coprocessor Not Available Exception
ISR_ERR 8	; Double Fault Exception (With Error Code!)
ISR 9		; Coprocessor Segment Overrun Exception
ISR_ERR 10	; Bad TSS Exception (With Error Code!)
ISR_ERR 11	; Segment Not Present Exception (With Error Code!)
ISR_ERR 12	; Stack Fault Exception (With Error Code!)
ISR_ERR 13	; General Protection Fault Exception (With Error Code!)
ISR_ERR 14	; Page Fault Exception (With Error Code!)
ISR 15		; Reserved Exception
ISR 16		; Floating Point Exception
ISR 17		; Alignment Check Exception
ISR 18		; Machine Check Exception
ISR 19		; Reserved
ISR 20		; Reserved
ISR 21		; Reserved
ISR 22		; Reserved
ISR 23		; Reserved
ISR 24		; Reserved
ISR 25		; Reserved
ISR 26		; Reserved
ISR 27		; Reserved
ISR 28		; Reserved
ISR 29		; Reserved
ISR 30		; Reserved
ISR 31		; Reserved

; IRQs
IRQ 32, 0
IRQ 33, 1
IRQ 34, 2
IRQ 35, 3
IRQ 36, 4
IRQ 37, 5
IRQ 38, 6
IRQ 39, 7
IRQ 40, 8
IRQ 41, 9
IRQ 42, 10
IRQ 43, 11
IRQ 44, 12
IRQ 45, 13
IRQ 46, 14
IRQ 47, 15


; Common ISR code
isr_common_stub:
    ; Save CPU state
	pusha		; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds	; Lower 16-bits of eax = ds
	
	; Save the current data segment
	push eax
	
	; Switch to kernel data segment
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; Call C handler
	push esp	; registers_t *r
    cld			; C code following the sysV ABI requires DF to be clear on function entry
	call isr_handler
	
    ; Restore CPU state
	pop eax 
    pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8	; Cleans up the pushed error code and pushed ISR number
	iret		; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; Common IRQ code. Identical to ISR code except for the 'call' and the 'pop ebx'
irq_common_stub:
	; Save CPU state
    pusha 
    mov ax, ds
	
	; Save current data segment
    push eax
	
	; Switch to kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
	
	; Call C handler
    push esp			; registers_t *r
    cld
    call irq_handler	; Common IRQ handler
	
	; Restore CPU state
    pop ebx  ; Different than the ISR code
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    iret
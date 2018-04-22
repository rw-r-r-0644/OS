; gdt.s
;
; Sets up an unprotected GDT (overlaps code and data) covering the full memory address
; Protection could easily be setup but it's not really needed when already using paging
;
; Based on os-dev.pdf
; Adapted to work on pretected mode
;

; GDT null segment
gdt_start:
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; GDT code segment
gdt_code: 
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; GDT data segment
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1	; size		(16 bit)
    dd gdt_start				; address	(32 bit)

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Load the GDT
global gdt_init
gdt_init:
	; Load the gdt
	lgdt [gdt_descriptor]
	
	; Far jump to "enable" the GDT
	jmp CODE_SEG:gdt_flush

; Update the segment registers
gdt_flush:
	; Update data segment
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; Return
	ret
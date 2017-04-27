; Doesn't work right now. not really needed until starting the graphics work....
; global fpu_init
; 
; fpu_init:
; 	; Search for an FPU
; 	mov edx, cr0							; Start probe, get cr0
; 	and edx, (-1) - (cr0_ts + cr0_em)		; clear TS and EM to force fpu access
; 	mov cr0, edx							; store control word
; 	fninit									; load defaults to FPU
; 	fnstsw [testword]						; store status word
; 	cmp word [testword], 0					; compare the written status with the expected FPU state
; 	jne nofpu								; jump if the FPU hasn't written anything (i.e. it's not there)
; 	jmp hasfpu
; 	 
; testword dw 0xAABB            		        ; store garbage to be able to detect a change
; 
; hasfpu:
; 	
; 	ret
; 
; nofpu:
; 	ret
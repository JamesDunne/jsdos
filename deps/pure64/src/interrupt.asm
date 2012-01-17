; =============================================================================
; Pure64 -- a 64-bit OS loader written in Assembly for x86-64 systems
; Copyright (C) 2008-2011 Return Infinity -- see LICENSE.TXT
;
; Interrupts
; =============================================================================


; -----------------------------------------------------------------------------
; Default exception handler
exception_gate:
	mov rsi, int_string
	call os_print_string
	mov rsi, exc_string
	call os_print_string
	jmp $					; hang
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Default interrupt handler
interrupt_gate:				; handler for all other interrupts
	iretq
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Real-time clock interrupt. IRQ 0x00, INT 0x20
align 16
timer:
	add qword [os_Counter], 1	; 64-bit counter started at bootup
	mov al, 0x20			; Acknowledge the IRQ
	out 0x20, al
	iretq
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; CPU Exception Gates
exception_gate_00:
	mov al, 0x00
	jmp exception_gate_main

exception_gate_01:
	mov al, 0x01
	jmp exception_gate_main

exception_gate_02:
	mov al, 0x02
	jmp exception_gate_main

exception_gate_03:
	mov al, 0x03
	jmp exception_gate_main

exception_gate_04:
	mov al, 0x04
	jmp exception_gate_main

exception_gate_05:
	mov al, 0x05
	jmp exception_gate_main

exception_gate_06:
	mov al, 0x06
	jmp exception_gate_main

exception_gate_07:
	mov al, 0x07
	jmp exception_gate_main

exception_gate_08:
	mov al, 0x08
	jmp exception_gate_main

exception_gate_09:
	mov al, 0x09
	jmp exception_gate_main

exception_gate_10:
	mov al, 0x0A
	jmp exception_gate_main

exception_gate_11:
	mov al, 0x0B
	jmp exception_gate_main

exception_gate_12:
	mov al, 0x0C
	jmp exception_gate_main

exception_gate_13:
	mov al, 0x0D
	jmp exception_gate_main

exception_gate_14:
	mov al, 0x0E
	jmp exception_gate_main

exception_gate_15:
	mov al, 0x0F
	jmp exception_gate_main

exception_gate_16:
	mov al, 0x10
	jmp exception_gate_main

exception_gate_17:
	mov al, 0x11
	jmp exception_gate_main

exception_gate_18:
	mov al, 0x12
	jmp exception_gate_main

exception_gate_19:
	mov al, 0x13
	jmp exception_gate_main

exception_gate_main:
	call os_print_newline
	mov rsi, int_string
	call os_print_string
	mov rsi, exc_string00
	and rax, 0xFF			; Clear out everything in RAX except for AL
	mov bl, 8
	mul bl				; AX = AL x BL
	add rsi, rax			; Use the value in RAX as an offset to get to the right message
	call os_print_string
	call os_print_newline
	call os_dump_regs

exception_gate_main_hang:
	nop
	jmp exception_gate_main_hang	; Hang. User must reset machine at this point

; Strings for the error messages
int_string db 'Pure64 - Interrupt ', 0
exc_string db '?? - Unknown Fatal Exception!', 0
align 16
exc_string00 db '00 - DE', 0
exc_string01 db '01 - DB', 0
exc_string02 db '02     ', 0
exc_string03 db '03 - BP', 0
exc_string04 db '04 - OF', 0
exc_string05 db '05 - BR', 0
exc_string06 db '06 - UD', 0
exc_string07 db '07 - NM', 0
exc_string08 db '08 - DF', 0
exc_string09 db '09     ', 0	; No longer generated on new CPU's
exc_string10 db '10 - TS', 0
exc_string11 db '11 - NP', 0
exc_string12 db '12 - SS', 0
exc_string13 db '13 - GP', 0
exc_string14 db '14 - PF', 0
exc_string15 db '15     ', 0
exc_string16 db '16 - MF', 0
exc_string17 db '17 - AC', 0
exc_string18 db '18 - MC', 0
exc_string19 db '19 - XM', 0


; =============================================================================
; EOF

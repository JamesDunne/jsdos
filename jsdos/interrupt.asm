
align 16
db 'DEBUG: INTERRUPT'
align 16

; -----------------------------------------------------------------------------
; Default exception handler
global hwi_exception_default;
hwi_exception_default:
    mov rsi, int_string00
    call os_print_string
    mov rsi, exc_string
    call os_print_string
    jmp $				; Hang
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Default interrupt handler
align 16
global hwi_interrupt_default;
hwi_interrupt_default:				; handler for all other interrupts
    iretq				; It was an undefined interrupt so return to caller
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Keyboard interrupt. IRQ 0x01, INT 0x21
; This IRQ runs whenever there is input on the keyboard
align 16
global hwi_keyboard;
hwi_keyboard:
    push rdi
    push rbx
    push rax

    mov rsi, int_string00
    call os_print_string

extern swi_keyboard
    call swi_keyboard

    mov rdi, [os_LocalAPICAddress]	; Acknowledge the IRQ on APIC
    add rdi, 0xB0
    xor eax, eax
    stosd
;	call os_smp_wakeup_all		; A terrible hack

    pop rax
    pop rbx
    pop rdi
    iretq
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Real-time clock interrupt. IRQ 0x08, INT 0x28
; Currently this IRQ runs 8 times per second (As defined in init_64.asm)
; The supervisor lives here
align 16
global hwi_rtc;
hwi_rtc:
    push rax
    push rcx
    push rsi

    cld				                ; Clear direction flag
    add qword [os_ClockCounter], 1	; 64-bit counter started at bootup

extern swi_rtc
    call swi_rtc

rtc_end:
    mov al, 0x0C			        ; Select RTC register C
    out 0x70, al			        ; Port 0x70 is the RTC index, and 0x71 is the RTC data
    in al, 0x71			            ; Read the value in register C
    mov rsi, [os_LocalAPICAddress]	; Acknowledge the IRQ on APIC
    xor eax, eax
    mov dword [rsi+0xB0], eax

    pop rsi
    pop rcx
    pop rax
    iretq
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Network interrupt.
align 16
global hwi_network;
hwi_network:
    push rdi
    push rsi
    push rcx
    push rax
    pushfq

extern swi_network
    call swi_network

network_end:
    mov rdi, [os_LocalAPICAddress]	; Acknowledge the IRQ on APIC
    add rdi, 0xB0
    xor eax, eax
    stosd

    popfq
    pop rax
    pop rcx
    pop rsi
    pop rdi
    iretq
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; A simple interrupt that just acknowledges an IPI. Useful for getting an AP past a 'hlt' in the code.
align 16
global hwi_ap_wakeup;
hwi_ap_wakeup:
    push rdi
    push rax

    cld				                ; Clear direction flag
    mov rdi, [os_LocalAPICAddress]	; Acknowledge the IPI
    add rdi, 0xB0
    xor rax, rax
    stosd

    pop rax
    pop rdi
    iretq				            ; Return from the IPI.
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; Resets a CPU to execute ap_clear
align 16
global hwi_ap_reset;
hwi_ap_reset:
    mov rsi, int_string00
    call os_print_string

    cld				                ; Clear direction flag
    mov rax, hwi_ap_clear		        ; Set RAX to the address of ap_clear
    mov [rsp], rax			        ; Overwrite the return address on the CPU's stack
    mov rdi, [os_LocalAPICAddress]	; Acknowledge the IPI
    add rdi, 0xB0
    xor rax, rax
    stosd
    iretq				            ; Return from the IPI. CPU will execute code at ap_clear
; -----------------------------------------------------------------------------



align 16
global hwi_ap_clear;
hwi_ap_clear:                   ; All cores start here on first startup and after an exception
    cli                         ; Disable interrupts on this core

    ; Get local ID of the core
    mov rsi, [os_LocalAPICAddress]
    xor eax, eax                ; Clear Task Priority (bits 7:4) and Task Priority Sub-Class (bits 3:0)
    mov dword [rsi+0x80], eax   ; APIC Task Priority Register (TPR)
    mov dword [rsi+0x90], eax   ; APIC Arbitration Priority Register (APR)
    mov eax, dword [rsi+0x20]   ; APIC ID
    shr rax, 24                 ; Shift to the right and AL now holds the CPU's APIC ID

    ; Calculate offset into CPU status table
    mov rdi, cpustatus
    add rdi, rax                ; RDI points to this cores status byte (we will clear it later)

    ; Set up the stack
    shl rax, 21                 ; Shift left 21 bits for a 2 MiB stack
    add rax, [os_StackBase]     ; The stack decrements when you "push", start at 2 MiB in
    mov rsp, rax

    ; Set the CPU status to "Present" and "Ready"
    mov al, 00000001b           ; Bit 0 set for "Present", Bit 1 clear for "Ready"
    stosb                       ; Set status to Ready for this CPU

    sti                         ; Re-enable interrupts on this core

    ; Clear registers. Gives us a clean slate to work with
    xor rax, rax                ; aka r0
    xor rcx, rcx                ; aka r1
    xor rdx, rdx                ; aka r2
    xor rbx, rbx                ; aka r3
    xor rbp, rbp                ; aka r5, We skip RSP (aka r4) as it was previously set
    xor rsi, rsi                ; aka r6
    xor rdi, rdi                ; aka r7
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

;extern swi_ap_clear
;    call swi_ap_clear

    jmp hwi_ap_clear


; -----------------------------------------------------------------------------
; CPU Exception Gates
align 16
global hwi_exception_gate_00;
hwi_exception_gate_00:
    push rax
    mov al, 0x00
    jmp exception_gate_main

align 16
exception_gate_01:
    push rax
    mov al, 0x01
    jmp exception_gate_main

align 16
exception_gate_02:
    push rax
    mov al, 0x02
    jmp exception_gate_main

align 16
exception_gate_03:
    push rax
    mov al, 0x03
    jmp exception_gate_main

align 16
exception_gate_04:
    push rax
    mov al, 0x04
    jmp exception_gate_main

align 16
exception_gate_05:
    push rax
    mov al, 0x05
    jmp exception_gate_main

align 16
exception_gate_06:
    push rax
    mov al, 0x06
    jmp exception_gate_main

align 16
exception_gate_07:
    push rax
    mov al, 0x07
    jmp exception_gate_main

align 16
exception_gate_08:
    push rax
    mov al, 0x08
    jmp exception_gate_main

align 16
exception_gate_09:
    push rax
    mov al, 0x09
    jmp exception_gate_main

align 16
exception_gate_10:
    push rax
    mov al, 0x0A
    jmp exception_gate_main

align 16
exception_gate_11:
    push rax
    mov al, 0x0B
    jmp exception_gate_main

align 16
exception_gate_12:
    push rax
    mov al, 0x0C
    jmp exception_gate_main

align 16
exception_gate_13:
    push rax
    mov al, 0x0D
    jmp exception_gate_main

align 16
exception_gate_14:
    push rax
    mov al, 0x0E
    jmp exception_gate_main

align 16
exception_gate_15:
    push rax
    mov al, 0x0F
    jmp exception_gate_main

align 16
exception_gate_16:
    push rax
    mov al, 0x10
    jmp exception_gate_main

align 16
exception_gate_17:
    push rax
    mov al, 0x11
    jmp exception_gate_main

align 16
exception_gate_18:
    push rax
    mov al, 0x12
    jmp exception_gate_main

align 16
exception_gate_19:
    push rax
    mov al, 0x13
    jmp exception_gate_main

align 16
exception_gate_main:
    push rbx
    push rdi
    push rsi
    push rax			; Save RAX since os_smp_get_id clobers it
    call os_print_newline
    mov bl, 0x0F
    mov rsi, int_string00
    call os_print_string_with_color
    call os_smp_get_id		; Get the local CPU ID and print it
    mov rdi, os_temp_string
    mov rsi, rdi
    call os_int_to_string
    call os_print_string_with_color
    mov rsi, int_string01
    call os_print_string
    mov rsi, exc_string00
    pop rax
    and rax, 0x00000000000000FF	; Clear out everything in RAX except for AL
    push rax
    mov bl, 52
    mul bl				; AX = AL x BL
    add rsi, rax			; Use the value in RAX as an offset to get to the right message
    pop rax
    mov bl, 0x0F
    call os_print_string_with_color
    call os_print_newline
    pop rsi
    pop rdi
    pop rbx
    pop rax
    call os_print_newline
    call os_debug_dump_reg
    mov rsi, rip_string
    call os_print_string
    push rax
    mov rax, [rsp+0x08] 	; RIP of caller
    call os_debug_dump_rax
    pop rax
    call os_print_newline
    push rax
    push rcx
    push rsi
    mov rsi, stack_string
    call os_print_string
    mov rsi, rsp
    add rsi, 0x18
    mov rcx, 4
next_stack:
    lodsq
    call os_debug_dump_rax
    mov al, ' '
    call os_print_char
;	call os_print_char
;	call os_print_char
;	call os_print_char
    loop next_stack
    call os_print_newline
    pop rsi
    pop rcx
    pop rax
;	jmp $				; For debugging

;    call init_memory_map
    jmp hwi_ap_clear			; jump to AP clear code


int_string00 db 'JSDOS - CPU ', 0
int_string01 db ' - ', 0
; Strings for the error messages
exc_string db 'Unknown Fatal Exception!', 0
exc_string00 db 'Interrupt 00 - Divide Error Exception (#DE)        ', 0
exc_string01 db 'Interrupt 01 - Debug Exception (#DB)               ', 0
exc_string02 db 'Interrupt 02 - NMI Interrupt                       ', 0
exc_string03 db 'Interrupt 03 - Breakpoint Exception (#BP)          ', 0
exc_string04 db 'Interrupt 04 - Overflow Exception (#OF)            ', 0
exc_string05 db 'Interrupt 05 - BOUND Range Exceeded Exception (#BR)', 0
exc_string06 db 'Interrupt 06 - Invalid Opcode Exception (#UD)      ', 0
exc_string07 db 'Interrupt 07 - Device Not Available Exception (#NM)', 0
exc_string08 db 'Interrupt 08 - Double Fault Exception (#DF)        ', 0
exc_string09 db 'Interrupt 09 - Coprocessor Segment Overrun         ', 0	; No longer generated on new CPU's
exc_string10 db 'Interrupt 10 - Invalid TSS Exception (#TS)         ', 0
exc_string11 db 'Interrupt 11 - Segment Not Present (#NP)           ', 0
exc_string12 db 'Interrupt 12 - Stack Fault Exception (#SS)         ', 0
exc_string13 db 'Interrupt 13 - General Protection Exception (#GP)  ', 0
exc_string14 db 'Interrupt 14 - Page-Fault Exception (#PF)          ', 0
exc_string15 db 'Interrupt 15 - Undefined                           ', 0
exc_string16 db 'Interrupt 16 - x87 FPU Floating-Point Error (#MF)  ', 0
exc_string17 db 'Interrupt 17 - Alignment Check Exception (#AC)     ', 0
exc_string18 db 'Interrupt 18 - Machine-Check Exception (#MC)       ', 0
exc_string19 db 'Interrupt 19 - SIMD Floating-Point Exception (#XM) ', 0
rip_string db ' IP:', 0
stack_string db ' ST:', 0

; =============================================================================
; EOF

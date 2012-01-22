; =============================================================================
; BareMetal -- a 64-bit OS written in Assembly for x86-64 systems
; Copyright (C) 2008-2011 Return Infinity -- see LICENSE.TXT
;
; SMP Functions
; =============================================================================

align 16
db 'DEBUG: SMP      '
align 16


; -----------------------------------------------------------------------------
; os_smp_reset -- Resets a CPU Core
;  IN:	AL = CPU #
; OUT:	Nothing. All registers preserved.
; Note:	This code resets an AP
;	For setup use only.
os_smp_reset:
	push rdi
	push rax

	mov rdi, [os_LocalAPICAddress]
	shl eax, 24		; AL holds the CPU #, shift left 24 bits to get it into 31:24, 23:0 are reserved
	mov [rdi+0x0310], eax	; Write to the high bits first
	xor eax, eax		; Clear EAX, namely bits 31:24
	mov al, 0x81		; Execute interrupt 0x81
	mov [rdi+0x0300], eax	; Then write to the low bits

	pop rax
	pop rdi
	ret
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_wakeup -- Wake up a CPU Core
;  IN:	AL = CPU #
; OUT:	Nothing. All registers preserved.
os_smp_wakeup:
	push rdi
	push rax

	mov rdi, [os_LocalAPICAddress]
	shl eax, 24		; AL holds the CPU #, shift left 24 bits to get it into 31:24, 23:0 are reserved
	mov [rdi+0x0310], eax	; Write to the high bits first
	xor eax, eax		; Clear EAX, namely bits 31:24
	mov al, 0x80		; Execute interrupt 0x80
	mov [rdi+0x0300], eax	; Then write to the low bits

	pop rax
	pop rdi
	ret
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_wakeup_all -- Wake up all CPU Cores
;  IN:	Nothing.
; OUT:	Nothing. All registers preserved.
os_smp_wakeup_all:
	push rdi
	push rax

	mov rdi, [os_LocalAPICAddress]
	xor eax, eax
	mov [rdi+0x0310], eax	; Write to the high bits first
	mov eax, 0x000C0080	; Execute interrupt 0x80
	mov [rdi+0x0300], eax	; Then write to the low bits

	pop rax
	pop rdi
	ret
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_get_id -- Returns the APIC ID of the CPU that ran this function
;  IN:	Nothing
; OUT:	RAX = CPU's APIC ID number, All other registers perserved.
os_smp_get_id:
	push rsi

	xor eax, eax
	mov rsi, [os_LocalAPICAddress]
	add rsi, 0x20		; Add the offset for the APIC ID location
	lodsd			; APIC ID is stored in bits 31:24
	shr rax, 24		; AL now holds the CPU's APIC ID (0 - 255)

	pop rsi
	ret
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_numcores -- Returns the number of cores in this computer
;  IN:	Nothing
; OUT:	RAX = number of cores in this computer
os_smp_numcores:
	xor eax, eax
	mov ax, [os_NumCores]
	ret
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_wait -- Wait until all other CPU Cores are finished processing
;  IN:	Nothing
; OUT:	Nothing. All registers preserved.
os_smp_wait:
	push rsi
	push rcx
	push rbx
	push rax

	call os_smp_get_id
	mov rbx, rax

	xor eax, eax
	xor ecx, ecx
	mov rsi, cpustatus

checkit:
	lodsb
	cmp rbx, rcx		; Check to see if it is looking at itself
	je skipit		; If so then skip as it shouild be marked as busy
	bt ax, 0		; Check the Present bit
	jnc skipit		; If carry is not set then the CPU does not exist
	bt ax, 1		; Check the Ready/Busy bit
	jnc skipit		; If carry is not set then the CPU is Ready
	sub rsi, 1
	jmp checkit		; Core is marked as Busy, check it again
skipit:
	add rcx, 1
	cmp rcx, 256
	jne checkit

	pop rax
	pop rbx
	pop rcx
	pop rsi
	ret
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_lock -- Attempt to lock a mutex
;  IN:	RAX = Address of lock variable
; OUT:	Nothing. All registers preserved.
os_smp_lock:
	bt word [rax], 0	; Check if the mutex is free (Bit 0 cleared to 0)
	jc os_smp_lock		; If not check it again
	lock			; The mutex was free, lock the bus
	bts word [rax], 0	; Try to grab the mutex (Bit 0 set to 1)
	jc os_smp_lock		; Jump if we were unsuccessful
	ret			; Lock acquired. Return to the caller
; -----------------------------------------------------------------------------


; -----------------------------------------------------------------------------
; os_smp_unlock -- Unlock a mutex
;  IN:	RAX = Address of lock variable
; OUT:	Nothing. All registers preserved.
os_smp_unlock:
	btr word [rax], 0	; Release the lock (Bit 0 cleared to 0)
	ret			; Lock released. Return to the caller
; -----------------------------------------------------------------------------


; =============================================================================
; EOF

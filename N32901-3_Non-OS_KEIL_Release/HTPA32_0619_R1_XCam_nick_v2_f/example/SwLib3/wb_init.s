

	AREA WB_INIT, CODE, READONLY

;--------------------------------------------
; Mode bits and interrupt flag (I&F) defines
;--------------------------------------------
USR_MODE	EQU		0x10
FIQ_MODE	EQU		0x11
IRQ_MODE	EQU		0x12
SVC_MODE	EQU		0x13
ABT_MODE	EQU		0x17
UDF_MODE	EQU		0x1B
SYS_MODE	EQU		0x1F

I_BIT		EQU		0x80
F_BIT		EQU		0x40

;----------------------------
; System / User Stack Memory
;----------------------------
 IF :DEF:N32901
RAM_Limit       EQU     0x200000          	; For unexpanded hardware board
 ENDIF
 
 IF :DEF:N32905
RAM_Limit       EQU     0x2000000          	; For unexpanded hardware board
 ENDIF 
 
 IF :DEF:N32903
RAM_Limit       EQU     0x800000          	; For unexpanded hardware board
 ENDIF 

UND_Stack		EQU		RAM_Limit
Abort_Stack		EQU		RAM_Limit-256
IRQ_Stack       EQU     RAM_Limit-512       ; followed by IRQ stack
FIQ_Stack       EQU     RAM_Limit-768       ; followed by IRQ stack
SVC_Stack       EQU     RAM_Limit-1024      ; SVC stack at top of memory
USR_Stack		EQU		RAM_Limit


	ENTRY
	EXPORT  Reset_Go


        EXPORT	Vector_Table
Vector_Table		
        B       Reset_Go    ; Modified to be relative jumb for external boot
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        DCD		0x0
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr

        
Reset_Addr      DCD     Reset_Go
Undefined_Addr  DCD     Undefined_Handler
SWI_Addr        DCD     SWI_Handler1
Prefetch_Addr   DCD     Prefetch_Handler
Abort_Addr      DCD     Abort_Handler
				DCD		0
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler


; ************************
; Exception Handlers
; ************************

; The following dummy handlers do not do anything useful in this example.
; They are set up here for completeness.

Undefined_Handler
        B       Undefined_Handler
SWI_Handler1
		mov 	r0, #0
		movs	pc, lr	   
Prefetch_Handler
        B       Prefetch_Handler
Abort_Handler
        B       Abort_Handler
IRQ_Handler
		B		IRQ_Handler
FIQ_Handler
        B       FIQ_Handler

	
Reset_Go

;--------------------------------
; Initial Stack Pointer register
;--------------------------------
;INIT_STACK 
 MSR	CPSR_c, #UDF_MODE | I_BIT | F_BIT
 LDR     SP, =UND_Stack

 MSR	CPSR_c, #ABT_MODE | I_BIT | F_BIT
 LDR     SP, =Abort_Stack

 MSR	CPSR_c, #IRQ_MODE | I_BIT | F_BIT
 LDR     SP, =IRQ_Stack

 MSR	CPSR_c, #FIQ_MODE | I_BIT | F_BIT
 LDR     SP, =FIQ_Stack

 MSR	CPSR_c, #SYS_MODE | I_BIT | F_BIT
 LDR     SP, =USR_Stack

 MSR	CPSR_c, #SVC_MODE | I_BIT | F_BIT
 LDR     SP, =SVC_Stack

;------------------------------------------------------
; Set the normal exception vector of CP15 control bit    
;------------------------------------------------------    
	MRC	p15, 0, r0 , c1, c0   	; r0 := cp15 register 1
	BIC r0, r0, #0x2000		; Clear bit13 in r1
	MCR p15, 0, r0 , c1, c0     ; cp15 register 1 := r0

;-----------------------------
; system initialization 
;------------------------------

 IF :DEF:SYS_INIT

 ldr	r0, =0xB0003000
 ldr	r1, =0x00130456
 str	r1, [r0]

 ldr	r0, =0xB0003030
 ldr	r1, =0x00001010
 str	r1, [r0]

 ldr	r0, =0xB0003010
 ldr	r1, =0x00000005
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x00000021
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x00000023
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x00000027
 str	r1, [r0]

 ldr	r0, =0xB000301C
 ldr	r1, =0x00001002
 str	r1, [r0]

 ldr	r0, =0xB0003018
 ldr	r1, =0x00000122
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x00000027
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x0000002B
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x0000002B
 str	r1, [r0]

 ldr	r0, =0xB0003018
 ldr	r1, =0x00000022
 str	r1, [r0]

 ldr	r0, =0xB0003004
 ldr	r1, =0x00000020
 str	r1, [r0]

 ldr	r0, =0xB0003034
 ldr	r1, =0x00AAAA00
 str	r1, [r0]

 ldr	r0, =0xB0003008
 ldr	r1, =0x0000805A
 str	r1, [r0]

 ldr	r0, =0xB00000A0
 ldr	r1, =0x00000000
 str	r1, [r0]

 ldr	r0, =0xB0000224
 ldr	r1, =0x0000447E
 str	r1, [r0]

 ldr	r0, =0xB000020C
 ldr	r1, =0x00000018
 str	r1, [r0]

 ldr	r0, =0xB000021C
 ldr	r1, =0x00000000
 str	r1, [r0]

 ldr	r0, =0xB0003028
 ldr	r1, =0x094E7425
 str	r1, [r0]

 ENDIF

	IMPORT	__main
;-----------------------------
;	enter the C code
;-----------------------------	
	B   __main

	END

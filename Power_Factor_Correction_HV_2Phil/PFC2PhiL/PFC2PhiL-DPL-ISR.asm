;----------------------------------------------------------------------------------
;	FILE:			PwrLibTestbench-DPL-ISR.asm
;
;	Description:	{ProjectName}-DPL-ISR.asm contains the ISR for the system
;					It also contains the initailization routine for all the macros 
;					being used in the system both for CLA and C28x macros
;   
;   Revision/ Version: See {ProjectName}-Main.c
;----------------------------------------------------------------------------------

		;Gives peripheral addresses visibility in assembly
	    .cdecls   C,LIST,"PeripheralHeaderIncludes.h"

		;include C header file - sets INCR_BUILD etc.(used in conditional builds)
		.cdecls C,NOLIST, "PFC2PhiL-Settings.h"

		;Include files for the Power Library Maco's being used by the system 
;		.include "ADCDRV_1ch.asm"
		.include "ADCDRV_4ch.asm"
		.include "CNTL_2P2Z.asm"
		.include "PWMDRV_PFC2PhiL.asm"
		.include "MATH_EMAVG.asm"
		.include "PFC_INVSQR.asm"
		.include "PFC_ICMD.asm"
		.include "DLOG_4ch.asm"
		
;=============================================================================
; Digital Power library - Initailization Routine 
;=============================================================================

		; label to DP initialisation function
		.def _DPL_Init	
		; label to DP ISR Run function
		.def _DPL_ISR
		
		; dummy variable for pointer initialisation
ZeroNet	 .usect "ZeroNet_Section",2,1,1	; output terminal 1
VloopCtr .usect "ISRVariables",2,1,1

		.text
_DPL_Init:
		ZAPA
		MOVL	XAR0, #ZeroNet
		MOVL	*XAR0, ACC

		; Initialize all the DP library macro used here 
		;---------------------------------------------------------
		.if(INCR_BUILD = 1)
			PWMDRV_PFC2PhiL_INIT 1			
			ADCDRV_4CH_INIT 0,1,2,3
			DLOG_4ch_INIT 1			; Multiple instances of 4 ch DLOG are not supported			
		.endif
		;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			PWMDRV_PFC2PhiL_INIT 1			
			ADCDRV_4CH_INIT 0,1,2,3
			CNTL_2P2Z_INIT 2
			CNTL_2P2Z_INIT 1
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			
			PFC_INVSQR_INIT 1				
			PFC_ICMD_INIT 1
			DLOG_4ch_INIT 1			; Multiple instances of 4 ch DLOG are not supported										
		.endif
		;---------------------------------------------------------			
		LRETR

;-----------------------------------------------------------------------------------------

; Digital Power library - Interrupt Service Routine

		.sect "ramfuncs"
_DPL_ISR:
		; full context save - push any unprotected registers onto stack
		PUSH  	AR1H:AR0H
		PUSH  	XAR2
		PUSH  	XAR3
		PUSH  	XAR4
		PUSH  	XAR5
		PUSH  	XAR6
		PUSH  	XAR7
		PUSH  	XT
		SPM   	0          				; set C28 mode
		CLRC  	AMODE       
		CLRC  	PAGE0,OVM 
;		CLRC	INTM					; clear interrupt mask - comment if ISR non-nestable
;-----------------------------------------------------------------------------------------

; call DP library modules
		ZAPA
		;---------------------------------------------------------
		.if(INCR_BUILD = 1)

		ADCDRV_4CH 1,2,4,5 
		.ref _IphA
		.ref _IphB
		.ref _IpfcTotal
		 MOVW 		DP,#(_IphA) 	
		 MOVL 		ACC,@_IphA
		 MOVW 		DP,#(_IphB)
		 ADDL 		ACC,@_IphB
		 SUB	    ACC, #3970<<9		; subtract 0.4 (Q24) - 3970(Q15: refer to the excel sheet) corresponds to 0.4
         ASR64		ACC:P,#1		
		 MOVW 		DP,#(_IpfcTotal)
		 MOVL		@_IpfcTotal, ACC		
		 PWMDRV_PFC2PhiL	1			; Run the PWM driver init on EPWM1	
		.endif
		;---------------------------------------------------------
		.if(INCR_BUILD = 2)

		ADCDRV_4CH 1,2,4,5

		.ref	_LineGain
		.ref	_VacLineRect
		.ref	_VacLineScaled
		MOVW		DP,#(_LineGain)
		MOVL		XT,@_LineGain		 ;Q30
		MOVW		DP,#(_VacLineRect)
		QMPYL		ACC,XT,@_VacLineRect ; Q30 * Q24 = Q22
		LSL			ACC,#2

		.ref	_shoulder
		MOVW	DP, #(_shoulder)
		SUBL    ACC, @(_shoulder)	 
		B		NO_ZERO, GEQ
		MOVB	ACC,#0
NO_ZERO:
		.ref	_VacLineScaled
		MOVW	DP, #(_VacLineScaled)	
		MOVL	@(_VacLineScaled), ACC  

		.ref	_MaxQ24ref
		MOVW	DP, #(_MaxQ24ref)		
		MOVL	ACC, @(_MaxQ24ref)  	; ACC = 0x00FFFFFF ~ 1 in Q24

		MOVW	DP, #(_VacLineScaled)	
		SUBL	ACC, @(_VacLineScaled)

		.ref	_InvVac
		MOVW	DP, #(_InvVac)			 
		MOVL 	XAR4,#(_InvVac)			; Net pointer to InvVac (XAR4)
		MOVL 	*XAR4,ACC				; Write to InvVac

		.ref	_InvSineComp
		MOVW	DP, #(_InvSineComp)
		MOVL 	XAR2,#(_InvSineComp)	; Net pointer to InvSineComp (XAR2)

		.ref	_CompAmpltd
		MOVW	DP, #(_CompAmpltd)		  
		MOVL 	XT,*XAR4				; XT = InvVac
		QMPYL 	ACC,XT,@_CompAmpltd		; ACC (Q22) = InvVac (Q24) * CompAmpltd (Q30)
		LSL		ACC,#2					; Q24
		MOVL 	*XAR2,ACC				; Write to InvSineComp 
		
		.ref _IphA
		.ref _IphB
		 MOVW 		DP,#(_IphA) 	
		 MOVL 		ACC,@_IphA
		 MOVW 		DP,#(_IphB)
		 ADDL 		ACC,@_IphB
		 SUB	    ACC, #3970<<9		; subtract 0.4 (Q24) - 3970(Q15: refer to the excel sheet) corresponds to 0.4
         ASR64		ACC:P,#1		

		.ref	_InvSineComp
		MOVW	DP, #(_InvSineComp)
		SUBL    ACC, @(_InvSineComp)	 
		B		LOW_LD, GEQ
		MOVB	ACC,#0		

LOW_LD:
		.ref	_IpfcTotal
		 MOVW 	DP,#(_IpfcTotal)
		 MOVL	@_IpfcTotal, ACC

		PFC_ICMD 		1
		CNTL_2P2Z		2				; call the 2P2Z controller
		PWMDRV_PFC2PhiL 1				; Run the PWM driver init on EPWM1
		MATH_EMAVG		1						
		PFC_INVSQR  	1
		MATH_EMAVG		2
		
		;Execute Vloop every VoltCurrLoopExecRatio times, defined in PFC2PhiL-Settings.h file 	
		MOVW	DP,#(VloopCtr)
		INC		@VloopCtr
		CMP		@VloopCtr,#VoltCurrLoopExecRatio
		B		VL_SKIP,LT
		MOV		@VloopCtr,#0
		CNTL_2P2Z		1
VL_SKIP:
		.endif

		DLOG_4ch  1

		ZAPA

;-----------------------------------------------------------------------------------------
; Interrupt management before exit

	.if(EPWMn_DPL_ISR=1)

	.if(EPWM1)
		MOVW 	DP,#_EPwm1Regs.ETCLR
		MOV 	@_EPwm1Regs.ETCLR,#0x01			; Clear EPWM1 Int flag
	.endif ; EPWM1

	.if(EPWM2)
		MOVW 	DP,#_EPwm2Regs.ETCLR
		MOV 	@_EPwm2Regs.ETCLR,#0x01			; Clear EPWM2 Int flag
	.endif ; EPWM2

	.if(EPWM3)
		MOVW 	DP,#_EPwm3Regs.ETCLR
		MOV 	@_EPwm3Regs.ETCLR,#0x01			; Clear EPWM3 Int flag
	.endif ; EPWM3

	.if(EPWM4)
		MOVW 	DP,#_EPwm4Regs.ETCLR
		MOV 	@_EPwm4Regs.ETCLR,#0x01			; Clear EPWM4 Int flag
	.endif ; EPWM4

	.if(EPWM5)
		MOVW 	DP,#_EPwm5Regs.ETCLR
		MOV 	@_EPwm5Regs.ETCLR,#0x01			; Clear EPWM5 Int flag
	.endif ; EPWM5

	.if(EPWM6)
		MOVW 	DP,#_EPwm6Regs.ETCLR
		MOV 	@_EPwm6Regs.ETCLR,#0x01			; Clear EPWM6 Int flag
	.endif ; EPWM6

		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 3
		MOV 	@_PieCtrlRegs.PIEACK, #0x4
	.endif ; EPWMn_ISR


	.if(ADC_DPL_ISR=1)
	; Case where ISR is triggered by ADC 
		MOVW 	DP,#ADCST>>6
		MOV 	@ADCST,#0x010					; Clear INT SEQ1 Int flag

		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 1
		MOV 	@_PieCtrlRegs.PIEACK,0x1
	.endif ; ADC_ISR 

	
;-----------------------------------------------------------------------------------------
; full context restore
;		SETC	INTM							; set INTM to protect context restore
		POP   	XT
		POP   	XAR7
		POP   	XAR6
		POP   	XAR5
		POP   	XAR4
		POP   	XAR3
		POP   	XAR2
		POP   	AR1H:AR0H
		IRET									; return from interrupt
		.end

; end of file


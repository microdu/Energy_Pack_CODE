;----------------------------------------------------------------------------------
;	FILE:			BridgelessPFC-DPL-ISR.asm
;
;	Description:	BridgelessPFC-DPL-ISR.asm contains the ISR for the system
;					It also contains the initailization routine for all the macros 
;					being used in the system both for CLA and C28x macros
;   
;   Revision/ Version: See BridgelessPFC-Main.c
;----------------------------------------------------------------------------------

		;Gives peripheral addresses visibility in assembly
	    .cdecls   C,LIST,"PeripheralHeaderIncludes.h"

		;include C header file - sets INCR_BUILD etc.(used in conditional builds)
		.cdecls C,NOLIST, "BridgelessPFC-Settings.h"

		;Include files for the Power Library Maco's being used by the system
		.include "ADCDRV_1ch.asm"
		.include "PFC_BL_ICMD.asm"
		.include "PWMDRV_1ch_UpDwnCnt.asm" 
		
		.include "CNTL_2P2Z.asm"
		.include "MATH_EMAVG.asm"
		.include "PFC_INVSQR.asm"
		.include "PFC_InvRmsSqr.asm"
			
;=============================================================================
; Digital Power library - Initailization Routine 
;=============================================================================

		; label to DP initialisation function
		.def _DPL_Init	

		; dummy variable for pointer initialisation
ZeroNet	 .usect "ZeroNet_Section",2,1,1	; output terminal 1
;_Vrect .usect "ISR_data",2,1,1
VloopCtr .usect "ISRVariables",2,1,1
	
;		.def _Vrect

		.text
_DPL_Init:
		ZAPA
		MOVL	XAR0, #ZeroNet
		MOVL	*XAR0, ACC

; Initialize all the DP library macro used here 
;---------------------------------------------------------
		.if(INCR_BUILD = 1)
			ADCDRV_1ch_INIT 1	; Ipfc
			ADCDRV_1ch_INIT 2	; Vpfc
			ADCDRV_1ch_INIT 3	; VL_fb
			ADCDRV_1ch_INIT 4	; VN_fb

			MATH_EMAVG_INIT 2			
		
			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			PWMDRV_1ch_UpDwnCnt_INIT 4	; PWM4A
			
		.endif
;---------------------------------------------------------------------	
;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			ADCDRV_1ch_INIT 1	; Ipfc
			ADCDRV_1ch_INIT 2	; Vpfc
			ADCDRV_1ch_INIT 3	; VL_fb
			ADCDRV_1ch_INIT 4	; VN_fb

			CNTL_2P2Z_INIT 1
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			
			
			PFC_InvRmsSqr_INIT 1

			PFC_BL_ICMD_INIT 1	;Bridgeless PFC current command init
			
			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			PWMDRV_1ch_UpDwnCnt_INIT 4	; PWM4A
			
		.endif
;---------------------------------------------------------------------		
		.if(INCR_BUILD = 3)
			ADCDRV_1ch_INIT 1	; Ipfc
			ADCDRV_1ch_INIT 2	; Vpfc
			ADCDRV_1ch_INIT 3	; VL_fb
			ADCDRV_1ch_INIT 4	; VN_fb

			CNTL_2P2Z_INIT 2
			CNTL_2P2Z_INIT 1
			
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			

			PFC_InvRmsSqr_INIT 1


			PFC_BL_ICMD_INIT 1	;Bridgeless PFC current command init
			
			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			PWMDRV_1ch_UpDwnCnt_INIT 4	; PWM4A
			
		.endif
;---------------------------------------------------------			
		LRETR

;-----------------------------------------------------------------------------------------

; Digital Power library - Interrupt Service Routine

		.sect "ramfuncs"
		; label to DP ISR Run function
		.def	_DPL_ISR

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
;---------------------------------------------------------
		.if(INCR_BUILD = 1)
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Vpfc
			ADCDRV_1ch 3	; VL_fb
			ADCDRV_1ch 4	; VN_fb
			
			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			
			MATH_EMAVG		2
			
			PWMDRV_1ch_UpDwnCnt 4		; PWM4A
		.endif
;----------------------------------------------
;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Vpfc
			ADCDRV_1ch 3	; VL_fb
			ADCDRV_1ch 4	; VN_fb

			PFC_BL_ICMD 1	;Bridgeless PFC current command
			
			CNTL_2P2Z		1
			
			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			
			MATH_EMAVG		1						
		    PFC_InvRmsSqr  	1
			MATH_EMAVG		2
			
			PWMDRV_1ch_UpDwnCnt 4		; PWM4A
		.endif
;----------------------------------------------
		.if(INCR_BUILD = 3)
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Vpfc
			ADCDRV_1ch 3	; VL_fb
			ADCDRV_1ch 4	; VN_fb
	
			PFC_BL_ICMD 1	;Bridgeless PFC current command
			
			CNTL_2P2Z		1
			
			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
		    
		    PFC_InvRmsSqr  	1
			MATH_EMAVG		2
			
			;PWMDRV_1ch_UpDwnCnt 4		; PWM4A
			
;Execute Vloop every VoltCurrLoopExecRatio times, defined in BridgelessPFC-Settings.h file 	
		MOVW	DP,#(VloopCtr)
		INC		@VloopCtr
		CMP		@VloopCtr,#VoltCurrLoopExecRatio
		B		SKIP_VLOOP_CALC,LT
		
		MOV		@VloopCtr,#0
		CNTL_2P2Z		2			;Volt loop controller
		
SKIP_VLOOP_CALC:
			
		.endif
;----------------------------------------------------			
					
;Calculate Vrect
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	MOV		ACC, @_AdcResult.ADCRESULT3<<12		; ACC = Line
        	SUB		ACC, @_AdcResult.ADCRESULT4<<12		; ACC = Line - Neutral
        	B		NegativeCycle, LEQ					; Branch to Negative Half Cycle
PositiveCycle:
	; Save Vrect
			.ref	_Vrect
        	MOVW	DP, #_Vrect
			MOVL	@_Vrect, ACC
	; ePWM1 & ADC configuration
	        MOVW 	DP, #_AdcRegs.ADCSOC1CTL            ; load Data Page to read ADC results
;			MOV		@_AdcRegs.ADCSOC1CTL.bit.CHSEL, #2	; Switch ADC to IpfcA current
			EALLOW
			MOV		@_AdcRegs.ADCSOC1CTL, #10374		; Switch ADC to IpfcA current
			EDIS
	        MOVW 	DP, #_EPwm1Regs.AQCTLA                     ; load Data Page to read ePWM registers
;			MOV		@_EPwm1Regs.AQCTLA.bit.CAU, #1		; CLEAR ePWM1 on CompA-Up (enable switching)
;			MOV		@_EPwm1Regs.AQCTLA.bit.CAD, #2		; SET ePWM1 on CompA-Down (enable switching)
			MOV		@_EPwm1Regs.AQCTLA, #144			; SET ePWM1 on CompA-Down, CLEAR CompA-Up (enable switching)
	        MOVW 	DP, #_EPwm2Regs.AQCTLA                     ; load Data Page to read ePWM registers
;			MOV		@_EPwm2Regs.AQCTLA.bit.CAU, #1		; CLEAR ePWM2 on CompA-Up
;			MOV		@_EPwm2Regs.AQCTLA.bit.CAD, #1		; CLEAR ePWM2 on CompA-Down (force low)
			MOV		@_EPwm2Regs.AQCTLA, #80				; CLEAR ePWM2 on CompA-Up/Down (force low)
	; Check if near Zero crossing before forcing ePWM2 High
			;SUB		ACC, #100<<12
			SUB		ACC, #50<<12
			B		SkipPWM2Force, LT
;			MOV		@_EPwm2Regs.AQCTLA.bit.CAU, #2		; SET ePWM2 on CompA-Up (force high)
;			MOV		@_EPwm2Regs.AQCTLA.bit.CAD, #2		; SET ePWM2 on CompA-Down
			MOV		@_EPwm2Regs.AQCTLA, #160			; SET ePWM2 on CompA-Up/Down (force high)
SkipPWM2Force:
;	        MOVW 	DP, #_GpioDataRegs.GPADAT                 	; load Data Page to read GPIO registers
;			MOV 	@_GpioDataRegs.GPASET, #128 ; Set GPIO7, Used for debug purposes
        	B		ControlLoopEnd, UNC				
NegativeCycle:
	; Save Vrect
        	MOV		ACC, @_AdcResult.ADCRESULT4<<12		; ACC = Neutral
        	SUB		ACC, @_AdcResult.ADCRESULT3<<12		; ACC = Neutral - Line
        	MOVW	DP, #_Vrect
			MOVL	@_Vrect, ACC

	; ePWM2 & ADC configuration
	        MOVW 	DP, #_AdcRegs.ADCSOC1CTL            ; load Data Page to read ADC results
;			MOV		@_AdcRegs.ADCSOC1CTL.bit.CHSEL, #4	; Switch ADC to IpfcB current
			EALLOW
			MOV		@_AdcRegs.ADCSOC1CTL, #10502		; Switch ADC to IpfcB current
	        EDIS
	        MOVW 	DP, #_EPwm2Regs.AQCTLA                   ; load Data Page to read ePWM registers
;			MOV		@_EPwm2Regs.AQCTLA.bit.CAU, #1		; CLEAR ePWM2 on CompA-Up (enable switching)
;			MOV		@_EPwm2Regs.AQCTLA.bit.CAD, #2		; SET ePWM2 on CompA-Down (enable switching)
			MOV		@_EPwm2Regs.AQCTLA, #144			; SET ePWM2 on CompA-Down, CLEAR CompA-Up (enable switching)
	        MOVW 	DP, #_EPwm1Regs.AQCTLA                     ; load Data Page to read ePWM registers
;			MOV		@_EPwm1Regs.AQCTLA.bit.CAU, #1		; CLEAR ePWM1 on CompA-Up
;			MOV		@_EPwm1Regs.AQCTLA.bit.CAD, #1		; CLEAR ePWM1 on CompA-Down (force low)
			MOV		@_EPwm1Regs.AQCTLA, #80				; CLEAR ePWM1 on CompA-Up/Down (force low)
	; Check if near Zero crossing before forcing ePWM1 High
			;SUB		ACC, #100<<12
			SUB		ACC, #50<<12
			B		SkipPWM1Force, LT
;			MOV		@_EPwm1Regs.AQCTLA.bit.CAU, #2		; SET ePWM1 on CompA-Up (force high)
;			MOV		@_EPwm1Regs.AQCTLA.bit.CAD, #2		; SET ePWM1 on CompA-Down
			MOV		@_EPwm1Regs.AQCTLA, #160			; SET ePWM1 on CompA-Up/Down (force high)
SkipPWM1Force:
;	        MOVW 	DP, #_GpioDataRegs.GPADAT                 	; load Data Page to read GPIO registers
;			MOV	@_GpioDataRegs.GPACLEAR, #128 		; Clear GPIO7, Used for debug purposes

ControlLoopEnd:
			;.endif
		;----------------------------------------------------------
;			.ref 	_Duty4A		
;			MOVW	DP, #_Vrect
;			MOVL	ACC, @_Vrect
;			MOVW	DP, #_Duty4A
;			MOVL    @_Duty4A, ACC	;Write 9 bit value to Duty4A
			
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
	
		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 3
		MOV 	@_PieCtrlRegs.PIEACK, #0x4
	.endif ; EPWMn_ISR

	.if(ADC_DPL_ISR=1)
	; Case where ISR is triggered by ADC 
;		MOVW 	DP,#_AdcRegs.ADCINTFLGCLR
;		MOV 	@AdcRegs.ADCINTFLGCLR, #0x01	; Clear ADCINT1 Flag

		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 1
		MOV 	@_PieCtrlRegs.PIEACK, #0x1
	.endif 

;-----------------------------------------------------------------------------------------
; full context restore
;		SETC	INTM					; set INTM to protect context restore
		POP   	XT
		POP   	XAR7
		POP   	XAR6
		POP   	XAR5
		POP   	XAR4
		POP   	XAR3
		POP   	XAR2
		POP   	AR1H:AR0H
		IRET							; return from interrupt
		.end

; end of file


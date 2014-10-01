;------------------------------------------------------------------------------
;	FILE:			Motor_PFC-DPL-ISR.asm
;
;	Description:	Motor_PFC-DPL-ISR.asm contains the ISR which controls
;					the PFC circuit on the motor control system.
;					It also contains the initialization routine for all the
;					Digital Power Library macros being used in the system.
;   
;------------------------------------------------------------------------------

		;Gives peripheral addresses visibility in assembly
	    .cdecls   C,LIST,"PeripheralHeaderIncludes.h"

		;include C header file - sets INCR_BUILD etc.(used in conditional builds)
		.cdecls C,NOLIST, "HVACI_Sensorless_PFC-Settings.h"

		;Include files for the Power Library Macro's used
		.include "ADCDRV_1ch.asm"
		.include "PFC_ICMD.asm"
		.include "PWMDRV_2ch_UpDwnCnt.asm" 
		
		.include "CNTL_2P2Z.asm"
		.include "MATH_EMAVG.asm"
		.include "PFC_INVSQR.asm"
		.include "PFC_InvRmsSqr.asm"
			
;==============================================================================
; Digital Power library - Initailization Routine 
;==============================================================================
		; label to DP initialization function
		.def _DPL_Init	

		; dummy variable for pointer initialization
ZeroNet	 .usect "ZeroNet_Section",2,1,1	; output terminal 1
VloopCtr .usect "ISRVariables",2,1,1
	
		.text
_DPL_Init:
		ZAPA
		MOVL	XAR0, #ZeroNet
		MOVL	*XAR0, ACC

; Initialize all the DP library macro used here 
		.if(INCR_BUILD = 1)
			ADCDRV_1ch_INIT 5			; Ipfc
			ADCDRV_1ch_INIT 4			; Vbus
			ADCDRV_1ch_INIT 6			; VL_fb
			ADCDRV_1ch_INIT 8			; VN_fb

			MATH_EMAVG_INIT 2			; VbusAvg
		
			PWMDRV_2ch_UpDwnCnt_INIT 4	; PWM4A
		.endif

		.if(INCR_BUILD = 2)
			ADCDRV_1ch_INIT 5			; Ipfc
			ADCDRV_1ch_INIT 4			; Vbus
			ADCDRV_1ch_INIT 6			; VL_fb
			ADCDRV_1ch_INIT 8			; VN_fb

			CNTL_2P2Z_INIT 1			; Current loop controller

			MATH_EMAVG_INIT 1			; VrectAvg
			MATH_EMAVG_INIT 2			; VbusAvg
			
			PFC_InvRmsSqr_INIT 1		; Vinv

			PFC_ICMD_INIT 1				; PFC current command
			
			PWMDRV_2ch_UpDwnCnt_INIT 4	; PWM4A
		.endif

		.if(INCR_BUILD = 3)
			ADCDRV_1ch_INIT 5			; Ipfc
			ADCDRV_1ch_INIT 4			; Vbus
			ADCDRV_1ch_INIT 6			; VL_fb
			ADCDRV_1ch_INIT 8			; VN_fb

			CNTL_2P2Z_INIT 1			; Current loop controller
			CNTL_2P2Z_INIT 2			; Voltage loop controller
			
			MATH_EMAVG_INIT 1			; VrectAvg
			MATH_EMAVG_INIT 2			; VbusAvg

			PFC_InvRmsSqr_INIT 1		; Vinv

			PFC_ICMD_INIT 1				; PFC current command
			
			PWMDRV_2ch_UpDwnCnt_INIT 4	; PWM4A
		.endif
		LRETR

;------------------------------------------------------------------------------
; Digital Power library - Interrupt Service Routine

		.sect "ramfuncs"
		.def	_DPL_ISR		; label to DP ISR Run function

_DPL_ISR:
	; Context save - push any unprotected registers onto stack
		PUSH  	AR1H:AR0H
		PUSH  	XAR2
		PUSH  	XAR3
		PUSH  	XAR4
		PUSH  	XAR5
		PUSH  	XAR6
		PUSH  	XAR7
		PUSH  	XT
		SPM   	0          		; set C28 mode
		CLRC  	AMODE       
		CLRC  	PAGE0,OVM 

	; Call DP library modules
		.if(INCR_BUILD = 1)
			ADCDRV_1ch 5			; Ipfc
			ADCDRV_1ch 4			; Vbus
			ADCDRV_1ch 6			; VL_fb
			ADCDRV_1ch 8			; VN_fb
			
			MATH_EMAVG 2			; Calculate VbusAvg
			
			PWMDRV_2ch_UpDwnCnt 4	; Update PWM4A duty
		.endif

		.if(INCR_BUILD = 2)
			ADCDRV_1ch 5			; Ipfc
			ADCDRV_1ch 4			; Vbus
			ADCDRV_1ch 6			; VL_fb
			ADCDRV_1ch 8			; VN_fb

			PFC_ICMD 1				; Calculate PFC current command
			
			CNTL_2P2Z 1				; Run current loop controller
			PWMDRV_2ch_UpDwnCnt 4	; Update PWM4A duty
			
			MATH_EMAVG 1			; Calculate VrectAvg
		    PFC_InvRmsSqr 1			; Calculate Vinv
			MATH_EMAVG 2			; Calculate VbusAvg
			
		.endif

		.if(INCR_BUILD = 3)
			ADCDRV_1ch 5			; Ipfc
			ADCDRV_1ch 4			; Vbus
			ADCDRV_1ch 6			; VL_fb
			ADCDRV_1ch 8			; VN_fb
	
			PFC_ICMD 1				; Caluclate PFC current command
			
			CNTL_2P2Z 1				; Run current loop controller
			PWMDRV_2ch_UpDwnCnt 4	; Update PWM4A duty
			
			MATH_EMAVG 1			; Calculate VrectAvg
		    PFC_InvRmsSqr 1			; Calculate Vinv
			MATH_EMAVG 2			; Calculate VbusAvg
			
			; Execute Vloop every VoltCurrLoopExecRatio times, defined in HVACI_Sensorless_PFC-Settings.h
			MOVW	DP, #(VloopCtr)
			INC		@VloopCtr
			CMP		@VloopCtr, #VoltCurrLoopExecRatio
			B		SKIP_VLOOP_CALC, LT

			MOV		@VloopCtr, #0
			CNTL_2P2Z 2					; Run voltage loop controller
SKIP_VLOOP_CALC:
		.endif

	; Calculate Vrect
        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
    	MOV		ACC, @_AdcResult.ADCRESULT6<<12		; ACC = Line
    	SUB		ACC, @_AdcResult.ADCRESULT8<<12		; ACC = Line - Neutral
    	B		NegativeCycle, LEQ					; Branch to Negative Half Cycle

PositiveCycle:
	; Save Vrect
		.ref	_Vrect
    	MOVW	DP, #_Vrect
		MOVL	@_Vrect, ACC
		B		ControlLoopEnd, UNC		; Do not execute NegativeCycle

NegativeCycle:
	; Save Vrect
    	MOV		ACC, @_AdcResult.ADCRESULT8<<12		; ACC = Neutral
    	SUB		ACC, @_AdcResult.ADCRESULT6<<12		; ACC = Neutral - Line
    	MOVW	DP, #_Vrect
		MOVL	@_Vrect, ACC

ControlLoopEnd:
	; Subtract Ipfc CS amplifier offset and calculate Ipfc filtered
        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
    	MOV 	ACC, @_AdcResult.ADCRESULT5<<12		; ACC = IL_raw
    	SUB		ACC, #28<<12		; ACC = IL_raw - offset, IL CS OPAMP offset = 0.026*3.3V = 85.8mV
    	; SUB		ACC, #98<<12		; ACC = IL_raw - offset, IL CS OPAMP offset = 0.026*3.3V = 85.8mV
									; Therefore, 12bit offset value = 0.024*4095 = 98
    	MOVB	XAR2, #0
		MAXL	ACC, @XAR2			; set min value to zero

	; Save Ipfc filtered
		.ref	_Ipfc_fltr
    	MOVW	DP, #_Ipfc_fltr
		MOVL	@_Ipfc_fltr, ACC
			
	; Interrupt management before exit

	; ISR is triggered by ADC
	;	MOVW 	DP,#_AdcRegs.ADCINTFLGCLR
	;	MOV 	@AdcRegs.ADCINTFLGCLR, #0x01	; Clear ADCINT1 Flag
		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 1
		MOV 	@_PieCtrlRegs.PIEACK, #0x1

	; Context restore
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

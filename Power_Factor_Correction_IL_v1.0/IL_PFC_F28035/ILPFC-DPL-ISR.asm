;----------------------------------------------------------------------------------
;	FILE:			ILPFC-DPL-ISR.asm
;
;	Description:	ILPFC-DPL-ISR.asm contains the ISR for the system
;					It also contains the initailization routine for all the macros 
;					being used in the system
;   
;   Revision/ Version: See ILPFC-Main.c
;----------------------------------------------------------------------------------

		;Gives peripheral addresses visibility in assembly
	    .cdecls   C,LIST,"PeripheralHeaderIncludes.h"

		;include C header file - sets INCR_BUILD etc.(used in conditional builds)
		.cdecls C,NOLIST, "ILPFC-Settings.h"

		;Include files for the Power Library Maco's being used by the system
		.include "ADCDRV_1ch.asm"
		.include "PFC_ICMD.asm"
		.include "PFC_BL_ICMD.asm"
		.include "PWMDRV_2ch_UpDwnCnt.asm" 
		
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
			;ADCDRV_1ch_INIT 1	; Ipfc
			;ADCDRV_1ch_INIT 2	; Vpfc
			;ADCDRV_1ch_INIT 3	; VL_fb
			;ADCDRV_1ch_INIT 4	; VN_fb
			
			;ADCDRV_1ch_INIT 1	; Ipfc
			;ADCDRV_1ch_INIT 2	; Ipfc
			;ADCDRV_1ch_INIT 3	; Ipfc
			;ADCDRV_1ch_INIT 4	; Ipfc
			;ADCDRV_1ch_INIT 5	; Vpfc
			;ADCDRV_1ch_INIT 6	; VL_fb
			;ADCDRV_1ch_INIT 7	; VN_fb
			
			ADCDRV_1ch_INIT 1	; Ipfc
			ADCDRV_1ch_INIT 2	; Ipfc
			ADCDRV_1ch_INIT 3	; Ipfc
			ADCDRV_1ch_INIT 4	; Ipfc
			ADCDRV_1ch_INIT 5	; Ipfc
			ADCDRV_1ch_INIT 6	; Ipfc
			ADCDRV_1ch_INIT 7	; Ipfc
			ADCDRV_1ch_INIT 8	; Ipfc
			ADCDRV_1ch_INIT 9	; Vpfc
			ADCDRV_1ch_INIT 10	; VL_fb
			ADCDRV_1ch_INIT 11	; VN_fb

			PFC_InvRmsSqr_INIT 1
			MATH_EMAVG_INIT 2			
		
			PFC_ICMD_INIT 1	;PFC current command init
		
;			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_2ch_UpDwnCnt_INIT 1	; PWM1A/1B
;			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			;PWMDRV_2ch_UpDwnCnt_INIT 4	; PWM4A
			
		.endif
;---------------------------------------------------------------------	
;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			;ADCDRV_1ch_INIT 1	; Ipfc
			;ADCDRV_1ch_INIT 2	; Vpfc
			;ADCDRV_1ch_INIT 3	; VL_fb
			;ADCDRV_1ch_INIT 4	; VN_fb

			;ADCDRV_1ch_INIT 1	; Ipfc
			;ADCDRV_1ch_INIT 2	; Ipfc
			;ADCDRV_1ch_INIT 3	; Ipfc
			;ADCDRV_1ch_INIT 4	; Ipfc
			;ADCDRV_1ch_INIT 5	; Vpfc
			;ADCDRV_1ch_INIT 6	; VL_fb
			;ADCDRV_1ch_INIT 7	; VN_fb
			
			ADCDRV_1ch_INIT 1	; Ipfc
			ADCDRV_1ch_INIT 2	; Ipfc
			ADCDRV_1ch_INIT 3	; Ipfc
			ADCDRV_1ch_INIT 4	; Ipfc
			ADCDRV_1ch_INIT 5	; Ipfc
			ADCDRV_1ch_INIT 6	; Ipfc
			ADCDRV_1ch_INIT 7	; Ipfc
			ADCDRV_1ch_INIT 8	; Ipfc
			ADCDRV_1ch_INIT 9	; Vpfc
			ADCDRV_1ch_INIT 10	; VL_fb
			ADCDRV_1ch_INIT 11	; VN_fb
			

			CNTL_2P2Z_INIT 1
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			
			
			PFC_InvRmsSqr_INIT 1

			PFC_ICMD_INIT 1	;PFC current command init
			
;			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			
			PWMDRV_2ch_UpDwnCnt_INIT 1	; PWM1A/1B

;			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
;			PWMDRV_2ch_UpDwnCnt_INIT 4	; PWM4A
			
		.endif
;---------------------------------------------------------------------		
		.if(INCR_BUILD = 3)
			;ADCDRV_1ch_INIT 1	; Ipfc
			;ADCDRV_1ch_INIT 2	; Vpfc
			;ADCDRV_1ch_INIT 3	; VL_fb
			;ADCDRV_1ch_INIT 4	; VN_fb
			
			;ADCDRV_1ch_INIT 1	; Ipfc
			;ADCDRV_1ch_INIT 2	; Ipfc
			;ADCDRV_1ch_INIT 3	; Ipfc
			;ADCDRV_1ch_INIT 4	; Ipfc
			;ADCDRV_1ch_INIT 5	; Vpfc
			;ADCDRV_1ch_INIT 6	; VL_fb
			;ADCDRV_1ch_INIT 7	; VN_fb
			
			ADCDRV_1ch_INIT 1	; Ipfc
			ADCDRV_1ch_INIT 2	; Ipfc
			ADCDRV_1ch_INIT 3	; Ipfc
			ADCDRV_1ch_INIT 4	; Ipfc
			ADCDRV_1ch_INIT 5	; Ipfc
			ADCDRV_1ch_INIT 6	; Ipfc
			ADCDRV_1ch_INIT 7	; Ipfc
			ADCDRV_1ch_INIT 8	; Ipfc
			ADCDRV_1ch_INIT 9	; Vpfc
			ADCDRV_1ch_INIT 10	; VL_fb
			ADCDRV_1ch_INIT 11	; VN_fb

			CNTL_2P2Z_INIT 2
			CNTL_2P2Z_INIT 1
			
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			

			PFC_InvRmsSqr_INIT 1


			PFC_ICMD_INIT 1	;Bridgeless PFC current command init
			
			
;			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	

			PWMDRV_2ch_UpDwnCnt_INIT 1	; PWM1A/1B

;			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
;			PWMDRV_2ch_UpDwnCnt_INIT 4	; PWM4A
			
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
			;ADCDRV_1ch 1	; Ipfc
			;ADCDRV_1ch 2	; Vpfc
			;ADCDRV_1ch 3	; VL_fb
			;ADCDRV_1ch 4	; VN_fb
			
			;ADCDRV_1ch 1	; Ipfc
			;ADCDRV_1ch 2	; Ipfc
			;ADCDRV_1ch 3	; Ipfc
			;ADCDRV_1ch 4	; Ipfc
			;ADCDRV_1ch 5	; Vpfc
			;ADCDRV_1ch 6	; VL_fb
			;ADCDRV_1ch 7	; VN_fb
			
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Ipfc
			ADCDRV_1ch 3	; Ipfc
			ADCDRV_1ch 4	; Ipfc
			ADCDRV_1ch 5	; Ipfc
			ADCDRV_1ch 6	; Ipfc
			ADCDRV_1ch 7	; Ipfc
			ADCDRV_1ch 8	; Ipfc
			ADCDRV_1ch 9	; Vpfc
			ADCDRV_1ch 10	; VL_fb
			ADCDRV_1ch 11	; VN_fb
			
						;Subtract Ipfc CS amplifier offset and calculate Ipfc filtered
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	;MOV 	ACC, @_AdcResult.ADCRESULT1<<12		; ACC = IL_raw
    		;Commented the above for testing oversampling
    		
    ;Test code for oversampling***********    	
        	;MOV 	ACC, @_AdcResult.ADCRESULT1<<10		; ACC = IL_raw, Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT2<<10		;Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT3<<10		;Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT4<<10		;Q24/4, Add 4 samples of current,4*(Q24/4) = Q24
        	
        	MOV 		ACC, @_AdcResult.ADCRESULT1<<9		; ACC = IL_raw, Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT2<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT3<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT4<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT5<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT6<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT7<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT8<<9		;Q24/8, Add 8 samples of current,8*(Q24/8) = Q24
        	
    ;end of test code
    		
    		;SUB		ACC, #24<<12
    		;SUB		ACC, #20<<12
        	;SUB		ACC, #90<<12		; ACC = IL_raw - offset, IL CS OPAMP offset = 0.026*3.3V = 85.8mV
										; Therefore, 12bit offset value = 0.026*4095 = 106
        	
        	MOVB	XAR2, #0							
			MAXL	ACC, @XAR2			; set min value to zero					
		; Save Ipfc filtered
			.ref	_Ipfc_fltr
        	MOVW	DP, #_Ipfc_fltr
			MOVL	@_Ipfc_fltr, ACC			
			
			PFC_ICMD 1	;PFC current command
			
;			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_2ch_UpDwnCnt 1		; PWM1A/PWM1B
;			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			
			PFC_InvRmsSqr  	1
			
			;Subtract Vbus offset
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	MOV 	ACC, @_AdcResult.ADCRESULT9<<12		; ACC = Vbus_raw
        	SUB		ACC, #16<<12		; ACC = Vbus_raw - offset, Vbus offset = 16 (max value 4095)
										; 12bit offset value = 0.004*4095 = 16
        	
        	MOVB	XAR2, #0							
			MAXL	ACC, @XAR2			; set min value to zero					
		; Save Vbus filtered
			.ref	_Vbus
        	MOVW	DP, #_Vbus
			MOVL	@_Vbus, ACC
        	
			MATH_EMAVG		2			;calculate average Vbus
			
		.endif
;----------------------------------------------
;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			;ADCDRV_1ch 1	; Ipfc
			;ADCDRV_1ch 2	; Vpfc
			;ADCDRV_1ch 3	; VL_fb
			;ADCDRV_1ch 4	; VN_fb

			;ADCDRV_1ch 1	; Ipfc
			;ADCDRV_1ch 2	; Ipfc
			;ADCDRV_1ch 3	; Ipfc
			;ADCDRV_1ch 4	; Ipfc
			;ADCDRV_1ch 5	; Vpfc
			;ADCDRV_1ch 6	; VL_fb
			;ADCDRV_1ch 7	; VN_fb
			
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Ipfc
			ADCDRV_1ch 3	; Ipfc
			ADCDRV_1ch 4	; Ipfc
			ADCDRV_1ch 5	; Ipfc
			ADCDRV_1ch 6	; Ipfc
			ADCDRV_1ch 7	; Ipfc
			ADCDRV_1ch 8	; Ipfc
			ADCDRV_1ch 9	; Vpfc
			ADCDRV_1ch 10	; VL_fb
			ADCDRV_1ch 11	; VN_fb
			
						;Subtract Ipfc CS amplifier offset and calculate Ipfc filtered
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	;MOV 	ACC, @_AdcResult.ADCRESULT1<<12		; ACC = IL_raw
    		;Commented the above for testing oversampling
    		
    ;Test code for oversampling***********    	
        	;MOV 	ACC, @_AdcResult.ADCRESULT1<<10		; ACC = IL_raw, Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT2<<10		;Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT3<<10		;Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT4<<10		;Q24/4, Add 4 samples of current,4*(Q24/4) = Q24
        	
        	MOV 		ACC, @_AdcResult.ADCRESULT1<<9		; ACC = IL_raw, Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT2<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT3<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT4<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT5<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT6<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT7<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT8<<9		;Q24/8, Add 8 samples of current,8*(Q24/8) = Q24
        	
    ;end of test code
    		
    		;SUB		ACC, #20<<12
        	;SUB		ACC, #90<<12		; ACC = IL_raw - offset, IL CS OPAMP offset = 0.026*3.3V = 85.8mV
										; Therefore, 12bit offset value = 0.026*4095 = 106
 
        	MOVB	XAR2, #0							
			MAXL	ACC, @XAR2			; set min value to zero					
		; Save Ipfc filtered
			.ref	_Ipfc_fltr
        	MOVW	DP, #_Ipfc_fltr
			MOVL	@_Ipfc_fltr, ACC			
			

			PFC_ICMD 1	;PFC current command
			
			CNTL_2P2Z		1
			
;			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_2ch_UpDwnCnt 1		; PWM1A/1B
;			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			
			MATH_EMAVG		1						
		    PFC_InvRmsSqr  	1
		    
		    
		    ;Subtract Vbus offset
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	MOV 	ACC, @_AdcResult.ADCRESULT9<<12		; ACC = Vbus_raw
        	SUB		ACC, #16<<12		; ACC = Vbus_raw - offset, Vbus offset = 16 (max value 4095)
										; 12bit offset value = 0.004*4095 = 16
        	
        	MOVB	XAR2, #0							
			MAXL	ACC, @XAR2			; set min value to zero					
		; Save Vbus filtered
			.ref	_Vbus
        	MOVW	DP, #_Vbus
			MOVL	@_Vbus, ACC
		    
			MATH_EMAVG		2			;calculate average Vbus
			
		.endif
;----------------------------------------------
		.if(INCR_BUILD = 3)
			;ADCDRV_1ch 1	; Ipfc
			;ADCDRV_1ch 2	; Vpfc
			;ADCDRV_1ch 3	; VL_fb
			;ADCDRV_1ch 4	; VN_fb
			
			;ADCDRV_1ch 1	; Ipfc
			;ADCDRV_1ch 2	; Ipfc
			;ADCDRV_1ch 3	; Ipfc
			;ADCDRV_1ch 4	; Ipfc
			;ADCDRV_1ch 5	; Vpfc
			;ADCDRV_1ch 6	; VL_fb
			;ADCDRV_1ch 7	; VN_fb
			
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Ipfc
			ADCDRV_1ch 3	; Ipfc
			ADCDRV_1ch 4	; Ipfc
			ADCDRV_1ch 5	; Ipfc
			ADCDRV_1ch 6	; Ipfc
			ADCDRV_1ch 7	; Ipfc
			ADCDRV_1ch 8	; Ipfc
			ADCDRV_1ch 9	; Vpfc
			ADCDRV_1ch 10	; VL_fb
			ADCDRV_1ch 11	; VN_fb
			
;-------------------------Moved ADC average calculation here---Improves Phase Margin------

			;Subtract Ipfc CS amplifier offset and calculate Ipfc filtered
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	;MOV 	ACC, @_AdcResult.ADCRESULT1<<12		; ACC = IL_raw
    		;Commented the above for testing oversampling
    		
    ;Test code for oversampling***********    	
        	;MOV 	ACC, @_AdcResult.ADCRESULT1<<10		; ACC = IL_raw, Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT2<<10		;Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT3<<10		;Q24/4
        	;ADD		ACC, @_AdcResult.ADCRESULT4<<10		;Q24/4, Add 4 samples of current,4*(Q24/4) = Q24
        	
        	MOV 	ACC, @_AdcResult.ADCRESULT1<<9		; ACC = IL_raw, Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT2<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT3<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT4<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT5<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT6<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT7<<9		;Q24/8
        	ADD		ACC, @_AdcResult.ADCRESULT8<<9		;Q24/8, Add 8 samples of current,8*(Q24/8) = Q24
        	
    ;end of test code
    		
    		;SUB		ACC, #24<<12
    		;SUB		ACC, #20<<12		; No need for offset calibration since ADC offset calibration is performed
        	;SUB		ACC, #90<<12		; ACC = IL_raw - offset, IL CS OPAMP offset = 0.026*3.3V = 85.8mV
										; Therefore, 12bit offset value = 0.026*4095 = 106

        	MOVB	XAR2, #0
			MAXL	ACC, @XAR2			; set min value to zero
		; Save Ipfc filtered
			.ref	_Ipfc_fltr
        	MOVW	DP, #_Ipfc_fltr
			MOVL	@_Ipfc_fltr, ACC			
			
;---------------------------------------	
			PFC_ICMD 1	;PFC current command
			
			CNTL_2P2Z		1
			
;			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_2ch_UpDwnCnt 1		; PWM1A/1B
;			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			
;			PWMDRV_2ch_UpDwnCnt 4		; PWM4A
			
			PFC_InvRmsSqr  	1
			
			
		    ;Subtract Vbus offset
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	MOV 	ACC, @_AdcResult.ADCRESULT9<<12		; ACC = Vbus_raw
        	SUB		ACC, #16<<12		; ACC = Vbus_raw - offset, Vbus offset = 16 (max value 4095)
										; 12bit offset value = 0.004*4095 = 16
        	
        	MOVB	XAR2, #0							
			MAXL	ACC, @XAR2			; set min value to zero					
		; Save Vbus filtered
			.ref	_Vbus
        	MOVW	DP, #_Vbus
			MOVL	@_Vbus, ACC
		    
			MATH_EMAVG		2			;calculate average Vbus
			
			
;Execute Vloop every VoltCurrLoopExecRatio times, defined in BridgelessPFC-Settings.h file 	
		MOVW	DP,#(VloopCtr)
		INC		@VloopCtr
		CMP		@VloopCtr,#VoltCurrLoopExecRatio
		B		SKIP_VLOOP_CALC,LT
		
		MOV		@VloopCtr,#0
		CNTL_2P2Z		2			;Volt loop controller
		
SKIP_VLOOP_CALC:
			
		.endif
		
;End of Build 3		
;----------------------------------------------------			
					
;Calculate Vrect
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	;MOV		ACC, @_AdcResult.ADCRESULT3<<12		; ACC = Line
        	;SUB		ACC, @_AdcResult.ADCRESULT4<<12		; ACC = Line - Neutral
        	
        	;MOV		ACC, @_AdcResult.ADCRESULT6<<12		; ACC = Line
        	;SUB		ACC, #29<<12						;*************Offset correction for higher Line sense volt********
        	;SUB		ACC, @_AdcResult.ADCRESULT7<<12		; ACC = Line - Neutral
        	
        	MOV		ACC, @_AdcResult.ADCRESULT10<<12		; ACC = Line
        	;SUB		ACC, #29<<12						;Donot use for final Rev1.1 board. Corrupts Vrms and Freq measurement.
        													;Offset correction for higher Line sense volt********
        	SUB		ACC, @_AdcResult.ADCRESULT11<<12		; ACC = Line - Neutral
        	
        	B		NegativeCycle, LEQ					; Branch to Negative Half Cycle
PositiveCycle:
	; Save Vrect
			.ref	_Vrect
        	MOVW	DP, #_Vrect
			MOVL	@_Vrect, ACC
			
			B		ControlLoopEnd, UNC ;***********Added this for ILPFC system***************



NegativeCycle:
	; Save Vrect
        	;MOV		ACC, @_AdcResult.ADCRESULT4<<12		; ACC = Neutral
        	;SUB		ACC, @_AdcResult.ADCRESULT3<<12		; ACC = Neutral - Line
        	
        	;MOV		ACC, @_AdcResult.ADCRESULT7<<12		; ACC = Neutral
        	;SUB		ACC, @_AdcResult.ADCRESULT6<<12		; ACC = Neutral - Line
        	;ADD		ACC, #29<<12						;*************Offset correction for higher Line sense volt********
        	
        	MOV		ACC, @_AdcResult.ADCRESULT11<<12		; ACC = Neutral
        	SUB		ACC, @_AdcResult.ADCRESULT10<<12		; ACC = Neutral - Line
        	;ADD		ACC, #29<<12						;*************Offset correction for higher Line sense volt********
        													;**Donot use for final Rev1.1 board. Corrupts Vrms and Freq measurement.
        	MOVW	DP, #_Vrect
			MOVL	@_Vrect, ACC



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


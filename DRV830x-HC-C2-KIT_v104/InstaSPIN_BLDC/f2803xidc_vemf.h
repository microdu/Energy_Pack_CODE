/* ==================================================================================
File name:       F2803XIDC_VEMF.H
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: This header file contains macro definition for ADC initilization 
(In this project ADC is used for DC bus current measurement and phase BEMF measurement)

This drv file is specific to "DRV8312" kit

Target: TMS320F2803x family
              
=====================================================================================
History:
-------------------------------------------------------------------------------------
 02-09-2011	Version 1.0 
------------------------------------------------------------------------------------*/

#ifndef __F2803XIDC_VEMF_H__
#define __F2803XIDC_VEMF_H__

#include "f2803xbmsk.h"

/*------------------------------------------------------------------------------
 ADC Initialization Macro Definition 
------------------------------------------------------------------------------*/


#define CPU_CLOCK_SPEED      16.667L   // 10.000L for a 100MHz CPU clock speed
#define ADC_usDELAY 10000L
#define DELAY_US(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_CLOCK_SPEED) - 9.0L) / 5.0L)

extern void DSP28x_usDelay(unsigned long Count);


#define ADC_MACRO_INIT()																				\
																								\
 DELAY_US(ADC_usDELAY);																			\
    AdcRegs.ADCCTL1.all=ADC_RESET_FLAG;															\
	asm(" NOP ");																				\
	asm(" NOP ");    																			\
																								\
	EALLOW;																						\
	 AdcRegs.ADCCTL1.bit.ADCBGPWD	= 1;	/* Power up band gap */								\
																								\
	DELAY_US(ADC_usDELAY);					/* Delay before powering up rest of ADC */			\
																								\
	AdcRegs.ADCCTL1.bit.ADCREFSEL	= 0;														\
   	AdcRegs.ADCCTL1.bit.ADCREFPWD	= 1;	/* Power up reference */							\
   	AdcRegs.ADCCTL1.bit.ADCPWDN 	= 1;	/* Power up rest of ADC */							\
	AdcRegs.ADCCTL1.bit.ADCENABLE	= 1;	/* Enable ADC */									\
																								\
	asm(" RPT#100 || NOP");																		\
																								\
	AdcRegs.ADCCTL1.bit.INTPULSEPOS=1;															\
	AdcRegs.ADCCTL1.bit.TEMPCONV=0;																\
																								\
	DELAY_US(ADC_usDELAY);																		\
																								\
	/******* CHANNEL SELECT *******/															\
																								\
																								\
	AdcRegs.ADCSOC0CTL.bit.CHSEL 	= 15;	/*Dummy meas. avoid 1st sample issue Rev0 Picollo*/	\
	AdcRegs.ADCSOC0CTL.bit.TRIGSEL  = 5;														\
	AdcRegs.ADCSOC0CTL.bit.ACQPS 	= 6;														\
																								\
	AdcRegs.ADCSOC1CTL.bit.CHSEL 	= 15; 	/* ChSelect: ADC B7-> V Phase A */					\
	AdcRegs.ADCSOC1CTL.bit.TRIGSEL 	= 5;	/* Set SOC0 start trigger on EPWM1A, due to round-robin SOC0 converts first then SOC1*/	\
	AdcRegs.ADCSOC1CTL.bit.ACQPS 	= 6;	/* Set SOC0 S/H Window to 7 ADC Clock Cycles, (6 ACQPS plus 1)*/						\
																								\
	AdcRegs.ADCSOC2CTL.bit.CHSEL 	= 7;	/* ChSelect: ADC A7-> V Phase B */					\
	AdcRegs.ADCSOC2CTL.bit.TRIGSEL  = 5;														\
	AdcRegs.ADCSOC2CTL.bit.ACQPS 	= 6;														\
																								\
	AdcRegs.ADCSOC3CTL.bit.CHSEL 	= 12;	/* ChSelect: ADC B4-> V Phase C */ 					\
	AdcRegs.ADCSOC3CTL.bit.TRIGSEL  = 5;														\
	AdcRegs.ADCSOC3CTL.bit.ACQPS 	= 6;														\
																								\
	AdcRegs.ADCSOC4CTL.bit.CHSEL 	= 2;	/* ChSelect: ADC A2-> Low Side DC Bus Return Cur.*/	\
	AdcRegs.ADCSOC4CTL.bit.TRIGSEL 	= 6;														\
	AdcRegs.ADCSOC4CTL.bit.ACQPS 	= 6;													 	\
																								\
	AdcRegs.INTSEL1N2.bit.INT1E     = 1;	/* Enabled ADCINT1 */								\
	AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;	/* Disable ADCINT1 Continuous mode */				\
	AdcRegs.INTSEL1N2.bit.INT1SEL	= 4;	/* Setup EOC4 to trigger ADCINT1 to fire */			\
																								\
	EDIS;																						\
																								\
    /* Set up Event Trigger with CNT_zero enable for Time-base of EPWM1 */						\
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;     /* Enable SOCA */										\
    EPwm1Regs.ETSEL.bit.SOCASEL = 2;    /* Enable TBCNTR = PRD event for SOCA */				\
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;     /* Generate SOCA on the 1st event */					\
	EPwm1Regs.ETCLR.bit.SOCA = 1;       /* Clear SOCA flag */									\
																								\
	/* Set up and SOCB pulse for current sampling*/												\
    EPwm1Regs.ETSEL.bit.SOCBEN = 1;     /* Enable SOCB */										\
    EPwm1Regs.ETSEL.bit.SOCBSEL = 6;    /* Enable TBCNTR = CMPB incrementing event for SOCB */	\
    EPwm1Regs.ETPS.bit.SOCBPRD = 1;     /* Generate SOCB on the 1st event */					\
	EPwm1Regs.ETCLR.bit.SOCB = 1;       /* Clear SOCB flag */

#endif // __F2803XIDC_VEMF_H__

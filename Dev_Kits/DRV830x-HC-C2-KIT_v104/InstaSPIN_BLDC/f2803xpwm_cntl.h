/* ==================================================================================
File name:        F2803XPWM_CNTL.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments
Description:  
Header file containing data type and object definitions and 
initializers.

Target: TMS320F2803x family
              
=====================================================================================
History:
-------------------------------------------------------------------------------------
 06-06-2011	Version 1.0: 
------------------------------------------------------------------------------------*/

#ifndef __F2803X_PWM_CNTL_H__
#define __F2803X_PWM_CNTL_H__

#include "f2803xbmsk.h"
#include "PeripheralHeaderIncludes.h"

/*----------------------------------------------------------------------------
Initialization constant for the F280X Time-Base Control Registers for PWM Generation. 
Sets up the timer to run free upon emulation suspend, count up mode
prescaler 1.
----------------------------------------------------------------------------*/
#define PWM_CNTL_INIT_STATE ( FREE_RUN_FLAG +         \
                            PRDLD_SHADOW  +       \
                            TIMER_CNT_UPDN +         \
                            HSPCLKDIV_PRESCALE_X_1 + \
                            CLKDIV_PRESCALE_X_1  +   \
                            PHSDIR_CNT_UP    +       \
                            CNTLD_DISABLE )

/*----------------------------------------------------------------------------
Initialization constant for the F280X Compare Control Register. 
----------------------------------------------------------------------------*/
#define PWM_CNTL_CMPCTL_INIT_STATE ( LOADAMODE_ZRO + \
                                   LOADBMODE_ZRO + \
                                   SHDWAMODE_SHADOW + \
                                   SHDWBMODE_SHADOW )

/*----------------------------------------------------------------------------
Initialization constant for the F280X Action Qualifier Output A Register. 
----------------------------------------------------------------------------*/
#define  PWM_CNTL_AQCTLA_INIT_STATE  (CAU_CLEAR + CAD_SET)

/*----------------------------------------------------------------------------
Initialization constant for the F280X Dead-Band Control Generator register.
----------------------------------------------------------------------------*/
#define  PWM_CNTL_DBCTL_INIT_STATE  BP_DISABLE

/*----------------------------------------------------------------------------
Initialization constant for the F280X PWM Chopper Control register for PWM Generation. 
----------------------------------------------------------------------------*/
#define  PWM_CNTL_PCCTL_INIT_STATE  CHPEN_DISABLE

/*----------------------------------------------------------------------------
Initialization constant for the F280X Trip Zone Select Register 
----------------------------------------------------------------------------*/
#define  PWM_CNTL_TZSEL_INIT_STATE  DISABLE_TZSEL
              

/*-----------------------------------------------------------------------------
Define the structure of the PWM Driver Object 
-----------------------------------------------------------------------------*/

typedef struct {	
		Uint16	State;			// input:  Current state range [0, 5]
		_iq		Duty;			// input:  Duty cycle input range [-1.0, 1.0]
		Uint16	PWMprd;			// input:  Period of the pwm counter  
		}PWM_CNTL;

/*-----------------------------------------------------------------------------
Define a PWMGEN_handle
-----------------------------------------------------------------------------*/
typedef PWM_CNTL *PWM_CNTL_handle;

/*------------------------------------------------------------------------------
Default Initializers for the F281X PWMGEN Object 
------------------------------------------------------------------------------*/
#define F2803X_PWM_CNTL			{0, \
								_IQ(0.0), \
								800 \
								}
							
#define PWM_CNTL_DEFAULTS	F2803X_PWM_CNTL

/*------------------------------------------------------------------------------
	F280XPWM_CNTL Macro Definitions
------------------------------------------------------------------------------*/

#define PWM_CNTL_INIT_MACRO(v)														\
																					\
		EALLOW;																		\
		SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;										\
		EDIS;																		\
																					\
        /* Init Timer-Base Period Register for EPWM1-EPWM3*/						\
		EPwm1Regs.TBPRD = v.PWMprd;													\
		EPwm2Regs.TBPRD = v.PWMprd;													\
		EPwm3Regs.TBPRD = v.PWMprd;													\
																					\
        /* Init Timer-Base Counter Register for EPWM1-EPWM3*/						\
		EPwm1Regs.TBCTR = 0x0000;													\
		EPwm2Regs.TBCTR = 0x0000;													\
		EPwm3Regs.TBCTR = 0x0000;													\
																					\
        /* Init Compare Register for EPWM1-EPWM3 to 50% duty cycle */				\
		EPwm1Regs.CMPA.half.CMPA = v.PWMprd/2;										\
		EPwm2Regs.CMPA.half.CMPA = v.PWMprd/2;										\
		EPwm3Regs.CMPA.half.CMPA = v.PWMprd/2;										\
																					\
        /* Init Timer-Base Phase Register for EPWM1-EPWM3*/							\
		EPwm1Regs.TBPHS.half.TBPHS = 0;                                 			\
		EPwm2Regs.TBPHS.half.TBPHS = 0;                                 			\
		EPwm3Regs.TBPHS.half.TBPHS = 0;                                 			\
																					\
        /* Init Timer-Base Control Register for EPWM1-EPWM3*/						\
		EPwm1Regs.TBCTL.all = PWM_CNTL_INIT_STATE;                                	\
		EPwm2Regs.TBCTL.all = PWM_CNTL_INIT_STATE;                                	\
		EPwm3Regs.TBCTL.all = PWM_CNTL_INIT_STATE;                                	\
																					\
         /* Setup Sync*/															\
		EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;                                 			\
		EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;                                 			\
		EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;                                 			\
																					\
        /* Allow each timer to be sync'ed*/											\
		EPwm1Regs.TBCTL.bit.PHSEN = 0;                                 				\
		EPwm2Regs.TBCTL.bit.PHSEN = 1;                                 				\
		EPwm3Regs.TBCTL.bit.PHSEN = 1;                                 				\
																					\
        /* Set count dir after sync event*/											\
		EPwm1Regs.TBCTL.bit.PHSDIR = 0;												\
		EPwm2Regs.TBCTL.bit.PHSDIR = 1;												\
		EPwm3Regs.TBCTL.bit.PHSDIR = 1;												\
																					\
        /* Init Compare Control Register for EPWM1-EPWM3	*/						\
		EPwm1Regs.CMPCTL.all = PWM_CNTL_CMPCTL_INIT_STATE;                         	\
		EPwm2Regs.CMPCTL.all = PWM_CNTL_CMPCTL_INIT_STATE;                        	\
		EPwm3Regs.CMPCTL.all = PWM_CNTL_CMPCTL_INIT_STATE;                         	\
																					\
        /* Init Action Qualifier Output A Register for EPWM1-EPWM3*/				\
		EPwm1Regs.AQCTLA.all = PWM_CNTL_AQCTLA_INIT_STATE;                         	\
		EPwm2Regs.AQCTLA.all = PWM_CNTL_AQCTLA_INIT_STATE;                         	\
		EPwm3Regs.AQCTLA.all = PWM_CNTL_AQCTLA_INIT_STATE;                         	\
																					\
        /* Init Dead-Band Generator Control Register for EPWM1-EPWM3*/				\
		EPwm1Regs.DBCTL.all = PWM_CNTL_DBCTL_INIT_STATE;                           	\
		EPwm2Regs.DBCTL.all = PWM_CNTL_DBCTL_INIT_STATE;                           	\
		EPwm3Regs.DBCTL.all = PWM_CNTL_DBCTL_INIT_STATE;                           	\
																					\
        /* Init PWM Chopper Control Register for EPWM1-EPWM3*/						\
		EPwm1Regs.PCCTL.all = PWM_CNTL_PCCTL_INIT_STATE;                           	\
		EPwm2Regs.PCCTL.all = PWM_CNTL_PCCTL_INIT_STATE;                           	\
		EPwm3Regs.PCCTL.all = PWM_CNTL_PCCTL_INIT_STATE;                           	\
 																					\
		EALLOW;           /* Enable EALLOW */				            			\
																					\
																					\
        /* Setting six EPWM as primary output pins*/								\
/*         GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;*/                /* EPWM1A pin*/			\
/*         GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;*/                /* EPWM1B pin*/			\
/*         GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;*/                /* EPWM2A pin*/			\
/*         GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;*/                /* EPWM2B pin*/			\
/*         GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;*/                /* EPWM3A pin*/			\
/*         GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;*/                /* EPWM3B pin*/			\
         																			\
		SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;										\
																					\
		EDIS;            /* Disable EALLOW*/ 





	_iq	iqPosDuty = _IQ(0.5);
	_iq iqNegDuty = _IQ(0.5);
	Uint16	uiPosDuty = 0;
	Uint16	uiNegDuty = 0;

#define PWM_CNTL_MACRO(v)																/*													*/\
	iqPosDuty = _IQmpy(v.Duty, _IQ(0.5)) + _IQ(0.5);									/*													*/\
																						/*													*/\
	uiPosDuty = (Uint16) ((_IQ18mpy((_iq)v.PWMprd<<18, _IQtoIQ18(iqPosDuty))) >> 18);	/*													*/\
	uiNegDuty = v.PWMprd - uiPosDuty;													/*													*/\
																						/*													*/\
	/*center the SOCB pulse in the center of the PWM on time for current sampling*/		/*													*/\
	EPwm1Regs.CMPB = (uiPosDuty + uiNegDuty)>>1;										/*													*/\
																						/*													*/\
	switch (v.State)																	/*													*/\
	{																					/*													*/\
/* State s1: current flows to motor windings from phase A->B, de-energized phase = C														*/\
		case 0:																			/*													*/\
			EPwm1Regs.CMPA.half.CMPA = uiPosDuty;										/*													*/\
			EPwm2Regs.CMPA.half.CMPA = uiNegDuty;										/*													*/\
			EALLOW;																		/*													*/\
			PHASE_A_ON;																	/*													*/\
			PHASE_B_ON;																	/*													*/\
			PHASE_C_OFF;																/*													*/\
			EDIS;																		/*													*/\
			break;																		/*													*/\
																						/*													*/\
/* State s2: current flows to motor windings from phase A->C, de-energized phase = B														*/\
		case 1:																			/*													*/\
			EPwm1Regs.CMPA.half.CMPA = uiPosDuty;										/*													*/\
			EPwm3Regs.CMPA.half.CMPA = uiNegDuty;										/*													*/\
			EALLOW;																		/*													*/\
			PHASE_A_ON;																	/*													*/\
			PHASE_C_ON;																	/*													*/\
			PHASE_B_OFF;																/*													*/\
			EDIS;																		/*													*/\
			break;																		/*													*/\
																						/*													*/\
/* State s3: current flows to motor windings from phase B->C, de-energized phase = A														*/\
		case 2:																			/*													*/\
			EPwm2Regs.CMPA.half.CMPA = uiPosDuty;										/*													*/\
			EPwm3Regs.CMPA.half.CMPA = uiNegDuty;										/*													*/\
			EALLOW;																		/*													*/\
			PHASE_B_ON;																	/*													*/\
			PHASE_C_ON;																	/*													*/\
			PHASE_A_OFF;																/*													*/\
			EDIS;																		/*													*/\
			break;																		/*													*/\
																						/*													*/\
/* State s4: current flows to motor windings from phase B->A, de-energized phase = C														*/\
		case 3:																			/*													*/\
			EPwm2Regs.CMPA.half.CMPA = uiPosDuty;										/*													*/\
			EPwm1Regs.CMPA.half.CMPA = uiNegDuty;										/*													*/\
			EALLOW;																		/*													*/\
			PHASE_A_ON;																	/*													*/\
			PHASE_B_ON;																	/*													*/\
			PHASE_C_OFF;																/*													*/\
			EDIS;																		/*													*/\
			break;																		/*													*/\
																						/*													*/\
/* State s5: current flows to motor windings from phase C->A, de-energized phase = B														*/\
		case 4:																			/*													*/\
			EPwm3Regs.CMPA.half.CMPA = uiPosDuty;										/*													*/\
			EPwm1Regs.CMPA.half.CMPA = uiNegDuty;										/*													*/\
			EALLOW;																		/*													*/\
			PHASE_A_ON;																	/*													*/\
			PHASE_C_ON;																	/*													*/\
			PHASE_B_OFF;																/*													*/\
			EDIS;																		/*													*/\
			break;																		/*													*/\
																						/*													*/\
/* State s6: current flows to motor windings from phase C->B, de-energized phase = A														*/\
		case 5:																			/*													*/\
			EPwm3Regs.CMPA.half.CMPA = uiPosDuty;										/*													*/\
			EPwm2Regs.CMPA.half.CMPA = uiNegDuty;										/*													*/\
			EALLOW;																		/*													*/\
			PHASE_B_ON;																	/*													*/\
			PHASE_C_ON;																	/*													*/\
			PHASE_A_OFF;																/*													*/\
			EDIS;																		/*													*/\
	}
    
    #endif  // __F2803X_PWM_CNTL_H__











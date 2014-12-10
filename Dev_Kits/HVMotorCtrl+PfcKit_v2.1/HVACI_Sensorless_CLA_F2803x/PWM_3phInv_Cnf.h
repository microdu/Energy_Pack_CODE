/* ==================================================================================
File name:        PWM_3phInv_Cnf.H                                         
Target   : 		  TMS320F2803x family
=====================================================================================*/
#ifndef __PWM_3phInv_Cnf_H__
#define __PWM_3phInv_Cnf_H__

#include "f2803xbmsk.h"
				  
/*----------------------------------------------------------------------------
Initialization constant for the F2803X Time-Base Control Registers for PWM Generation. 
Sets up the timer to run free upon emulation suspend, count up-down mode
prescaler 1.
----------------------------------------------------------------------------*/
#define PWM_INIT_STATE ( FREE_RUN_FLAG +         \
                         PRDLD_IMMEDIATE  +       \
                         TIMER_CNT_UPDN +         \
                         HSPCLKDIV_PRESCALE_X_1 + \
                         CLKDIV_PRESCALE_X_1  +   \
                         PHSDIR_CNT_UP    +       \
                         CNTLD_DISABLE )

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Compare Control Register. 
----------------------------------------------------------------------------*/
#define CMPCTL_INIT_STATE ( LOADAMODE_ZRO + \
                            LOADBMODE_ZRO + \
                            SHDWAMODE_SHADOW + \
                            SHDWBMODE_SHADOW )

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Action Qualifier Output A Register. 
----------------------------------------------------------------------------*/
#define AQCTLA_INIT_STATE ( CAD_SET + CAU_CLEAR )

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Dead-Band Generator registers for PWM Generation. 
Sets up the dead band for PWM and sets up dead band values.
----------------------------------------------------------------------------*/
#define DBCTL_INIT_STATE  (BP_ENABLE + POLSEL_ACTIVE_HI_CMP)

#define DBCNT_INIT_STATE   100   // 100 counts = 1.66 usec (delay) * 100 count/usec (for TBCLK = SYSCLK/1)

/*----------------------------------------------------------------------------
Initialization constant for the F2803X PWM Chopper Control register for PWM Generation. 
----------------------------------------------------------------------------*/
#define  PCCTL_INIT_STATE  CHPEN_DISABLE

/*----------------------------------------------------------------------------
Initialization constant for the F2803X Trip Zone Select Register 
----------------------------------------------------------------------------*/
#define  TZSEL_INIT_STATE  DISABLE_TZSEL
              
/*------------------------------------------------------------------------------
	PWM Init & PWM Update Macro Definitions
------------------------------------------------------------------------------*/

#define PWM_3phInv_CNF(ch1,ch2,ch3,prd, dbd)										\
	     /* Setup Sync*/													\
         (*ePWM[ch1]).TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/		\
		 (*ePWM[ch2]).TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/		\
		 (*ePWM[ch3]).TBCTL.bit.SYNCOSEL = 0;       /* Pass through*/		\
		 																	\
         /* Allow each timer to be sync'ed*/								\
         (*ePWM[ch1]).TBCTL.bit.PHSEN = 1;									\
         (*ePWM[ch2]).TBCTL.bit.PHSEN = 1;									\
         (*ePWM[ch3]).TBCTL.bit.PHSEN = 1;									\
         																	\
         /* Init Timer-Base Period Register for EPWM1-EPWM3*/				\
         (*ePWM[ch1]).TBPRD = prd;									\
         (*ePWM[ch2]).TBPRD = prd;									\
         (*ePWM[ch3]).TBPRD = prd;									\
																			\
         /* Init Timer-Base Phase Register for EPWM1-EPWM3*/				\
         (*ePWM[ch1]).TBPHS.half.TBPHS = 0;									\
         (*ePWM[ch2]).TBPHS.half.TBPHS = 0;									\
         (*ePWM[ch3]).TBPHS.half.TBPHS = 0;									\
																			\
         /* Init Timer-Base Control Register for EPWM1-EPWM3*/				\
         (*ePWM[ch1]).TBCTL.all = PWM_INIT_STATE;							\
		 (*ePWM[ch2]).TBCTL.all = PWM_INIT_STATE;							\
		 (*ePWM[ch3]).TBCTL.all = PWM_INIT_STATE;							\
																			\
         /* Init Compare Control Register for EPWM1-EPWM3*/					\
         (*ePWM[ch1]).CMPCTL.all = CMPCTL_INIT_STATE;						\
         (*ePWM[ch2]).CMPCTL.all = CMPCTL_INIT_STATE;						\
         (*ePWM[ch3]).CMPCTL.all = CMPCTL_INIT_STATE;						\
																			\
         /* Init Action Qualifier Output A Register for EPWM1-EPWM3*/		\
         (*ePWM[ch1]).AQCTLA.all = AQCTLA_INIT_STATE;						\
         (*ePWM[ch2]).AQCTLA.all = AQCTLA_INIT_STATE;						\
         (*ePWM[ch3]).AQCTLA.all = AQCTLA_INIT_STATE;						\
																			\
         /* Init Dead-Band Generator Control Register for EPWM1-EPWM3*/		\
         (*ePWM[ch1]).DBCTL.all = DBCTL_INIT_STATE;							\
         (*ePWM[ch2]).DBCTL.all = DBCTL_INIT_STATE;							\
         (*ePWM[ch3]).DBCTL.all = DBCTL_INIT_STATE;							\
																			\
         /* Init Dead-Band Generator for EPWM1-EPWM3*/						\
         (*ePWM[ch1]).DBFED = dbd;											\
         (*ePWM[ch1]).DBRED = dbd;											\
         (*ePWM[ch2]).DBFED = dbd;											\
         (*ePWM[ch2]).DBRED = dbd;											\
         (*ePWM[ch3]).DBFED = dbd;											\
         (*ePWM[ch3]).DBRED = dbd;											\
																			\
         /* Init PWM Chopper Control Register for EPWM1-EPWM3*/				\
         (*ePWM[ch1]).PCCTL.all = PCCTL_INIT_STATE;							\
         (*ePWM[ch2]).PCCTL.all = PCCTL_INIT_STATE;							\
         (*ePWM[ch3]).PCCTL.all = PCCTL_INIT_STATE;							\
          																	\
         EALLOW;                       /* Enable EALLOW */					\
																			\
         /* Init Trip Zone Select Register*/								\
         (*ePWM[ch1]).TZSEL.all = TZSEL_INIT_STATE;							\
         (*ePWM[ch2]).TZSEL.all = TZSEL_INIT_STATE;							\
         (*ePWM[ch3]).TZSEL.all = TZSEL_INIT_STATE;							\
																			\
         EDIS;                         /* Disable EALLOW*/			


#endif  // __F2803X_PWM_H__



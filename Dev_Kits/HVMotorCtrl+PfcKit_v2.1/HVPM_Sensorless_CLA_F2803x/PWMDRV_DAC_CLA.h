/* =================================================================================
File name:       PWMDRV_DAC_CLA.H                     
===================================================================================*/

#ifndef __PWMDRV_DAC_CLA_H__
#define __PWMDRV_DAC_CLA_H__

#define PWMDRV_DAC_CLA_MACRO(d1,d2,d3,d4,HalfPeriod)					\
	 EPwm6Regs.CMPA.half.CMPA = (d1 * HalfPeriod  )+ HalfPeriod;		\
	 EPwm6Regs.CMPB 		  = (d2 * HalfPeriod  )+ HalfPeriod; 		\
	 EPwm7Regs.CMPA.half.CMPA = (d3 * HalfPeriod  )+ HalfPeriod;		\
	 EPwm7Regs.CMPB = 			(d4 * HalfPeriod  )+ HalfPeriod; 		\

#endif  // __F2803X_PWM_H__

#ifndef __PWMDRV_3phInv_CLA_H__
#define __PWMDRV_3phInv_CLA_H__

typedef struct {   
        short PeriodMax;   	  // Parameter: PWM Half-Period in CPU clock cycles 
        short HalfPerMax;     // Parameter: Half of PeriodMax 				  
        short Deadband;       // Parameter: PWM deadband in CPU clock cycles     
        float MfuncC1;        // Input: EPWM1 A&B Duty cycle ratio 
        float MfuncC2;        // Input: EPWM2 A&B Duty cycle ratio 
        float MfuncC3;        // Input: EPWM3 A&B Duty cycle ratio
        } PWMDRV_3phInv_CLA ;    

#define PWMDRV_3phInv_CLA_MACRO(m)													\
	 EPwm1Regs.CMPA.half.CMPA = pwm1.MfuncC1*pwm1.HalfPerMax + pwm1.HalfPerMax;	\
	 EPwm2Regs.CMPA.half.CMPA = pwm1.MfuncC2*pwm1.HalfPerMax + pwm1.HalfPerMax;	\
	 EPwm3Regs.CMPA.half.CMPA = pwm1.MfuncC3*pwm1.HalfPerMax + pwm1.HalfPerMax;	\

#endif  // __F2803X_PWM_H__

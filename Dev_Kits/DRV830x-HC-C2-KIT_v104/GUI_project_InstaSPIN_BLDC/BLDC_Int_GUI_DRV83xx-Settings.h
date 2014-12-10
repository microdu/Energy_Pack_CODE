/* =================================================================================
File name:  BLDC_Int_GUI_DRV83xx-Settings.h                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Incremental Build Level control file.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 06-08-2011	Version 1.0
=================================================================================  */
#ifndef PROJ_SETTINGS_H

#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define PI 3.14159265358979

// Define the system frequency (MHz)
#if (DSP2803x_DEVICE_H==1)
#define SYSTEM_FREQUENCY 60
#elif (DSP280x_DEVICE_H==1)
#define SYSTEM_FREQUENCY 100
#elif (F2806x_DEVICE_H==1)
#define SYSTEM_FREQUENCY 80
#endif

//Define system Math Type
// Select Floating Math Type for 2806x
// Select IQ Math Type for 2803x 
#if (DSP2803x_DEVICE_H==1)
#define MATH_TYPE 0 
#elif (F2806x_DEVICE_H==1)
#define MATH_TYPE 1
#endif

// Define the ISR frequency (kHz)
//#define ISR_FREQUENCY 20
//#define PWM_FREQUENCY 20

//cutoff freq and time constant of the offset calibration LPF
#define WC_CAL	100.0
#define TC_CAL	1/WC_CAL

//cutoff freq and time constant of the speed display LPF
#define WC_SPD	1200.0
#define TC_SPD	1/WC_SPD

// This machine parameters are based on 24V PM motors inside Multi-Axis +PFC package
// Define the PMSM motor parameters
//#define RS 		0.79               		// Stator resistance (ohm)
//#define RR   	0               		// Rotor resistance (ohm) 
//#define LS   	0.0012     				// Stator inductance (H) 
//#define LR   	0						// Rotor inductance (H) 	
//#define LM   	0						// Magnetizing inductance (H)
//#define POLES   8						// Number of poles 

// Define the base quantites 
//#define BASE_VOLTAGE    66.32		    // Base peak phase voltage (volt), maximum measurable DC Bus/sqrt(3) 
//#if defined(DRV8312)
//#define BASE_CURRENT    8.6            	// Base peak phase current (amp) , maximum measurable peak current
//#endif
//#if defined(DRV8301) || defined(DRV8302)
//options for BASE_CURRENT based on DRV830x current-sense amplifier gain setting
//NOTE: DRV8302 can only be set to gain of 10 or 40
//#define BASE_CURRENT    82.5           	// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 10)
//#define BASE_CURRENT    41.25          	// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 20)
//#define BASE_CURRENT    20.625         	// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 40)
//#define BASE_CURRENT    10.3125        	// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 80)
//#endif
//#define BASE_FREQ      	200           	// Base electrical frequency (Hz)

//define motor start up parameters
//#define BEGIN_START_RPM 50.0
//#define END_START_RPM 100.0
//#define END_START_RPM 600.0

#define VOLTAGE		0			//open-loop volage mode only
#define CURRENT		1			//closed-loop current control
#define	VELOCITY	2			//closed-loop velocity control
#define	CASCADE		3			//cascaded closed-loop velocity->current control

//define starting and ending parameters for ramp generator
#define COMMUTATES_PER_E_REV 6.0
//#define RAMP_START_RATE	(PWM_FREQUENCY*1000)*60.0/BEGIN_START_RPM/COMMUTATES_PER_E_REV/(POLES/2.0)
//#define RAMP_END_RATE (PWM_FREQUENCY*1000)*60.0/END_START_RPM/COMMUTATES_PER_E_REV/(POLES/2.0)

#if defined(DRV8312)
//define phase enable/disable macros used by the PWM control module
#define PHASE_A_OFF		GpioDataRegs.GPACLEAR.bit.GPIO1 = 1
#define PHASE_A_ON		GpioDataRegs.GPASET.bit.GPIO1 = 1

#define PHASE_B_OFF		GpioDataRegs.GPACLEAR.bit.GPIO3 = 1
#define PHASE_B_ON		GpioDataRegs.GPASET.bit.GPIO3 = 1

#define PHASE_C_OFF		GpioDataRegs.GPACLEAR.bit.GPIO5 = 1
#define PHASE_C_ON		GpioDataRegs.GPASET.bit.GPIO5 = 1
#endif

#if defined(DRV8301) || defined(DRV8302)
//define phase enable/disable macros used by the PWM control module
#define PHASE_A_OFF		EPwm1Regs.TZFRC.bit.OST = 1
#define PHASE_A_ON		EPwm1Regs.TZCLR.bit.OST = 1

#define PHASE_B_OFF		EPwm2Regs.TZFRC.bit.OST = 1
#define PHASE_B_ON		EPwm2Regs.TZCLR.bit.OST = 1

#define PHASE_C_OFF		EPwm3Regs.TZFRC.bit.OST = 1
#define PHASE_C_ON		EPwm3Regs.TZCLR.bit.OST = 1
#endif

#endif 

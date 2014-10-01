/* =================================================================================
File name:  HVACI_Sensorless_PFC-Settings.H
=================================================================================  */

#ifndef PROJ_SETTINGS_H

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1
#define LEVEL2  2
#define LEVEL3  3

/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define INCR_BUILD LEVEL3

#ifndef INCR_BUILD
#error  Critical: BUILDLEVEL must be defined !!
#endif  // BUILDLEVEL
//------------------------------------------------------------------------------

// General defines
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
#endif

// PFC defines
#define VoltCurrLoopExecRatio 2
#define VBUS_FULL_RANGE (519.0) // Full range of ADC for VBUS
#define VBUS_RATED_VOLTS (398.0)
#define VBUS_MIN ((int32) ((100.0 / VBUS_FULL_RANGE) * 4096 * 4096)) // Min bus volt with AC in and PFC off
#define VBUS_OVP_THRSHLD ((int32) ((430.0 / VBUS_FULL_RANGE) * 4096 * 4096))
#define VBUS_TARGET ((int32) ((VBUS_RATED_VOLTS / VBUS_FULL_RANGE) * 4096 * 4096))
#define VBUS_ERROR_NL_CNTRL_THRSHLD ((int32) ((20.0 / VBUS_FULL_RANGE) * 4096 * 4096)) // Vbus error threshold to activate NL Vloop control
#define VBUS_MOTOR_START ((int32)((350.0 / VBUS_FULL_RANGE) * 4096 * 4096))

// Motor control defines
// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 10

// Define the electrical motor parameters (1/4 hp Marathon Motor)
#define RS 		11.05		        // Stator resistance (ohm) 
#define RR   	6.11		        // Rotor resistance (ohm) 
#define LS   	0.316423    	  	// Stator inductance (H) 
#define LR   	0.316423	  		// Rotor inductance (H) 	
#define LM   	0.293939	   		// Magnetizing inductance (H)
#define POLES  	4					// Number of poles

// Define the base quantities for PU system conversion
#define BASE_VOLTAGE    236.140     // Base peak phase voltage (volt)
#define BASE_CURRENT    10          // Base peak phase current (amp)
#define BASE_TORQUE         		// Base torque (N.m)
#define BASE_FLUX       		    // Base flux linkage (volt.sec/rad)
#define BASE_FREQ      	120         // Base electrical frequency (Hz) 
									// Note that 0.5 pu (1800 rpm) is max for Marathon motor 
									// Above 1800 rpm, field weakening is needed.
#endif


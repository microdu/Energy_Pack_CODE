/* =================================================================================
File name:  HVBLDC_Sensored-Settings.H                     

Description: Incremental Build Level control file.

=================================================================================  */
#ifndef PROJ_SETTINGS_H

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1      		// Module check out (do not connect the motors) 
#define LEVEL2  2           // Verify ADC, calibrate the offset, run the motor open loop 
#define LEVEL3  3      		// Verify closed-loop operation of BLDC, not regulated. 
#define LEVEL4  4           // Verify the closed current loop and current PI controller.
#define LEVEL5  5           // Verify the closed speed loop and speed PI controller.

/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   BUILDLEVEL LEVEL1


#ifndef BUILDLEVEL    
#error  Critical: BUILDLEVEL must be defined !!
#endif  // BUILDLEVEL
//------------------------------------------------------------------------------


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

// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 40
#define PWM_FREQUENCY 20

// Parameter Initializations 
#define ALIGN_DUTY   0x0500
#define LOOP_CNT_MAX 0

// This machine parameters are based on BLDC motor
// Define the BLDC motor parameters 

#define RS 		2.35    	           	// Stator resistance (ohm) 
#define RR   			               	// Rotor resistance (ohm) 
#define LS   	0.007	      			// Stator inductance (H) 
#define LR   			  				// Rotor inductance (H) 	
#define LM   			   				// Magnatizing inductance (H)
#define POLES  	4						// Number of poles

// Define the base quantites
#define BASE_VOLTAGE    236	            // Base peak phase voltage (volt)
#define BASE_CURRENT    10              // Base peak phase current (amp)
#define BASE_TORQUE         		    // Base torque (N.m)
#define BASE_FLUX       	       	    // Base flux linkage (volt.sec/rad)
#define BASE_FREQ      	100             // Base electrical frequency (Hz) 

#endif


/* =================================================================================
File name:  HVPM_Sensored_Servo-Settings.H
                 
Description: Incremental Build Level control file.
=================================================================================  */
#ifndef PROJ_SETTINGS_H

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1      		// Module check out (do not connect the motors) 
#define LEVEL2  2           // Verify ADC, park/clarke, calibrate the offset 
#define LEVEL3  3           // Verify closed current(torque) loop, QEP and speed meas.
#define LEVEL4  4           // Verify close speed loop and speed PID 
#define LEVEL5  5           // verify position control with QEP
#define LEVEL6  6           // verify position control with position f/b using SPI
/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   BUILDLEVEL LEVEL6


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
#define ISR_FREQUENCY 10

//// Define the electrical motor parametes (Estun Servomotor)
//#define RS 		2.35		    	    // Stator resistance (ohm)
//#define RR   			               	// Rotor resistance (ohm)
//#define LS   	0.0065					// Stator inductance (H)
//#define LR   			  				// Rotor inductance (H)
//#define LM   			   				// Magnatizing inductance (H)
//#define POLES  	8						// Number of poles
//
//// Define the base quantites
//#define BASE_VOLTAGE    236.140        // Base peak phase voltage (volt)
//#define BASE_CURRENT    20             // Base peak phase current (amp)
//#define BASE_TORQUE     		       // Base torque (N.m)
//#define BASE_FLUX       		       // Base flux linkage (volt.sec/rad)
//#define BASE_FREQ      	200            // Base electrical frequency (Hz)

// Define the electrical motor parametes (Teco Servomotor)
#define RS 		3.15		    	    // Stator resistance (ohm)
#define RR   			               	// Rotor resistance (ohm) 
#define LS   	0.005					// Stator inductance (H)
#define LR   			  				// Rotor inductance (H) 	
#define LM   			   				// Magnatizing inductance (H)
#define POLES  	8						// Number of poles

// Define the base quantites
#define BASE_VOLTAGE    120        // Base peak phase voltage (volt)
#define BASE_CURRENT    2             // Base peak phase current (amp)
#define BASE_TORQUE     		       // Base torque (N.m)
#define BASE_FLUX       		       // Base flux linkage (volt.sec/rad)
#define BASE_FREQ      	200            // Base electrical frequency (Hz) 

// QEP specs
#define QEP_PULSE_PER_CHNL         2500       // QEP's pulse per channel
#define QEP_PULSE_PER_POLEPAIR    (QEP_PULSE_PER_CHNL*4/(POLES/2))

// RESOLVER specs
#define RESOLVER_STEPS_PER_TURN         4096       // Resolver's discrete steps/turn
#define RESOLVER_STEPS_PER_POLEPAIR    (RESOLVER_STEPS_PER_TURN/(POLES/2))
#define REF_PULSE_PER_REV              10000


#endif

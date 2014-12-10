/* =================================================================================
File name:  HVPM_eSensorless-Settings.H                     
                    
Description: Incremental Build Level control file.
=================================================================================  */
#ifndef PROJ_SETTINGS_H
#define PROJ_SETTINGS_H

/*------------------------------------------------------------------------------
Following is the list of the Build Level choices.
------------------------------------------------------------------------------*/
#define LEVEL1  1      		// Module check out (do not connect the motors) 
#define LEVEL2  2           // Verify ADC, park/clarke, calibrate the offset 
#define LEVEL3  3           // Verify closed current(torque) loop and PIDs and speed measurement
#define LEVEL4  4           // Verify speed estimation and rotor position est.
#define LEVEL5  5           // Verify close speed loop and speed PID
#define LEVEL6  6           // Verify close speed loop sensorless
#define LEVEL7  7           // Verify close speed loop sensorless - eSMO; contact TI sales for eSMO source code
#define LEVEL8  8           // Verify IPD + HFI; contact TI sales for HFI/IPD library
#define LEVEL9  9           // Verify close speed loop sensorless - IPD + HFI + eSMO; contact TI sales for HFI/IPD library

/*------------------------------------------------------------------------------
This line sets the BUILDLEVEL to one of the available choices.
------------------------------------------------------------------------------*/
#define   BUILDLEVEL LEVEL7


#ifndef BUILDLEVEL    
#error  Critical: BUILDLEVEL must be defined !!
#endif  // BUILDLEVEL

#if (BUILDLEVEL  == LEVEL8) || (BUILDLEVEL == LEVEL9)
#error  *** Cannot compile, contact TI sales for HFI/IPD library !! ***
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

// Define the offset filter corner frequency (in Hz)
#define  OFFSET_FILTER_CORNER_FREQ    20    /*  in Hz  */


// Define the electrical motor parametes (Estun Servomotor)
#define RS 		2.35		    	    // Stator resistance (ohm) 
#define RR   			               	// Rotor resistance (ohm) 
#define LS   	0.0065					// Stator inductance (H) 
#define LR   			  				// Rotor inductance (H) 	
#define LM   			   				// Magnatizing inductance (H)
#define POLES  	8						// Number of poles

// Define the base quantites
#define BASE_VOLTAGE    236.14        // Base peak phase voltage (volt), Vdc/sqrt(3)
#define BASE_CURRENT    10            // Base peak phase current (amp), Max. measurable peak curr.
#define BASE_TORQUE     		      // Base torque (N.m)
#define BASE_FLUX       			  // Base flux linkage (volt.sec/rad)
#define BASE_FREQ      	200           // Base electrical frequency (Hz) 

#endif


/* ==============================================================================
System Name:  	BLDC_Int

File Name:		BLDC_Int.h

Description:	Primary system header file for the Real Implementation of Sensorless  
          		Trapezoidal Control of Brushless DC Motors (BLDC) using BEMF Integration

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DRV8312-EVM. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 06-08-2011	Version 1.0
=================================================================================  */

/*-------------------------------------------------------------------------------
Next, Include project specific include files.
-------------------------------------------------------------------------------*/

#include "pid_grando.h"       	// Include header for the PID_GRANDO_CONTROLLER object 
#include "rmp3cntl.h"       	// Include header for the RMP3 object
#include "impulse.h"       		// Include header for the IMPULSE object
#include "mod6_cnt_dir.h"      	// Include header for the MOD6CNTDIR object
#include "InstaSPIN_BLDC_Lib.h"	// Include header for the InstaSPIN library
#include "speed_pr.h"           // Include header for the SPEED_MEAS_REV object
#include "rmp_cntl.h"       	// Include header for the RMPCNTL object*/  

#include "f2803xidc_vemf.h"	// Include header for the ILEG2DCBUSMEAS object 
#if defined(DRV8312)
#include "f2803xpwm_cntl.h"     // Include header for the PWMGEN object
#include "f2803xpwmdac_BLDC.h"    	// Include header for the PWMGEN object
#endif
#if defined(DRV8301) || defined(DRV8302)
#include "f2803xpwmdac_BLDC_8301.h"    	// Include header for the PWMGEN object
#include "f2803xpwm_cntl_8301.h"     // Include header for the PWMGEN object
#endif
#include "DSP2803x_EPwm_defines.h"	// Include header for PWM defines 

#include "dlog4ch-BLDC_Int.h" // Include header for the DLOG_4CH object

//SPI only used for DRV8301
#ifdef DRV8301
#include "DRV8301_SPI.h"
#endif

//===========================================================================
// No more.
//===========================================================================

/* ==============================================================================
System Name:  	HVBLDC_Sensored

File Name:		HVBLDC_Sensored.h

Description:	Primary system header file for the Trapezoidal BLDC Control 
				Using Hall Effect Sensors            		
=================================================================================  */



/*-------------------------------------------------------------------------------
Next, Include project specific include files.
-------------------------------------------------------------------------------*/

//#include "com_trig.h"       	// Include header for the CMTN object
#include "pi.h"       	// Include header for the PIDREG3 object
#include "rmp2cntl.h"       	// Include header for the RMP2 object
#include "rmp3cntl.h"       	// Include header for the RMP3 object
#include "impulse.h"       		// Include header for the IMPULSE object
#include "mod6_cnt.h"       	// Include header for the MOD6CNT object
#include "speed_pr.h"           // Include header for the SPEED_MEAS_REV object
#include "rmp_cntl.h"       	// Include header for the RMPCNTL object  

#if (DSP2803x_DEVICE_H==1)
#include "f2803xileg_vdc.h" 	// Include header for the ILEG2DCBUSMEAS object
#include "f2803xbldcpwm.h"      // Include header for the PWMGEN object
#include "f2803xpwmdac.h"       // Include header for the PWMDAC object
#include "f2803xhall_gpio.h"    // Include header for the HALL object
#include "DSP2803x_EPwm_defines.h" // Include header for PWM defines 
#endif 

#include "dlog4ch-HVBLDC_Sensored.h"	// Include header for the DLOG_4CH object

//===========================================================================
// No more.
//===========================================================================

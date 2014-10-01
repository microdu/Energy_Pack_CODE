/* ==============================================================================
System Name:  	HVACI_Scalar

File Name:		HVACI_Scalar

Description:	Primary system header file for the Real Implementation of Scalar  
          		Field Orientation Control for Induction Motor
=================================================================================  */



/*-------------------------------------------------------------------------------
Next, Include project specific include files.
-------------------------------------------------------------------------------*/

#include "pi.h"       			// Include header for the PIDREG3 object 
#include "rmp_cntl.h"       	// Include header for the RMPCNTL object 
#include "speed_pr.h"			// Include header for the SPEED_MEAS_CAP object 
#include "speed_fr.h"			// Include header for the SPEED_MEAS_QEP object
#include "svgen_mf.h"			// Include header for SVGEN_MF object
#include "vhzprof.h"			// Include header for VHZ_PROF object

#if (DSP2803x_DEVICE_H==1)
#include "f2803xileg_vdc.h" 	// Include header for the ILEG2DCBUSMEAS object 
#include "f2803xpwm.h"        	// Include header for the PWMGEN object
#include "f2803xpwmdac.h"       // Include header for the PWMGEN object
#include "f2803xqep.h"        	// Include header for the QEP object
#include "f2803xcap.h"        	// Include header for the CAP object
#include "DSP2803x_EPwm_defines.h" // Include header for PWM defines  
#endif

#include "dlog4ch-HVACI_Scalar.h"			// Include header for the DLOG_4CH object

//===========================================================================
// No more.
//===========================================================================

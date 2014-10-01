//=================================================================================
//	DacDrvCnf(int16 n, int16 DACval)    configuration function for Comparator/DAC
//=================================================================================
//	FILE:			DacDrvCnf.c
//
//	Description:	Config to support a Comparator/DAC
//
//	Version: 		1.00
//  Target:  		TMS320F280x 
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2009
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description
//----------------------------------------------------------------------------------
//  11/04/09  | Release 1.0  		New release.
//----------------------------------------------------------------------------------
// Description:
// ------------
// Comparator/DAC configuration func
//
// The function call is:
//
// DacDrvCnf(int16 n, int16 DACval)
//
// Function arguments defined as:
//-------------------------------
// n = 		Target Comparator module, 1,2,...16.  e.g. if n=2, then target is ePWM2
// DACval = Set DAC output - Input is Q15
//
//==================================================================================

#include "DSP2802x_Device.h"
#include "DSP2802x_Comp.h"     	// DSP280x Headerfile Include File


extern volatile struct COMP_REGS *Comp[];

void DacDrvCnf(int16 n, int16 DACval)
{
   EALLOW;

   	(*Comp[n]).COMPCTL.bit.COMPDACEN = 1;       // Power up Comparator 1 locally
//	(*Comp[n]).COMPCTL.bit.COMPSOURCE = 1;      // Connect the inverting input to pin COMP2B
	(*Comp[n]).COMPCTL.bit.COMPSOURCE = 0;      // Connect the inverting input to Internal DAC
	(*Comp[n]).DACVAL.bit.DACVAL = (DACval>>5);	// Set DAC output - Input is Q15 - Convert to Q10

	EDIS;
}


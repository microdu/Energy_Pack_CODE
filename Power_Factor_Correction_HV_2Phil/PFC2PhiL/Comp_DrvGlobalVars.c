//----------------------------------------------------------------------------------
//	FILE:			Comp_DrvGlobalVars.C
//
//	Description:	Contains shared global variables for the Comparator drivers
//
//	Version: 		1.00
//
//  Target:  		TMS320F280x 
//
//	Type: 			Device dependent
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2009                                				
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description
//----------------------------------------------------------------------------------
//  11/04/09  | Release 1.0  		New release.
//----------------------------------------------------------------------------------

#include "DSP2802x_Device.h"
#include "DSP2802x_Comp.h"     	// DSP280x Headerfile Include File

// 2802x
volatile struct COMP_REGS *Comp[] = 
 				  { &Comp1Regs,
					&Comp1Regs,				  
					&Comp2Regs};

/*volatile struct COMP_REGS *Comp[] = 
 				  { &Comp1Regs,
				   	&Comp1Regs};
					//&Comp2Regs,				  
					//&Comp3Regs,*/


//----------------------------------------------------------------------------------
//	FILE:			Interleaved PFC-Settings.h
//
//	Description:    This file contains the definitions for this project, and is 
//					linked to both Interleaved PFC-Main.c and Interleaved PFC-DPL-ISR.asm 
//					(where X is the project name).  
//
//	Type: 			Device Independent
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2010
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 9 April 2010 - MB
//----------------------------------------------------------------------------------

#ifndef _PROJSETTINGS_H
#define _PROJSETTINGS_H

//**********************************************************************************
//  NOTE: WHEN CHANGING THIS FILE PLEASE REBUILD ALL
//**********************************************************************************

//==================================================================================
// Incremental Build options for System check-out
//==================================================================================

//#define INCR_BUILD 1
//#define INCR_BUILD 2
#define INCR_BUILD 3

#define VoltCurrLoopExecRatio	2
//==================================================================================
// System Settings
//----------------------------------------------------------------------------------
//Add any system specific setting below
#define HistorySize 8
//#define DLOG_SIZE   200



//==================================================================================
// Interrupt Framework options
//==================================================================================

#define EPWMn_DPL_ISR	1	// for EPWM triggered ISR set as 1
#define ADC_DPL_ISR	    0	// for ADC triggered ISR set as 1 

//----------------------------------------------------------------------------------
// If EPWMn_DPL_ISR = 1, then choose which module
//----------------------------------------------------------------------------------
#define EPWM1			0	// EPWM1 provides ISR trigger
#define EPWM2			1 	// EPWM2 provides ISR trigger

#define VIN_FULL_RANGE (405.2)//(409.8) //full range of ADC for Vin (each of line and neutral sense)

#define VBUS_FULL_RANGE 	(533.00)//(530.45)//(519.0)//full range of ADC for VBUS
#define VBUS_RATED_VOLTS	(390.0)
#define VBUS_MIN ((int32)((160.0/VBUS_FULL_RANGE)*4095*4095)) //Min bus volt with AC in and PFC off

#define VBUS_OVP_THRSHLD ((int32)((435.0/VBUS_FULL_RANGE)*4095*4095)) //435V

#define VBUS_TARGET			((int32)((VBUS_RATED_VOLTS/VBUS_FULL_RANGE)*4095*4095)) //395V
#define VBUS_ERROR_NL_CNTRL_THRSHLD ((int32)((15.0/VBUS_FULL_RANGE)*4095*4095)) //Vbus error threshold to activate NL Vloop control

// Power metering parameters
#define V_DIODE			_IQ15(0.7/VIN_FULL_RANGE)
#define I_GAIN			_IQ15(0.995)
#define I_OFFSET		_IQ15(0.01/19.8)
#define R_SHUNT			_IQ15((19.8*0.05/3.0)/(10.0*VIN_FULL_RANGE))//Rs = 0.05/3, Imax = 19.8A, CS OPAMP Gain = 10.0

#endif //_PROJSETTINGS_H


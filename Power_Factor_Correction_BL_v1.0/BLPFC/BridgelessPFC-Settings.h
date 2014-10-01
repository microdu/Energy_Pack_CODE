//----------------------------------------------------------------------------------
//	FILE:			BridgelessPFC-Settings.h
//
//	Description:    This file contains the definitions for this project, and is 
//					linked to both BridgelessPFC-Main.c and BridgelessPFC-DPL-ISR.asm 
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
// BUILD 1 	Open Loop PWMDRV_1ch using PWM 1 on CLA, PWMDRV_CLA_1ch using PWM2 on C28x

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
#define EPWM1			1	// EPWM1 provides ISR trigger
#define EPWM2			0 	// EPWM2 provides ISR trigger

#define VIN_FULL_RANGE (409.8) //full range of ADC for Vin (each of line and neutral sense)

#define VBUS_FULL_RANGE 	(519.0)//(522.5)//(503.0) //full range of ADC for VBUS
#define VBUS_RATED_VOLTS	(400.0)//(395.0) //395.0V
#define VBUS_MIN ((int32)((100.0/VBUS_FULL_RANGE)*4096*4096)) //Min bus volt with AC in and PFC off
#define VBUS_OVP_THRSHLD ((int32)((440.0/VBUS_FULL_RANGE)*4096*4096)) //435V
#define VBUS_DPWM_OFF_LEVEL ((int32)((425.0/VBUS_FULL_RANGE)*4096*4096)) //425V
#define VBUS_DPWM_ON_LEVEL  ((int32)((395.0/VBUS_FULL_RANGE)*4096*4096)) //395V
#define VBUS_TARGET			((int32)((VBUS_RATED_VOLTS/VBUS_FULL_RANGE)*4096*4096)) //395V
#define VBUS_ERROR_NL_CNTRL_THRSHLD ((int32)((10.0/VBUS_FULL_RANGE)*4096*4096)) //Vbus error threshold to activate NL Vloop control


#endif //_PROJSETTINGS_H


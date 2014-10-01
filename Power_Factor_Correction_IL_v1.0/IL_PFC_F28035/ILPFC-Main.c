//----------------------------------------------------------------------------------
//	FILE:			ILPFC-Main.C
//
//	Description:	2-Ph Interleaved PFC
//					The file drives duty on PWM1A and PWM1B using C28x
//					C28x ISR is triggered by the PWM 2 interrupt
//
//	Version: 		1.0
//
//  Target:  		TMS320F2803x(PiccoloB), 
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments 02-15-2013
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// Feb 13 , 2013  - File created (SC)
//----------------------------------------------------------------------------------
//
// PLEASE READ - Useful notes about this Project

// Although this project is made up of several files, the most important ones are:
//	 "ILPFC-Main.C"	- this file
//		- Application Initialization, Peripheral config,
//		- Application management
//		- Slower background code loops and Task scheduling
//	 "ILPFC-DevInit_F28xxx.C
//		- Device Initialization, e.g. Clock, PLL, WD, GPIO mapping
//		- Peripheral clock enables
//		- DevInit file will differ per each F28xxx device series, e.g. F280x, F2833x,
//	 "ILPFC-DPL-ISR.asm
//		- Assembly level library Macros and any cycle critical functions are found here
//	 "ILPFC-Settings.h"
//		- Global defines (settings) project selections are found here
//		- This file is referenced by both C and ASM files.
//
// Code is made up of sections, e.g. "FUNCTION PROTOTYPES", "VARIABLE DECLARATIONS" ,..etc
//	each section has FRAMEWORK and USER areas.
//  FRAMEWORK areas provide useful ready made "infrastructure" code which for the most part
//	does not need modification, e.g. Task scheduling, ISR call, GUI interface support,...etc
//  USER areas have functional example code which can be modified by USER to fit their appl.
//
// Code can be compiled with various build options (Incremental Builds IBx), these
//  options are selected in file "ILPFC-Settings.h".  Note: "Rebuild All" compile
//  tool bar button must be used if this file is modified.
//----------------------------------------------------------------------------------
#include "PeripheralHeaderIncludes.h"
#include "DSP2803x_EPWM_defines.h"		
#include "ILPFC-Settings.h"
#include "IQmathLib.h"

#include "SineAnalyzer2.h"
		
#include "DPlib.h"	

#include <math.h>

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTION PROTOTYPES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Add protoypes of functions being used in the project here 
void DeviceInit(void);
void SCIA_Init();
void SerialHostComms();
#ifdef FLASH		
	void InitFlash();
#endif
void MemCopy();
//-------------------------------- DPLIB --------------------------------------------
void PWM_2ch_UpDwnCnt_CNF(int16 n, int16 period, int16 mode, int16 phase);
void PWM_1ch_UpDwnCnt_CNF(int16 n, int16 period, int16 mode, int16 phase);
void ADC_SOC_CNF(int ChSel[], int TrigSel[], int ACQPS[], int IntChSel, int mode);

interrupt void SECONDARY_ISR(void);
// -------------------------------- FRAMEWORK --------------------------------------
// State Machine function prototypes
//----------------------------------------------------------------------------------
// Alpha states
void A0(void);	//state A0
void B0(void);	//state B0
void C0(void);	//state C0

// A branch states
void A1(void);	//state A1
void A2(void);	//state A2

// B branch states
void B1(void);	//state B1
void B2(void);	//state B2

// C branch states
void C1(void);	//state C1
void C2(void);	//state C2
void C3(void);	//state C3
void C4(void);	//state C4

// Variable declarations
void (*Alpha_State_Ptr)(void);	// Base States pointer
void (*A_Task_Ptr)(void);		// State pointer A branch
void (*B_Task_Ptr)(void);		// State pointer B branch
void (*C_Task_Ptr)(void);		// State pointer C branch


//Solar_SineAnalyzer sine_mainsV = Solar_SineAnalyzer_DEFAULTS;
SineAnalyzer sine_mainsV = SineAnalyzer_DEFAULTS;

//----------------------------------------------------------------------------------

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - GENERAL
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -------------------------------- FRAMEWORK --------------------------------------
int16 VTimer0[4];	// Virtual Timers slaved off CPU Timer 0
int16 VTimer1[4];	// Virtual Timers slaved off CPU Timer 1
int16 VTimer2[4];	// Virtual Timers slaved off CPU Timer 2

int16	SerialCommsTimer;
int16	CommsOKflg;

// Used for running BackGround in flash, and ISR in RAM
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;

extern void DSP28x_usDelay(Uint32 Count);
extern void InitAdc(void);
extern void AdcOffsetSelfCal(void);

// Used for ADC Configuration 
int ChSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int TrigSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int ACQPS[16] = {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6};//For 8x PFC current oversampling

// Used to indirectly access all EPWM modules
volatile struct EPWM_REGS *ePWM[] = 
 				  { &EPwm1Regs,			//intentional: (ePWM[0] not used)
				  	&EPwm1Regs,
					&EPwm2Regs,
					&EPwm3Regs,
					&EPwm4Regs,
					#if (!DSP2802x_DEVICE_H)
					&EPwm5Regs,
					&EPwm6Regs,
					#if (DSP2803x_DEVICE_H || DSP2804x_DEVICE_H)
					&EPwm7Regs,
					#if (DSP2804x_DEVICE_H)
					&EPwm8Regs
					#endif 
					#endif 
					#endif
				  };

// Used to indirectly access all Comparator modules
volatile struct COMP_REGS *Comp[] = 
 				  { &Comp1Regs,			//intentional: (Comp[0] not used)
					&Comp1Regs,				  
					&Comp2Regs,
					#if (DSP2803x_DEVICE_H)
					&Comp3Regs
					#endif 
				  };
// ---------------------------------- USER -----------------------------------------

long Dmax_I;


// ---------------------------- DPLIB Net Pointers ---------------------------------
// Declare net pointers that are used to connect the DP Lib Macros  here 

// ADC Driver Macro. Ipfc is oversampled 8 times.
extern volatile long *ADCDRV_1ch_Rlt1;	// instance #1, Ipfc
extern volatile long *ADCDRV_1ch_Rlt2;	// instance #2, Ipfc
extern volatile long *ADCDRV_1ch_Rlt3;	// instance #3, Ipfc
extern volatile long *ADCDRV_1ch_Rlt4;	// instance #4, Ipfc
extern volatile long *ADCDRV_1ch_Rlt5;	// instance #5, Ipfc 
extern volatile long *ADCDRV_1ch_Rlt6;	// instance #6, Ipfc
extern volatile long *ADCDRV_1ch_Rlt7;	// instance #7, Ipfc 
extern volatile long *ADCDRV_1ch_Rlt8;	// instance #4, Ipfc
extern volatile long *ADCDRV_1ch_Rlt9;	// instance #5, Vbus 
extern volatile long *ADCDRV_1ch_Rlt10;	// instance #6, VL_fb
extern volatile long *ADCDRV_1ch_Rlt11;	// instance #7, VN_fb 

// CONTROL_2P2Z
extern volatile long *CNTL_2P2Z_Ref1;	// instance #1
extern volatile long *CNTL_2P2Z_Out1;	// instance #1
extern volatile long *CNTL_2P2Z_Fdbk1;	// instance #1
extern volatile long *CNTL_2P2Z_Coef1; 	// instance #1
extern volatile long CNTL_2P2Z_DBUFF1[5];

// CONTROL_2P2Z
extern volatile long *CNTL_2P2Z_Ref2;	// instance #2
extern volatile long *CNTL_2P2Z_Out2;	// instance #2
extern volatile long *CNTL_2P2Z_Fdbk2;	// instance #2
extern volatile long *CNTL_2P2Z_Coef2; 	// instance #2
extern volatile long CNTL_2P2Z_DBUFF2[5];

extern volatile long *PWMDRV_2ch_UpDwnCnt_Duty1;	// instance #1, EPWM1

extern volatile long *PFC_ICMD_Vcmd1;
extern volatile long *PFC_ICMD_VinvSqr1;
extern volatile long *PFC_ICMD_VacRect1;
extern volatile long *PFC_ICMD_Out1;
extern volatile long PFC_ICMD_VmaxOverVmin1;

//MATH_AVG - instance #1
extern volatile long *MATH_EMAVG_In1;
extern volatile long *MATH_EMAVG_Out1;
extern volatile long MATH_EMAVG_Multiplier1;

//MATH_AVG - instance #2
extern volatile long *MATH_EMAVG_In2;
extern volatile long *MATH_EMAVG_Out2;
extern volatile long MATH_EMAVG_Multiplier2;

//PFC_InvRmsSqr - instance #1
extern volatile long *PFC_InvRmsSqr_In1;
extern volatile long *PFC_InvRmsSqr_Out1;
extern volatile long PFC_InvRmsSqr_VminOverVmax1;
extern volatile long PFC_InvRmsSqr_Vmin1;


// ---------------------------- DPLIB Variables ---------------------------------
// Declare the net variables being used by the DP Lib Macro here 
volatile long Ipfc,Ipfc1,Ipfc2,Ipfc3,Ipfc4,Ipfc5,Ipfc6,Ipfc7,Ipfc8;
volatile long Ipfc_fltr;
volatile long Vbus; 
volatile long VL_fb; 
volatile long VN_fb; 

volatile long DutyA; 
volatile long VrectAvg; 
volatile long Km, Vrect, VinvSqr, VbusAvg, VrectRMS, IrectRMS, PinRMS, Freq_Vin;
volatile long VbusVcmd,PFCIcmd;
volatile long Duty4A;
volatile long VbusTarget, error_v;		// Set point for the PFC voltage loop

volatile long VbusTargetSlewed;		// Slewed set point for the voltage loop
volatile long VbusSlewRate;	// Voltage loop Slew rate adjustment (Q24)
volatile long pfc_slew_temp;		// Temp variable: used only if implementing 
										// slew rate control in the slower state machine
long	temp_zero;

int 	init_boost;		// Small boost command when PFC is enabled the first time
int16 	start_flag, pfc_on_flag, Soft_Start_Phase, OV_flag, flag_NL_Vloop;//Set NL Vloop flag for NL Vloop Control

#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct1, "CNTL_2P2Z_Coef"); 
#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct2, "CNTL_2P2Z_Coef"); 
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct1;
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct2;

long Pgain_V,Igain_V,Dgain_V,Dmax_V;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - CCS WatchWindow / GUI support
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -------------------------------- FRAMEWORK --------------------------------------

//GUI support variables
// sets a limit on the amount of external GUI controls - increase as necessary
int16 *varSetTxtList[16];	//16 textbox controlled variables
int16 *varSetBtnList[16];	//16 button controlled variables
int16 *varSetSldrList[16];	//16 slider controlled variables
int16 *varGetList[16];		//16 variables sendable to GUI
int16 *arrayGetList[16];	//16 arrays sendable to GUI	
int16 LedBlinkCnt;

// ---------------------------------- USER -----------------------------------------

// Monitor ("Get")	// Display as:
int16 Gui_Vbus;	// Q06
int16 Gui_VrectAvg;	// Q06
int16 Gui_VrectRMS;	// Q06
int16 Gui_Freq_Vin;	// Q06
int16 Gui_IrectRMS;	// Q10
int16 Gui_PinRMS_16;	// Q5
int32 Gui_PinRMS;	// Q5

// Power metering parameters
/*volatile long Vdiode;
volatile long Igain;
volatile long Ioffset;
volatile long Rshunt;*/


// History arrays are used for Running Average calculation (boxcar filter)
// Used for CCS display and GUI only, not part of control loop processing
//Uint16 Hist_Ipfc[HistorySize];
Uint16 Hist_Vbus[HistorySize];
Uint16 Hist_VrectAvg[HistorySize];
Uint16 Hist_VrectRMS[HistorySize];
Uint16 Hist_IrectRMS[HistorySize];

Uint16 Hist_Freq_Vin[HistorySize];

//Scaling Constants (exact value calibrated per board)
Uint16 K_Ipfc;	// 
Uint16 K_Vbus;	// 
Uint16 K_VrectAvg;	//
Uint16 K_Vrms;	//
Uint16 K_Irms;	//

long K_Prms;
long slope_Pcorr;
long offset_Pcorr;

Uint16 K_Freq_Vin;	//


int16	disable_auto_cloop_coeff_change;

// Variables for background support only (no need to access)
int16 i,k;						// common use incrementer
Uint32 HistPtr, HistPtr2, temp_Scratch; 	// Temp here means Temporary


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN CODE - starts here
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void main(void)
{
//=================================================================================
//	INITIALISATION - General
//=================================================================================

	// The DeviceInit() configures the clocks and pin mux registers 
	// The function is declared in BridgelessPFC-DevInit_F2803/2x.c,
	// Please ensure/edit that all the desired components pin muxes 
	// are configured properly that clocks for the peripherals used
	// are enabled, for example the individual PWM clock must be enabled 
	// along with the Time Base Clock 

	DeviceInit();	// Device Life support & GPIO

//-------------------------------- FRAMEWORK --------------------------------------

// Only used if running from FLASH
// Note that the variable FLASH is defined by the compiler with -d FLASH

#ifdef FLASH		
// Copy time critical code and Flash setup code to RAM
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the linker files. 
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
	InitFlash();	// Call the flash wrapper init function
#endif //(FLASH)

#ifdef FLASH
	SCIA_Init();  	// Initalize the Serial Comms A peripheral - Uncomment for FLASH config
#endif //(FLASH)

// Timing sync for background loops
// Timer period definitions found in PeripheralHeaderIncludes.h
	CpuTimer0Regs.PRD.all =  mSec1;		// A tasks
	CpuTimer1Regs.PRD.all =  mSec50;	// B tasks
	CpuTimer2Regs.PRD.all =  mSec10;	// C tasks

// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;
	C_Task_Ptr = &C1;

	VTimer0[0] = 0;	
	VTimer1[0] = 0;
	VTimer2[0] = 0;
	LedBlinkCnt = 5;
	
#ifdef	FLASH
	CommsOKflg = 0;
	SerialCommsTimer = 0;
#endif
	
	
// ---------------------------------- USER -----------------------------------------
//  put common initialization/variable definitions here

	//K_Vbus=29783;//15-bit GUI variable is denoted in Q6 since max Vbus is 465.38 which requires 9 integer bits;
	//K_Vbus=(465.38/511)*32767 = (465.38/511)*32767 = 29783
	//K_Vbus=33439;//K_Vbus=(522.5/511)*32767 = 33439
	//K_Vbus=33215;//K_Vbus=(519/511)*32767 = 33215; Voltage will never gets to 512V. So this scaling is fine
	//K_Vbus=33948;//K_Vbus=(530.45/511)*32767 = 33948; Voltage will never gets to 512V. So this scaling is fine
				////DC Bus voltage is viewed as a Q6 number	
	//K_Vbus=33729;//K_Vbus=(526/511)*32767 = 33729;
	K_Vbus=34184;//K_Vbus=(533/511)*32767 = 34184;
	
	//K_Vrms=26226;	//K_Vrms=(409.8/511)*32767 = 26226, //RMS voltage is viewed as a Q6 number
	K_Vrms=25983;	//K_Vrms=(405.2/511)*32767 = 25983, //RMS voltage is viewed as a Q6 number
	
	K_Irms=20275;	//K_Irms=(19.8/31)*32767 = 20275//Current is viewed as a Q10 number
	
	//K_Prms=259641.36;	//K_Prms=(19.8*409.8/1023)*32767 = 259641.36//Power is viewed as a Q5 number
	
	K_Prms=256977.84;	//K_Prms=(19.8*405.2/1023)*32767 = 256726.9//Power is viewed as a Q5 number

	//*********************Pin calibration parameters***********
	slope_Pcorr= 1.03*256977.84;//Slope of 1.03 calculated using two points from the raw Pcal data
	offset_Pcorr= 7.6694*31;//Negative Offset of 7.6694 calculated using two points from the raw Pcal data
	//***********These calibration parameters for Pin monitoring have been calculated at 110V input supplied from 
	//C2000 lab AC source Chroma model 61502. ILPFC EVM aux power supplied from the external +12V supply. 
	//The on-board aux output was not used during this test. Also inrush current limit thermister was shorted.***********//
	
	OV_flag = 0;
	
	
//===============================================================================
//	INITIALISATION - GUI connections
//=================================================================================
// Use this section only if you plan to "Instrument" your application using the 
// Microsoft C# freeware GUI Template provided by TI

	//"Set" variables
	//---------------------------------------
	// assign GUI variable Textboxes to desired "setable" parameter addresses
	//varSetTxtList[0] = &Gui_Vbus_set;

	//varSetTxtList[1] = &b0_Gui;				// Q15
	//varSetTxtList[2] = &b1_Gui;				// Q15
	//varSetTxtList[3] = &b2_Gui;				// Q15
	//varSetTxtList[4] = &a0_Gui;				// Q15
	//varSetTxtList[5] = &a1_Gui;				// Q15
	//varSetTxtList[6] = &a2_Gui;				// Q15 
	//varSetTxtList[7] = &coeff_change;		// Q0
	//varSetTxtList[8] = &vloop_coeff_change; // Q0
	
/*
	// assign GUI Buttons to desired flag addresses
	varSetBtnList[0] = &Gui_BtnListVar;
    varSetBtnList[1] = &Gui_BtnListVar;
	varSetBtnList[2] = &Gui_BtnListVar;
	varSetBtnList[3] = &Gui_BtnListVar;
	varSetBtnList[4] = &Gui_BtnListVar;
*/

	//varSetBtnList[0] = &pid2p2z_Gui;

	// assign GUI Sliders to desired "setable" parameter addresses
/*	varSetSldrList[0] = &Gui_Pgain_I;
	varSetSldrList[1] = &Gui_Igain_I;
	varSetSldrList[2] = &Gui_Dgain_I;
	varSetSldrList[3] = &Gui_Pgain_V;
	varSetSldrList[4] = &Gui_Igain_V;
	varSetSldrList[5] = &Gui_Dgain_V;*/
	
	//"Get" variables
	//---------------------------------------
	// assign a GUI "getable" parameter address
	varGetList[0] = &Gui_Vbus;
	varGetList[1] = &Gui_VrectRMS;
	varGetList[2] = &Gui_IrectRMS;
	varGetList[3] = &Gui_Freq_Vin;
	varGetList[4] = &Gui_PinRMS_16;
	varGetList[5] = &pfc_on_flag;

/*	
	varGetList[1] = &Gui_GetListVar;
	varGetList[2] = &Gui_GetListVar;
	varGetList[3] = &Gui_GetListVar;
	varGetList[4] = &Gui_GetListVar;
	varGetList[5] = &Gui_GetListVar;
	varGetList[6] = &Gui_GetListVar;
	varGetList[7] = &Gui_GetListVar;
	varGetList[8] = &Gui_GetListVar;
	varGetList[9] = &Gui_GetListVar;
	varGetList[10] = &Gui_GetListVar;
	varGetList[11] = &Gui_GetListVar;
	varGetList[12] = &Gui_GetListVar;
	varGetList[13] = &Gui_GetListVar;
	varGetList[14] = &Gui_GetListVar;
	varGetList[15] = &Gui_GetListVar;

	// assign a GUI "getable" parameter array address
	arrayGetList[0] = &DBUFF1;  	//only need to set initial position of array,
	arrayGetList[1] = &DBUFF2;		//  program will run through it accordingly
	arrayGetList[2] = &DBUFF3;
	arrayGetList[3] = &DBUFF4;
*/

//==================================================================================
//	INCREMENTAL BUILD OPTIONS - NOTE: selected via {ILPFC-Settings.h
//==================================================================================
// ---------------------------------- USER -----------------------------------------

#define period 300	//300 cycles -> 200KHz @60MHz CPU
#define period_instr_pwm 120	//512 cycles -> 117k @60MHz CPU

// Configure PWM1A/1B for 200Khz switching Frequency. Used for PFC PWM on IL PFC EVM
	PWM_2ch_UpDwnCnt_CNF(1, period, 1, 0);
	
	// Configure PWM2 for 200Khz switching Frequency. Used for ISR generation using CMPB 
	PWM_2ch_UpDwnCnt_CNF(2, period, 0, 0); 

    EALLOW;
  SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;  // ePWM3
// Setup TBCLK
  EPwm3Regs.TBPRD = period;           		 // Set timer period 300 TBCLKs -- 100kHz for PFC current over sampling trigger
  EPwm3Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm3Regs.TBCTR = 0x0000;                      // Clear counter
// Setup counter mode
  EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;
  EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

  SysCtrlRegs.PCLKCR1.bit.EPWM4ENCLK = 1;  // ePWM4
// Setup TBCLK
  EPwm4Regs.TBPRD = period;           		 // Set timer period 300 TBCLKs -- 100kHz for PFC current over sampling trigger
  EPwm4Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
  EPwm4Regs.TBCTR = 0x0000;                      // Clear counter
// Setup counter mode
  EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
  EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
  EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
  EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;
  EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;

  EDIS;

      EALLOW;
   SysCtrlRegs.PCLKCR1.bit.EPWM7ENCLK = 1;  // ePWM7
// Setup TBCLK
   EPwm7Regs.TBPRD = 3000;           // Set timer period 3000 TBCLKs
   EPwm7Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm7Regs.TBCTR = 0x0000;                      // Clear counter
// Setup counter mode
   EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
   EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
   EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
   EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;
   EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
// Configure the Start of Conversion for the ADC.
   EPwm7Regs.ETSEL.bit.INTEN = 1;               // 
   EPwm7Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;   // Select Int from counter = PRD
   EPwm7Regs.ETPS.bit.INTPRD = ET_1ST;        // Generate pulse on 1st event
    
      PieVectTable.EPWM7_INT  = &SECONDARY_ISR;     
      IER   |= M_INT3;                                // Enable CPU INT3 for capture interrupt   
      PieCtrlRegs.PIEIER3.bit.INTx7       = 1;  // Enable EPWM7 int in PIE group 3
      EDIS;

//sine analyzer initialization
      sine_mainsV.Vin=0;
      sine_mainsV.Iin=0;
      sine_mainsV.SampleFreq=_IQ15(10000.0);
      sine_mainsV.Threshold=_IQ15(0.1);//(0.015);//(0.02);
// End sine analyzer initialization

// Power metering parameters initialization
/*      Vdiode = V_DIODE;
      Igain = I_GAIN;
      Ioffset = I_OFFSET;
      Rshunt = R_SHUNT;*/

#define		IpfcR	AdcResult.ADCRESULT1		//Q12
#define		VbusR	AdcResult.ADCRESULT9		//Q12
#define		VL_fbR	AdcResult.ADCRESULT10		//Q12
#define		VN_fbR	AdcResult.ADCRESULT11		//Q12
	
	//Oversampling of current, 8x OVS, 
	//New ILPFC board
	ChSel[0] = 4;		// Dummy read for first 
	ChSel[1] = 4;		// A4 - IpfcA
	ChSel[2] = 4;		// A4 - IpfcA
	ChSel[3] = 4;		// A4 - IpfcA
	ChSel[4] = 4;		// A4 - IpfcA
	ChSel[5] = 4;		// A4 - IpfcA
	ChSel[6] = 4;		// A4 - IpfcA
	ChSel[7] = 4;		// A4 - IpfcA
	ChSel[8] = 4;		// A4 - IpfcA
	ChSel[9] = 2;		// A2 - Vbus
	ChSel[10] = 10;		// B2 - VL_fb
	ChSel[11] = 8;		// B0 - VN_fb
	
	
	// ADC Trigger Selection, New ILPFC board
	TrigSel[0] = ADCTRIG_EPWM3_SOCA;	// ePWM3, ADCSOCA
	TrigSel[1] = ADCTRIG_EPWM3_SOCA;	// ePWM3, ADCSOCA
	TrigSel[2] = ADCTRIG_EPWM4_SOCA;	// ePWM4, ADCSOCA
	TrigSel[3] = ADCTRIG_EPWM3_SOCB;	// ePWM3, ADCSOCB
	TrigSel[4] = ADCTRIG_EPWM4_SOCB;	// ePWM4, ADCSOCB
	
	TrigSel[5] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[6] = ADCTRIG_EPWM2_SOCA;	// ePWM2, ADCSOCA
	TrigSel[7] = ADCTRIG_EPWM1_SOCB;	// ePWM1, ADCSOCB
	
	TrigSel[8] = ADCTRIG_EPWM2_SOCB;	// ePWM2, ADCSOCB
	TrigSel[9] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[10] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[11] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA


   	// Configure ADC 
	InitAdc();
	AdcOffsetSelfCal();
	ADC_SOC_CNF(ChSel, TrigSel, ACQPS, 17, 0);

	EPwm2Regs.CMPA.half.CMPA = 75;
	EPwm3Regs.CMPB = 150;
	EPwm4Regs.CMPA.half.CMPA = 75;
	EPwm4Regs.CMPB = 225;

	// Configure ePWMs to generate ADC SOC pulses for PFC current oversampling
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;					// Enable ePWM1 SOCA pulse
	EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;		// SOCA from ePWM1 Zero event
	EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;			// Trigger ePWM1 SOCA on every event
	EPwm1Regs.ETSEL.bit.SOCBEN = 1;					// Enable ePWM1 SOCB pulse
	EPwm1Regs.ETSEL.bit.SOCBSEL = ET_CTR_PRD;		// SOCB from ePWM1 PRD event
	EPwm1Regs.ETPS.bit.SOCBPRD = ET_1ST;			// Trigger ePWM1 SOCB on every event

	EPwm2Regs.ETSEL.bit.SOCAEN = 1;					// Enable ePWM2 SOCA pulse
	EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;		// SOCA from ePWM2 CMPA up event
	EPwm2Regs.ETPS.bit.SOCAPRD = ET_1ST;			// Trigger ePWM2 SOCA on every event
	EPwm2Regs.ETSEL.bit.SOCBEN = 1;					// Enable ePWM2 SOCB pulse
	EPwm2Regs.ETSEL.bit.SOCBSEL = ET_CTRD_CMPA;		// SOCB from ePWM2 CMPA down event
	EPwm2Regs.ETPS.bit.SOCBPRD = ET_1ST;			// Trigger ePWM2 SOCB on every event

	// Configure ePWMs to generate ADC SOC pulses
	EPwm3Regs.ETSEL.bit.SOCAEN = 1;					// Enable ePWM3 SOCA pulse
	EPwm3Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;		// SOCA from ePWM3 zero event
	EPwm3Regs.ETPS.bit.SOCAPRD = ET_1ST;			// Trigger ePWM3 SOCA on every event
	EPwm3Regs.ETSEL.bit.SOCBEN = 1;					// Enable ePWM3 SOCB pulse
	EPwm3Regs.ETSEL.bit.SOCBSEL = ET_CTRU_CMPB;		// SOCB from ePWM3 CMPB up event
	EPwm3Regs.ETPS.bit.SOCBPRD = ET_1ST;			// Trigger ePWM3 SOCB on every event

	EPwm4Regs.ETSEL.bit.SOCAEN = 1;					// Enable ePWM4 SOCA pulse
	EPwm4Regs.ETSEL.bit.SOCASEL = ET_CTRU_CMPA;		// SOCA from ePWM4 CMPA up event
	EPwm4Regs.ETPS.bit.SOCAPRD = ET_1ST;			// Trigger ePWM4 SOCA on every event
	EPwm4Regs.ETSEL.bit.SOCBEN = 1;					// Enable ePWM4 SOCB pulse
	EPwm4Regs.ETSEL.bit.SOCBSEL = ET_CTRU_CMPB;		// SOCB from ePWM4 CMPB up event
	EPwm4Regs.ETPS.bit.SOCBPRD = ET_1ST;			// Trigger ePWM4 SOCB on every event

/*	EPwm4Regs.ETSEL.bit.SOCAEN = 1;					// Enable ePWM4 SOCA pulse
	EPwm4Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;		// SOCA from ePWM4 Zero event 
	EPwm4Regs.ETPS.bit.SOCAPRD = ET_1ST;			// Trigger ePWM4 SOCA on every event*/

	EPwm1Regs.TBCTL.bit.PHSEN=TB_DISABLE;
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	
/*	EPwm4Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_IN;
	EPwm4Regs.TBCTL.bit.PHSEN=TB_ENABLE; 
	EPwm4Regs.TBCTL.bit.PHSDIR=TB_DOWN;*/
	
	EPwm2Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_IN;
	EPwm2Regs.TBCTL.bit.PHSEN=TB_ENABLE; 
	EPwm2Regs.TBCTL.bit.PHSDIR=TB_UP;
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	EPwm3Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_IN;
	EPwm3Regs.TBCTL.bit.PHSEN=TB_ENABLE;
	EPwm3Regs.TBCTL.bit.PHSDIR=TB_UP;
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
	EPwm4Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_IN;
	EPwm4Regs.TBCTL.bit.PHSEN=TB_ENABLE;
	EPwm4Regs.TBCTL.bit.PHSDIR=TB_UP;
//*************************************************

	DPL_Init();
//----------------------------------------------------------------------
#if (INCR_BUILD == 1) 	// Open Loop Debug only
//----------------------------------------------------------------------

	// Lib Module connection to "nets" 
	//----------------------------------------
	// Connect the PWM Driver input to an input variable, Open Loop System
	ADCDRV_1ch_Rlt1 = &Ipfc1;
	ADCDRV_1ch_Rlt2 = &Ipfc2;
	ADCDRV_1ch_Rlt3 = &Ipfc3;
	ADCDRV_1ch_Rlt4 = &Ipfc4;
	ADCDRV_1ch_Rlt5 = &Ipfc5;
	ADCDRV_1ch_Rlt6 = &Ipfc6;
	ADCDRV_1ch_Rlt7 = &Ipfc7;
	ADCDRV_1ch_Rlt8 = &Ipfc8;
	ADCDRV_1ch_Rlt9 = &Vbus;
	ADCDRV_1ch_Rlt10 = &VL_fb;
	ADCDRV_1ch_Rlt11 = &VN_fb;
	
	PWMDRV_2ch_UpDwnCnt_Duty1 = &DutyA;
	
	// Math_avg block connections - Instance 2
	MATH_EMAVG_In2=&Vbus;
	MATH_EMAVG_Out2=&VbusAvg;
	MATH_EMAVG_Multiplier2=_IQ30(0.00025);
	
	// INV_RMS_SQR block connections 
	VrectRMS = (sine_mainsV.Vrms)<< 9;//Q15 --> Q24, (sine_mainsV.Vrms) is in Q15
	PFC_InvRmsSqr_In1=&VrectRMS;	
	PFC_InvRmsSqr_Out1=&VinvSqr;
	PFC_InvRmsSqr_VminOverVmax1=_IQ30(0.1956);		// 80V/409V
	PFC_InvRmsSqr_Vmin1=_IQ24(0.1956);	
	
		// PFC_ICMD block connections
	PFC_ICMD_Vcmd1 = &VbusVcmd;
	PFC_ICMD_VinvSqr1=&VinvSqr;
	PFC_ICMD_VacRect1=&Vrect;
	PFC_ICMD_Out1=&PFCIcmd;
	PFC_ICMD_VmaxOverVmin1=_IQ24(3.00);		// 3.5625 <=> 285V/80V

	// Initialize the net variables
	DutyA =_IQ24(0.1);//Variable initialized for open loop test
	VrectAvg = _IQ24(0.0);
	VrectRMS = _IQ24(0.0);
	VbusAvg = _IQ24(0.0);
	VinvSqr = _IQ24(0.0);
	Vrect = _IQ24(0.0);
	PFCIcmd = _IQ24(0.0);
	VbusVcmd = _IQ24(0.0);
    temp_zero = 0;
		
#endif // (INCR_BUILD == 1),  
//--------------------------------------------------------------------------------	
	
	
//----------------------------------------------------------------------
#if (INCR_BUILD == 2) 	// Closed Current Loop IL PFC, Open Volt Loop
//----------------------------------------------------------------------
	// Lib Module connection to "nets" 
	ADCDRV_1ch_Rlt1 = &Ipfc1;
	ADCDRV_1ch_Rlt2 = &Ipfc2;
	ADCDRV_1ch_Rlt3 = &Ipfc3;
	ADCDRV_1ch_Rlt4 = &Ipfc4;
	ADCDRV_1ch_Rlt5 = &Ipfc5;
	ADCDRV_1ch_Rlt6 = &Ipfc6;
	ADCDRV_1ch_Rlt7 = &Ipfc7;
	ADCDRV_1ch_Rlt8 = &Ipfc8;
	ADCDRV_1ch_Rlt9 = &Vbus;
	ADCDRV_1ch_Rlt10 = &VL_fb;
	ADCDRV_1ch_Rlt11 = &VN_fb;
	
	//connect the 2P2Z connections, for the inner Current Loop, Loop1
	CNTL_2P2Z_Ref1 = &PFCIcmd;
	CNTL_2P2Z_Out1 = &DutyA; 
	CNTL_2P2Z_Fdbk1= &Ipfc_fltr;
	CNTL_2P2Z_Coef1 = &CNTL_2P2Z_CoefStruct1.b2;
	// Math_avg block connections - Instance 2
	MATH_EMAVG_In2=&Vbus;
	MATH_EMAVG_Out2=&VbusAvg;
	MATH_EMAVG_Multiplier2=_IQ30(0.00025);
	// INV_RMS_SQR block connections 
	VrectRMS = (sine_mainsV.Vrms)<< 9;//Q15 --> Q24, (sine_mainsV.Vrms) is in Q15
	PFC_InvRmsSqr_In1=&VrectRMS;
	PFC_InvRmsSqr_Out1=&VinvSqr;
	PFC_InvRmsSqr_VminOverVmax1=_IQ30(0.1956);		// 80V/409V
	PFC_InvRmsSqr_Vmin1=_IQ24(0.1956);			// 80V/409V
	
	// PFC_ICMD block connections
	PFC_ICMD_Vcmd1 = &VbusVcmd;
	PFC_ICMD_VinvSqr1=&VinvSqr;
	PFC_ICMD_VacRect1=&Vrect;
	PFC_ICMD_Out1=&PFCIcmd;
	PFC_ICMD_VmaxOverVmin1=_IQ24(3.00);		// 3.5625 <=> 285V/80V

	PWMDRV_2ch_UpDwnCnt_Duty1 = &DutyA;

	// Math_avg block connections - Instance 1
	MATH_EMAVG_In1=&Vrect;
	MATH_EMAVG_Out1=&VrectAvg;
	MATH_EMAVG_Multiplier1=_IQ30(0.000030);
	
	// Initialize the net variables
	DutyA =_IQ24(0.0);
	VrectAvg = _IQ24(0.0);
	VrectRMS = _IQ24(0.0);
	VbusAvg = _IQ24(0.0);
	VinvSqr = _IQ24(0.0);
	Vrect = _IQ24(0.0);
	PFCIcmd = _IQ24(0.0);
	VbusVcmd = _IQ24(0.05);//Variable initialized for open Volt loop $ closed current loop test with light load
	VbusTarget = _IQ24(0.0);
	VbusTargetSlewed = _IQ24(0.0);
    pfc_slew_temp = 0;
    start_flag = 0;
    temp_zero = 0;
    
    
    // Coefficients for Inner Current Loop
	// PID coefficients & Clamp values - Current loop (Q26), 100kHz Cloop sampling
	Dmax_I  = _IQ24(0.999); 	
	    
    CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);    // B2
    CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.3131);// B1
    CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.5506);  // B0
    
	CNTL_2P2Z_CoefStruct1.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);                       	// A1 = 1 
    
    CNTL_2P2Z_CoefStruct1.max  =Dmax_I;					  		  	//Clamp Hi 	
	CNTL_2P2Z_CoefStruct1.i_min  =_IQ24(-0.98); 					//Clamp Min for control calculation
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min used for output	
		
#endif // (INCR_BUILD == 2),  


//--------------------------------------------------------------------------------
//----------------------------------------------------------------------
#if (INCR_BUILD == 3) 	// Closed Current Loop & closed volt loop IL PFC
//----------------------------------------------------------------------
	// Lib Module connection to "nets" 
	
	ADCDRV_1ch_Rlt1 = &Ipfc1;
	ADCDRV_1ch_Rlt2 = &Ipfc2;
	ADCDRV_1ch_Rlt3 = &Ipfc3;
	ADCDRV_1ch_Rlt4 = &Ipfc4;
	ADCDRV_1ch_Rlt5 = &Ipfc5;
	ADCDRV_1ch_Rlt6 = &Ipfc6;
	ADCDRV_1ch_Rlt7 = &Ipfc7;
	ADCDRV_1ch_Rlt8 = &Ipfc8;
	ADCDRV_1ch_Rlt9 = &Vbus;
	ADCDRV_1ch_Rlt10 = &VL_fb;
	ADCDRV_1ch_Rlt11 = &VN_fb;
	
	//connect the 2P2Z connections, for the inner Current Loop, Loop1
	CNTL_2P2Z_Ref1 = &PFCIcmd;//Testing this with OVS scheme. This is appropriate with OVS scheme
	CNTL_2P2Z_Out1 = &DutyA; 
	CNTL_2P2Z_Fdbk1= &Ipfc_fltr;
	CNTL_2P2Z_Coef1 = &CNTL_2P2Z_CoefStruct1.b2;
	//connect the 2P2Z connections, for the outer Voltage Loop, Loop2
	CNTL_2P2Z_Ref2 = &VbusTargetSlewed;
	CNTL_2P2Z_Out2 = &VbusVcmd;
	CNTL_2P2Z_Fdbk2= &Vbus;
	CNTL_2P2Z_Coef2 = &CNTL_2P2Z_CoefStruct2.b2;
	// Math_avg block connections - Instance 2
	MATH_EMAVG_In2=&Vbus;
	MATH_EMAVG_Out2=&VbusAvg;//Average PFC bus volt calculated for OV protection
	MATH_EMAVG_Multiplier2=_IQ30(0.00025);
	// INV_RMS_SQR block connections 
	VrectRMS = (sine_mainsV.Vrms)<< 9;//Q15 --> Q24, (sine_mainsV.Vrms) is in Q15
	PFC_InvRmsSqr_In1=&VrectRMS;	
	PFC_InvRmsSqr_Out1=&VinvSqr;
	PFC_InvRmsSqr_VminOverVmax1=_IQ30(0.1956);		// 80V/409V
	PFC_InvRmsSqr_Vmin1=_IQ24(0.1956);	
	
	// PFC_ICMD block connections
	PFC_ICMD_Vcmd1 = &VbusVcmd;
	PFC_ICMD_VinvSqr1=&VinvSqr;
	PFC_ICMD_VacRect1=&Vrect;
	PFC_ICMD_Out1=&PFCIcmd;
	PFC_ICMD_VmaxOverVmin1=_IQ24(3.00);		// 3.5625 <=> 285V/80V
	
	PWMDRV_2ch_UpDwnCnt_Duty1 = &DutyA;
	
	// Math_avg block connections - Instance 1
	MATH_EMAVG_In1=&Vrect;
	MATH_EMAVG_Out1=&VrectAvg;
	MATH_EMAVG_Multiplier1=_IQ30(0.000015);
	

	// Initialize the net variables
	DutyA =_IQ24(0.0);
	VrectAvg = _IQ24(0.0);
	VbusAvg = _IQ24(0.0);
	VinvSqr = _IQ24(0.0);
	Vrect = _IQ24(0.0);
	PFCIcmd = _IQ24(0.0);
	VbusVcmd = _IQ24(0.0);
	VbusTarget = _IQ24(0.0);
	VbusTargetSlewed = _IQ24(0.0);
	VbusSlewRate = 25600;
	init_boost = 10240;
	error_v=0;
    pfc_slew_temp = 0;
    start_flag = 1;//initialize to 1 for standalone start-up when min Vbus volt is established.  
    				//Init to 0 when starting from CCS watch window.
    pfc_on_flag = 0;
    Soft_Start_Phase = 0;
    temp_zero = 0;
    disable_auto_cloop_coeff_change = 0;//0 => Auto coeff update enabled, 1=> auto coeff update disabled
    flag_NL_Vloop=0;
    
    //--------------------------------------------------------------------------------
    // Coefficients for Inner Current Loop
    //--------------------------------------------------------------------------------
	// PID coefficients & Clamp values - Current loop (Q26), 100kHz Cloop sampling
	
	Dmax_I  = _IQ24(0.999); //Max output from Cloop controller	
	   
    CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.07124);    // B2
    CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.3822);// B1
    CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.4104);  // B0
    		
	CNTL_2P2Z_CoefStruct1.a2   =_IQ26(-0.116);                           // A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.116);                       	// A1 = 1                         	      
    
    CNTL_2P2Z_CoefStruct1.max  =Dmax_I;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct1.i_min  =_IQ24(-0.98); 					  	//Clamp Min for control calculation
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min used for output	
    
//------------------------------------------------------------------------------
// Coefficients for Outer Voltage Loop
//------------------------------------------------------------------------------
	// PID coefficients & Clamp values - Voltage loop (Q26), 50kHz Vloop sampling
	
	Dmax_V  = _IQ24(0.999);//0.99);//Max output from Vloop controller	
	
	Pgain_V = _IQ26(0.25);	  
	Igain_V = _IQ26(0.0005); 
	Dgain_V =_IQ26(0.0); 			
				
	// Coefficient init	--- Coeeficient values in Q26
	// Use IQ Maths to generate floating point values for the CLA 
	CNTL_2P2Z_CoefStruct2.b2   =Dgain_V;                            // B2
    CNTL_2P2Z_CoefStruct2.b1   =(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    CNTL_2P2Z_CoefStruct2.b0   =(Pgain_V + Igain_V + Dgain_V);      // B0
    CNTL_2P2Z_CoefStruct2.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct2.a1   =_IQ26(1.0);                       	// A1 = 1 
    
    CNTL_2P2Z_CoefStruct2.max  =Dmax_V;					  		  	//Clamp Hi    
    CNTL_2P2Z_CoefStruct2.i_min  =_IQ24(0.0); 					//Clamp Min for control calculation
    CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0); 					  	//Clamp Min used for output	
    		
#endif // (INCR_BUILD == 3),

//--------------------------------------------------------------------------------

//Activate /TZ1 under s/w control to shut off PWM4A, PWM4B under DC bus overvoltage condition
		EALLOW;
		
		EPwm1Regs.TZSEL.bit.OSHT1 = 1;
		//EPwm2Regs.TZSEL.bit.OSHT1 = 1;
		EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM1A will go low 
		EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWM1B will go low
		//EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM2A will go low
				
/*		EPwm4Regs.TZSEL.bit.OSHT1 = 1;
		EPwm4Regs.TZSEL.bit.OSHT1 = 1;
		EPwm4Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM4A will go low 
		EPwm4Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWM4B will go low*/
		
		EDIS;
//====================================================================================
// INTERRUPTS & ISR INITIALIZATION (best to run this section after other initialization)
//====================================================================================

//Also Set the appropriate # define's in the BridgelessPFC-Settings.h 
//to enable interrupt management in the ISR
	EALLOW;
// Set up C28x Interrupt

// ADC EOC based ISR trigger
//	PieVectTable.ADCINT1 = &DPL_ISR;      		// Map Interrupt
//	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;      	// PIE level enable, Grp1 / Int1, ADCINT1
//	AdcRegs.INTSEL1N2.bit.INT1SEL = 4;			// ADC Channel 4 EOC causes ADCInterrupt 1
//	IER |= M_INT1;                          	// Enable CPU INT1 group:

// PWM based ISR trigger
    //PieVectTable.EPWM1_INT = &DPL_ISR;      	// Map Interrupt
    PieVectTable.EPWM2_INT = &DPL_ISR;      	// Map Interrupt
   	//PieCtrlRegs.PIEIER3.bit.INTx1 = 1;      	// PIE level enable, Grp3 / Int1, ePWM1
   	PieCtrlRegs.PIEIER3.bit.INTx2 = 1;      	// PIE level enable, Grp3 / Int2, ePWM2

	//EPwm1Regs.CMPB = 80;						// ISR trigger point
	EPwm2Regs.CMPB = 80;						// ISR trigger point
	
   	EPwm2Regs.ETSEL.bit.INTSEL = ET_CTRU_CMPB; 	// INT on CompareB-Up event
   	EPwm2Regs.ETSEL.bit.INTEN = 1;              // Enable INT
    
    //EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;         // Generate INT on every event
 												
    EPwm2Regs.ETPS.bit.INTPRD = ET_2ND;         // Generate INT on every other event
    
	IER |= M_INT3;                          	// Enable CPU INT3 connected to EPWM1-6 INTs:
     
    EINT;                                   	// Enable Global interrupt INTM
    ERTM;                                   	// Enable Global realtime interrupt DBGM
	EDIS;      

//=================================================================================
//	BACKGROUND (BG) LOOP
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
	for(;;)  //infinite loop
	{
		// State machine entry & exit point
		//===========================================================
		(*Alpha_State_Ptr)();	// jump to an Alpha state (A0,B0,...)
		//===========================================================

	}
} //END MAIN CODE

interrupt void SECONDARY_ISR(void)
{
	EINT;
	
		if (VbusAvg > VBUS_OVP_THRSHLD)//Check for Vbus OV Condition; Use for Build 2 & 3
		//if (Vbus > VBUS_OVP_THRSHLD)//Use this for Build 1 test
		{
		 	OV_flag = 1;	
		 	EALLOW;
		 	//EPwm4Regs.TZFRC.bit.OST = 1;//Turn off PWM for OV condition
		 	EPwm1Regs.TZFRC.bit.OST = 1;//Turn off PWM for OV condition
		 	EDIS;	
		 	
		 	VbusTargetSlewed = 0;
		 	VbusTarget = 0;		
		 	pfc_on_flag = 0;
		 	//Gui_Vbus_set = 0;	 	
		}			
	
	//Calculate RMS input voltage and input frequency
//	sine_mainsV.Iin = _IQ15mpy(Igain, (Ipfc_fltr >> 9)); // + Ioffset; // input in IQ15 format
//	sine_mainsV.Vin = (Vrect >> 9) + Vdiode + _IQ15mpy((Ipfc_fltr >> 9), Rshunt); // input in IQ15 format
	sine_mainsV.Iin = Ipfc_fltr >> 9;
	sine_mainsV.Vin = Vrect >> 9;
	SineAnalyzer_MACRO (sine_mainsV);
	VrectRMS = (sine_mainsV.Vrms)<< 9;//    Convert sine_mainsV.Vrms from Q15 to Q24 and save as VrectRMS
	IrectRMS = (sine_mainsV.Irms)<< 9;//    Convert sine_mainsV.Irms from Q15 to Q24 and save as IrectRMS
	PinRMS = (sine_mainsV.Prms)<< 9;//    Convert sine_mainsV.Prms from Q15 to Q24 and save as PinRMS
	Freq_Vin = sine_mainsV.SigFreq;// Q15
	//VrmsReal = _IQ15mpy (KvInv, sine_mainsV.Vrms);


	//Gui_PinRMS = (sine_mainsV.Prms*K_Prms) >> 15;//Q15*Q15 >> 15 = Q15, When no calibration is used

		if((sine_mainsV.Prms>123)&&(sine_mainsV.Prms <= 858)){	//Max Power used for normalization is 19.8*405.2=8022.96W.// 
										//So 210W = 210/8022.96 *32767 = 858 in Q15 format//
										// 30W = 30/8022.96 *32767 = 123 in Q15 format//
			Gui_PinRMS = (((sine_mainsV.Prms*slope_Pcorr) >> 15)- offset_Pcorr);//Q15*Q15 >> 15 = Q15, 
			//apply calibration between 30W and 210W
		}
		else{
			Gui_PinRMS = (sine_mainsV.Prms*K_Prms) >> 15;//Q15*Q15 >> 15 = Q15, apply zero calibration 
		}


	Gui_PinRMS_16 = (int16)(Gui_PinRMS);
	



	if (INCR_BUILD == 3)//Start of Build 3 specific feature
	{
		//Start of Non-linear Volt loop control
		if ((pfc_on_flag == 1) && (flag_NL_Vloop == 1)) //If soft-start is over, PFC running normally & NL Vloop flag is set
		{
		
			error_v = VbusTargetSlewed - Vbus;

			if ((error_v > VBUS_ERROR_NL_CNTRL_THRSHLD) || (((-1)*(error_v))> VBUS_ERROR_NL_CNTRL_THRSHLD)) 
				// (15V/519V)*4095*4095 = 484654.0
				//(12V/519V)*4095*4095 = 387912.0
			{
			CNTL_2P2Z_CoefStruct2.b1   =_IQ26(-4.495);//(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // 4.5, 0.005
    		CNTL_2P2Z_CoefStruct2.b0   =_IQ26(4.505);//(Pgain_V + Igain_V + Dgain_V);      // B0						;//Use NL v loop coefficients
			}
			else
			{
			CNTL_2P2Z_CoefStruct2.b1   =_IQ26(-0.2495);//Otherwise use normal v loop coefficients
			CNTL_2P2Z_CoefStruct2.b0   =_IQ26(0.2505);//KP=0.25, KI=0.0005
			}
		} 
		//End of NL V loop Control


	//Start of Adaptive Current control loop
		if (disable_auto_cloop_coeff_change == 0)
		{
			//if (VrectRMS > 4512209 && VrectRMS <= 8204017)//Input between 110V and 200V  
			if (VrectRMS <= 8204017)//Input less than 200V  
			{
			//(110/409)*2e24 = 4512209,
			// (200/409)*2e24 = 8204017,
				if(VbusVcmd > _IQ24(0.22))//Load higher than 20%
				//if(VbusVcmd > _IQ24(0.18))//Load higher than 18%
				{
			    	CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.07124);    // B2
    				CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.3822);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.4104);  // B0
    		
					CNTL_2P2Z_CoefStruct1.a2   =_IQ26(-0.116); // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.116);  // A1 = 1   
				}
				
				if(VbusVcmd <= _IQ24(0.22)&&(VbusVcmd > _IQ24(0.1)))//Load 20% or lower, higher than 10%
				//if(VbusVcmd <= _IQ24(0.18)&&(VbusVcmd > _IQ24(0.1)))//Load 18% or lower, higher than 10%
				{
					//Set 1 :
					CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
					CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.4167);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.024);  // B0
    				CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
    				 
				}
				
				if(VbusVcmd <= _IQ24(0.1))//Load 10% or lower
				{	//increased gain by 50%*******
					CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
					CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.62505);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.536);  // B0
    				CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
    				 
				}
				/*if(VbusVcmd <= _IQ24(0.1))//Load 10% or lower
				{	//increased gain by 35%***********
					CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
					CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.5625);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.3824);  // B0
    				CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
				}*/
			}
			
			if (VrectRMS > 8204017)//Input grater than 200V
			{
			// (200/409)*2e24 = 8204017,
			
				//if(VbusVcmd > _IQ24(0.22))//Load higher than 20%
				if(VbusVcmd > _IQ24(0.42))//Load higher than 50%
				{
					
					//CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
					//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.4449);// B1
    				//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.7824);  // B0
    				
    				//CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);    // B2
    				//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.3131);// B1
    				//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.5506);  // B0
    		
    				//CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				//CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
    				
    				//CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0178);    // B2
    				//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.387);// B1
    				//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.6256);  // B0			
					
					CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0144);    // B2
    				CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.3201);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.6305);  // B0	
					
					CNTL_2P2Z_CoefStruct1.a2   =_IQ26(-0.348);                         
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.348);  
				}
				
				if((VbusVcmd <= _IQ24(0.42))&&(VbusVcmd > _IQ24(0.35)))//Load higher than % and less than %
				{
					CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
					CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.4449);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.7824);  // B0
    		
    				CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
				}
				
				//if((VbusVcmd <= _IQ24(0.42))&&(VbusVcmd > _IQ24(0.22)))//Load higher than 20% and less than 50%
				if((VbusVcmd <= _IQ24(0.35))&&(VbusVcmd > _IQ24(0.22)))
				{
					CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
    				CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.4167);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.024);  // B0
    		
    				CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
				}
				
				if((VbusVcmd <= _IQ24(0.22))&&(VbusVcmd > _IQ24(0.1)))//Load 20% to 10%
				{
					//CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
					//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.4167);// B1
    				//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.024);  // B0
    				
    				//CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
    				//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.2902);// B1
    				//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.4);  // B0
    		
    		    	CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0);       // B2
    				CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.1598);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.598);  // B0
    		
    				CNTL_2P2Z_CoefStruct1.a2   =0.0;           // A2 = 0
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);   // A1 = 1 
				}
				
				if(VbusVcmd <= _IQ24(0.1))//Load less than 10%
				{
    				CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0104);    // B2
    				CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-1.059);// B1
    				CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.881);  // B0			
					CNTL_2P2Z_CoefStruct1.a2   =_IQ26(-0.5);                         
    				CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.5);  
				}
			}
		}//*********End of Adaptive Current control loop
	}//******************End of Build 3 specific features
	

	EPwm7Regs.ETCLR.bit.INT = 1;					// Enable ePWM7 INTN pulse
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
	return;
}


//=================================================================================
//	STATE-MACHINE SEQUENCING AND SYNCRONIZATION
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
void A0(void)
{
	// loop rate synchronizer for A-tasks
	if(CpuTimer0Regs.TCR.bit.TIF == 1)
	{
		CpuTimer0Regs.TCR.bit.TIF = 1;	// clear flag

		//-----------------------------------------------------------
		(*A_Task_Ptr)();		// jump to an A Task (A1,A2,A3,...)
		//-----------------------------------------------------------
		
		SerialCommsTimer++;

		VTimer0[0]++;			// virtual timer 0, instance 0 (spare)
	}

	Alpha_State_Ptr = &B0;		// Comment out to allow only A tasks
}

void B0(void)
{
	// loop rate synchronizer for B-tasks
	if(CpuTimer1Regs.TCR.bit.TIF == 1)
	{
		CpuTimer1Regs.TCR.bit.TIF = 1;				// clear flag

		//-----------------------------------------------------------
		(*B_Task_Ptr)();		// jump to a B Task (B1,B2,B3,...)
		//-----------------------------------------------------------
		VTimer1[0]++;			// virtual timer 1, instance 0 (spare)
	}

	Alpha_State_Ptr = &C0;		// Allow C state tasks
}

void C0(void)
{
	// loop rate synchronizer for C-tasks
	if(CpuTimer2Regs.TCR.bit.TIF == 1)
	{
		CpuTimer2Regs.TCR.bit.TIF = 1;				// clear flag

		//-----------------------------------------------------------
		(*C_Task_Ptr)();		// jump to a C Task (C1,C2,C3,...)
		//-----------------------------------------------------------
		VTimer2[0]++;			//virtual timer 2, instance 0 (spare)
	}

	Alpha_State_Ptr = &A0;	// Back to State A0
}


//=================================================================================
//	A - TASKS
//=================================================================================
//--------------------------------------------------------
void A1(void) 
//--------------------------------------------------------
{

	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2
	A_Task_Ptr = &A2;
	//-------------------
}

//-----------------------------------------------------------------
void A2(void) 
//-----------------------------------------------------------------
{	 
	SerialHostComms();	// Uncomment for FLASH config
	
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-------------------
}


//=================================================================================
//	B - TASKS
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void)
//----------------------------------------
{	
	HistPtr++;
	if (HistPtr >= HistorySize)	
		HistPtr = 0;

	// BoxCar Averages - Input Raw samples into BoxCar arrays
	//----------------------------------------------------------------
	
	Hist_Vbus[HistPtr] = (int16)(VbusAvg>>12);
	//VbusAvg is in Q24. So first make it Q12 and then change it to int16.
	
	//Hist_VrectAvg[HistPtr] = (int16)(VrectAvg>>12);
	//VrectAvg is in Q24. So first make it Q12 and then change it to int16
	
	Hist_VrectRMS[HistPtr] = (int16)(VrectRMS>>12);
	//VrectRMS is in Q24. So first make it Q12 and then change it to int16
	
	Hist_Freq_Vin[HistPtr] = (int16)(Freq_Vin >>12);
	//Freq_Vin is in Q15.
	
	Hist_IrectRMS[HistPtr] = (int16)(IrectRMS>>12);
	//IrectRMS is in Q24. So first make it Q12 and then change it to int16


	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_Vbus[i];	//Q12 * 8 = Q15
	
	Gui_Vbus = ((long)temp_Scratch*(long)K_Vbus) >> 15;	//Q15*Q15 >> 15 = Q15

	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_VrectRMS[i];	//Q12 * 8 = Q15
	
	Gui_VrectRMS = ((long)temp_Scratch*(long)K_Vrms) >> 15;	//Q15*Q15 >> 15 = Q15

	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_Freq_Vin[i];	//Q3 * 8 = Q6
	
	Gui_Freq_Vin = ((long)temp_Scratch);	//Q6

	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_IrectRMS[i];	//Q12 * 8 = Q15
	
	Gui_IrectRMS = ((long)temp_Scratch*(long)K_Irms) >> 15;	//Q15*Q15 >> 15 = Q15

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2
	B_Task_Ptr = &B2;	
	//-----------------
}

//----------------------------------------
void B2(void) // Blink LED on the control CArd
//----------------------------------------
{
	if(LedBlinkCnt==0)
		{
			GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;	//turn on/off LD3 on the controlCARD
			LedBlinkCnt=5;
		}
	else
			LedBlinkCnt--;
			
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;
	//-----------------
}


//=================================================================================
//	C - TASKS
//=================================================================================

//--------------------------------- USER ------------------------------------------

//------------------------------------------------------
void C1(void)  // PFC Start Phase
//------------------------------------------------------
{
	
	if (INCR_BUILD == 3)
	{
		if (VbusTargetSlewed == 0) //
		{
			temp_zero = 0;
			CNTL_2P2Z_Ref2 = &temp_zero;	// Slewed Voltage Command
		}

		if ((Soft_Start_Phase == 0)&&(pfc_on_flag == 0))//If PFC is not in Soft Start mode, and not in run mode
		{
			if (start_flag == 1 && Vbus > VBUS_MIN)//Use this to start PFC from CCS watch window using "start_flag"
			//Initialize start_flag to 1 to start PFC in stand alone mode.
			//To start PFC from CCS watch window initialize start_flag to 0 and then set it to 1 from CCS watch window
			{
				VbusTargetSlewed = Vbus+ init_boost; 		// Start slewing the boost command from a value slightly greater than the PFC output voltage 
				CNTL_2P2Z_Ref2 = &VbusTargetSlewed;			// Slewed Voltage Command 

				start_flag = 0;
				Soft_Start_Phase = 1;
			}
		}	
	}//*****End of Build 3 specific code
	
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C2
	C_Task_Ptr = &C2;
	//-------------------
}
//----------------------------------------
void C2(void) //PFC Soft Start Mode
//----------------------------------------
{ 
	if (INCR_BUILD == 3)
	{
		if(Soft_Start_Phase == 1)
		{
			// pfcSlewRate has to be a positive value
			//pfc_slew_temp = VbusTarget - VbusTargetSlewed;
			pfc_slew_temp = VBUS_TARGET - VbusTargetSlewed;

			if (pfc_slew_temp >= VbusSlewRate) // Positive Command. Slewed Vbus is less than VBUS_TARGET, so increase it. This is
									//implement soft-start for Vbus. VbusSlewRate is initialized at the begining of this file.
			{
				VbusTargetSlewed = VbusTargetSlewed + VbusSlewRate;
			}
			else	
			{
				//Soft-start is complete. So set the flag for RUN mode and go to Task C3 for RUN time adjustment of Vbus
				VbusTargetSlewed = VBUS_TARGET;
				VbusTarget = VBUS_TARGET;
				
				Soft_Start_Phase = 0;
				pfc_on_flag = 1;//PFC ON/OFF status flag
			}
		}//********End of Soft Start Phase Implementation
		
	}//*****End of Build 3 specific code

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C3
	C_Task_Ptr = &C3;	
	//-----------------
}


//-----------------------------------------
void C3(void) // PFC ON Mode
//-----------------------------------------
{
	if (INCR_BUILD == 3)
	{
		if (pfc_on_flag == 1) //If soft-start is over and PFC running normally
		{
			// pfcSlewRate has to be a positive value
			pfc_slew_temp = VbusTarget - VbusTargetSlewed;

			//pfc_slew_temp = VBUS_TARGET - VbusTargetSlewed;

			if (pfc_slew_temp >= VbusSlewRate) // Positive Command. Increase Vbus
			{
				VbusTargetSlewed = VbusTargetSlewed + VbusSlewRate;
			}
			else
			{
				if ((-1)*(pfc_slew_temp) >= VbusSlewRate) // Negative Command. Reduce Vbus
				{
					VbusTargetSlewed = VbusTargetSlewed - VbusSlewRate;
				}
			}
		}//********End of PFC ON Mode Functional Implementation
		
	 }//*****End of Build 3 specific code
	
	
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------

}



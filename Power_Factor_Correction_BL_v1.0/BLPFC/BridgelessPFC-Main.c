//----------------------------------------------------------------------------------
//	FILE:			BridgelessPFC-Main.C
//
//	Description:	Bridgeless PFC
//					The file drives duty on PWM1A and PWM2A using C28x
//					C28x ISR is triggered by the PWM 1 interrupt
//
//	Version: 		1.0
//
//  Target:  		TMS320F2803x(PiccoloB), 
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2004-2010
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// July 29 , 2010  - File created (DC)
//----------------------------------------------------------------------------------
//
// PLEASE READ - Useful notes about this Project

// Although this project is made up of several files, the most important ones are:
//	 "BridgelessPFC-Main.C"	- this file
//		- Application Initialization, Peripheral config,
//		- Application management
//		- Slower background code loops and Task scheduling
//	 "BridgelessPFC-DevInit_F28xxx.C
//		- Device Initialization, e.g. Clock, PLL, WD, GPIO mapping
//		- Peripheral clock enables
//		- DevInit file will differ per each F28xxx device series, e.g. F280x, F2833x,
//	 "BridgelessPFC-DPL-ISR.asm
//		- Assembly level library Macros and any cycle critical functions are found here
//	 "BridgelessPFC-Settings.h"
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
//  options are selected in file "BridgelessPFC-Settings.h".  Note: "Rebuild All" compile
//  tool bar button must be used if this file is modified.
//----------------------------------------------------------------------------------
#include "PeripheralHeaderIncludes.h"
#include "DSP2803x_EPWM_defines.h"		
#include "BridgelessPFC-Settings.h"
#include "IQmathLib.h"

//#include "Solar_SineAnalyzer.h"
#include "SineAnalyzer.h"
		
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

// Used for ADC Configuration 
int ChSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int TrigSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int ACQPS[16] = {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6};
// ---------------------------------- USER -----------------------------------------
int16 ChannelEnable;	// Enable PFC, [ 0="off", 1="on" ] 	
//int16 AC_Phase;			// AC line +/- phase, [ 0="+", 1="-" ]
int16 RelayEnable;

long Vbus_set;				// Control loop target

//long Pgain;				// PID P gain
//long Igain;				// PID I gain
//long Dgain;				// PID D gain
long Dmax_I;

//Solar_SineAnalyzer RMSCalc = Solar_SineAnalyzer_DEFAULTS;

// ---------------------------- DPLIB Net Pointers ---------------------------------
// Declare net pointers that are used to connect the DP Lib Macros  here 
//extern volatile long *CNTL_2P2Z_Ref1, *CNTL_2P2Z_Fdbk1, *CNTL_2P2Z_Out1;
//extern volatile long *CNTL_2P2Z_Coef1;

// ADC
extern volatile long *ADCDRV_1ch_Rlt1;	// instance #1, IpfcA
extern volatile long *ADCDRV_1ch_Rlt2;	// instance #2, IpfcB
extern volatile long *ADCDRV_1ch_Rlt3;	// instance #3, Vbus
extern volatile long *ADCDRV_1ch_Rlt4;	// instance #4, VL_fb
extern volatile long *ADCDRV_1ch_Rlt5;	// instance #5, VN_fb 

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

// PFC-ch1
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty1;	// instance #1, EPWM1
// PFC-ch2
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty2;	// instance #2, EPWM2
// Instrumentation-ch4
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty4;	// instance #3, EPWM4

extern volatile long *PFC_BL_ICMD_Vcmd1;
extern volatile long *PFC_BL_ICMD_VinvSqr1;
extern volatile long *PFC_BL_ICMD_VacRect1;
extern volatile long *PFC_BL_ICMD_Out1;
extern volatile long PFC_BL_ICMD_VmaxOverVmin1;
extern volatile long *PFC_BL_ICMD_Vpfc1;
extern volatile long *PFC_BL_ICMD_Duty1;
extern volatile long PFC_BL_ICMD_VoutMaxOverVinMax1;

//MATH_AVG - instance #1
extern volatile long *MATH_EMAVG_In1;
extern volatile long *MATH_EMAVG_Out1;
extern volatile long MATH_EMAVG_Multiplier1;

//MATH_AVG - instance #2
extern volatile long *MATH_EMAVG_In2;
extern volatile long *MATH_EMAVG_Out2;
extern volatile long MATH_EMAVG_Multiplier2;

//PFC_INVSQR - instance #1
//extern volatile long *PFC_INVSQR_In1;
//extern volatile long *PFC_INVSQR_Out1;
//extern volatile long PFC_INVSQR_VminOverVmax1;
//extern volatile long PFC_INVSQR_Vmin1;


//PFC_InvRmsSqr - instance #1
extern volatile long *PFC_InvRmsSqr_In1;
extern volatile long *PFC_InvRmsSqr_Out1;
extern volatile long PFC_InvRmsSqr_VminOverVmax1;
extern volatile long PFC_InvRmsSqr_Vmin1;


// ---------------------------- DPLIB Variables ---------------------------------
// Declare the net variables being used by the DP Lib Macro here 
volatile long Ipfc;//K_dcm_corr=0; 
volatile long Vbus; 
volatile long VL_fb; 
volatile long VN_fb; 

volatile long DutyA; 
volatile long Km, Vrect, VinvSqr, VrectAvg, VbusAvg, VbusAvg2, VrectRMS, Freq_Vin;
volatile long VbusVcmd,PFCIcmd;//PFCIcmd_avg;
volatile long Duty4A;
volatile long   VbusTarget, error_v=0;		// Set point for the PFC voltage loop

volatile long   VbusTargetSlewed;		// Slewed set point for the voltage loop
volatile long   VbusSlewRate = 25600;	// Voltage loop Slew rate adjustment (Q24)
//volatile long   VbusSlewRate = 12000;	// Voltage loop Slew rate adjustment (Q24)
volatile long   pfc_slew_temp;		// Temp variable: used only if implementing 
										// slew rate control in the slower state machine
long	temp_zero;

int 	init_boost = 10240;		// Small boost command when PFC is enabled the first time
int16 	start_flag, run_flag, OV_flag=0, flag_NL_Vloop=1;//Set NL Vloop flag for NL Vloop Control

//volatile long VrefNetBus;	 			// used as address for Vref

//volatile struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct1;
//volatile struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct2;

//volatile long Coef2P2Z_1[7];			// used as address for 2P2Z coefficients
//volatile long UoutNetBus=0;		 	// used as address for Uout

#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct1, "CNTL_2P2Z_Coef"); 
#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct2, "CNTL_2P2Z_Coef"); 
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct1;
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct2;

long Pgain_I,Igain_I,Dgain_I,Dmax_I;
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
//Uint16 Gui_Ipfc;	// Q11	
int16 Gui_Vbus;	// Q06
int16 Gui_VrectAvg;	// Q06
int16 Gui_VrectRMS;	// Q06
int16 Gui_Freq_Vin;	// Q06
int16 Gui_KDCM;//Q12


// Configure ("Set")
//Uint16 Gui_Vbus_set;
int16 	Gui_Vbus_set;

int16	Gui_Pgain_I, Gui_Igain_I, Gui_Dgain_I; // Itrip_level = 29669; 	// Current Loop P I D coefficients and DAC reference
int16	Gui_Pgain_V, Gui_Igain_V, Gui_Dgain_V;							// Voltage Loop P I D coefficients


// History arrays are used for Running Average calculation (boxcar filter)
// Used for CCS display and GUI only, not part of control loop processing
Uint16 Hist_Ipfc[HistorySize];
Uint16 Hist_Vbus[HistorySize];
Uint16 Hist_VrectAvg[HistorySize];
Uint16 Hist_VrectRMS[HistorySize];
Uint16 Hist_Freq_Vin[HistorySize];
Uint16 Hist_KDCM[HistorySize];

//Scaling Constants (exact value calibrated per board)
Uint16 K_Ipfc;	// 
Uint16 K_Vbus;	// 
Uint16 K_VrectAvg;	//
Uint16 K_Vrms;	//
Uint16 K_Freq_Vin;	//
Uint16 K_VL_fb;	// 
Uint16 K_VN_fb;	// 
Uint16 iK_Vbus_set;	// 

// Tuning the CNTL LOOP 
int16 UpdateCoef; 
volatile float	B2_I,B1_I,B0_I,A2_I,A1_I;
volatile float fz1,fz2, fp2, fp1,fs,Kdc;
volatile float wz1,wz2,wp1,wp2,wr,Q,c2,c1,c0,d2,d1,d0,den,pi;

int16	b2_Gui, b1_Gui, b0_Gui, a2_Gui, a1_Gui, a0_Gui;

int16	pid2p2z_Gui= 1;//Initialize this variable to 1
int16	coeff_change = 0, vloop_coeff_change = 0, disable_auto_cloop_coeff_change=0, KDCM=0;//These two variables must be initialized to 0

// Variables for background support only (no need to access)
int16 i;						// common use incrementer
Uint32 HistPtr, temp_Scratch; 	// Temp here means Temporary


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
	
	
	//"Set" variables
	//---------------------------------------
	// assign GUI variable Textboxes to desired "setable" parameter addresses
	
	
	//varGetList[0] = &Gui_Vbus;		// Q6
	
// ---------------------------------- USER -----------------------------------------
//  put common initialization/variable definitions here
//	Pgain=5;				//
//	Igain=2;				//
//	Dgain=1;				//

	K_Ipfc=16896;	// 
	//K_Vbus=29783;//15-bit GUI variable is denoted in Q6 since max Vbus is 465.38 which requires 9 integer bits;
	//K_Vbus=(465.38/2e9)*32767 = (465.38/512)*32767 = 29783
	//K_Vbus=33439;//K_Vbus=(522.5/512)*32767 = 33439
	K_Vbus=33215;//K_Vbus=(519/512)*32767 = 33215; Voltage will never gets to 512V. So this scaling is fine
	
	
	//K_VrectAvg=24589;	//K_VrectAvg=(384.22/2e9)*32767 = (384.22/512)*32767 = 24589
	K_VrectAvg=26226;	//K_VrectAvg=(409.8/512)*32767 = 26226
	K_Vrms=26226;	//K_Vrms=(409.8/512)*32767 = 26226
	K_VL_fb=26226;	// 
	K_VN_fb=26226;	// 
	//K_Freq_Vin=32767;	
	
	iK_Vbus_set=16163;//(512/522.5)*2e14=16055; //16677;	//(512/519)*2e14=16163, Q14 value
	
	Gui_Vbus_set=0;
	Vbus_set=0;
	//VrefNetBus=0;
	OV_flag = 0;

	ChannelEnable=0; 	
//	AC_Phase=0;
	RelayEnable=1;
	
	
//===============================================================================
//	INITIALISATION - GUI connections
//=================================================================================
// Use this section only if you plan to "Instrument" your application using the 
// Microsoft C# freeware GUI Template provided by TI

	//"Set" variables
	//---------------------------------------
	// assign GUI variable Textboxes to desired "setable" parameter addresses
	varSetTxtList[0] = &Gui_Vbus_set;

	varSetTxtList[1] = &b0_Gui;				// Q15
	varSetTxtList[2] = &b1_Gui;				// Q15
	varSetTxtList[3] = &b2_Gui;				// Q15
	varSetTxtList[4] = &a0_Gui;				// Q15
	varSetTxtList[5] = &a1_Gui;				// Q15
	varSetTxtList[6] = &a2_Gui;				// Q15 
	varSetTxtList[7] = &coeff_change;		// Q0
	varSetTxtList[8] = &vloop_coeff_change; // Q0
	varSetTxtList[9] = &KDCM; // Q12
//	varSetTxtList[10] = &Flag_DCM_Corr; // Q0
	
/*
	// assign GUI Buttons to desired flag addresses
	varSetBtnList[0] = &Gui_BtnListVar;
    varSetBtnList[1] = &Gui_BtnListVar;
	varSetBtnList[2] = &Gui_BtnListVar;
	varSetBtnList[3] = &Gui_BtnListVar;
	varSetBtnList[4] = &Gui_BtnListVar;
*/

	varSetBtnList[0] = &pid2p2z_Gui;

	// assign GUI Sliders to desired "setable" parameter addresses
	varSetSldrList[0] = &Gui_Pgain_I;
	varSetSldrList[1] = &Gui_Igain_I;
	varSetSldrList[2] = &Gui_Dgain_I;
	varSetSldrList[3] = &Gui_Pgain_V;
	varSetSldrList[4] = &Gui_Igain_V;
	varSetSldrList[5] = &Gui_Dgain_V;
	
	//"Get" variables
	//---------------------------------------
	// assign a GUI "getable" parameter address
	varGetList[0] = &Gui_Vbus;
	varGetList[1] = &Gui_VrectAvg;
	varGetList[2] = &Gui_VrectRMS;
	varGetList[3] = &Gui_Freq_Vin;
	varGetList[4] = &Gui_KDCM;
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
//	INCREMENTAL BUILD OPTIONS - NOTE: selected via {BridgelessPFC-Settings.h
//==================================================================================
// ---------------------------------- USER -----------------------------------------

#define period 300	//300 cycles -> 200KHz @60MHz CPU
#define period_instr_pwm 120	//512 cycles -> 117k @60MHz CPU

	// Configure PWM1 for 200Khz switching Frequency 
	PWM_1ch_UpDwnCnt_CNF(1, period, 1, 0); 
	// Configure PWM2 for 200Khz switching Frequency 
	PWM_1ch_UpDwnCnt_CNF(2, period, 0, 0); 

// Configure PWM4 for Instrumentation; 1Mhz switching Frequency 
	PWM_1ch_UpDwnCnt_CNF(4, period_instr_pwm, 0, 0);

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
      sine_mainsV.SampleFreq=_IQ15(10000.0);
      sine_mainsV.Threshold=_IQ15(0.1);//(0.02);
// End sine analyzer initialization


#define		IpfcR	AdcResult.ADCRESULT1		//Q12
#define		VbusR	AdcResult.ADCRESULT2		//Q12
#define		VL_fbR	AdcResult.ADCRESULT3		//Q12
#define		VN_fbR	AdcResult.ADCRESULT4		//Q12

			// ADC Channel Selection for C2000EVM
	ChSel[0] = 2;		// Dummy read for first 
	ChSel[1] = 2;		// A2 - IpfcA
	ChSel[2] = 6;		// A6 - Vbus
	ChSel[3] = 1;		// A1 - VL_fb
	ChSel[4] = 0;		// A0 - VN_fb
	
	// ADC Trigger Selection
	TrigSel[0] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[1] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[2] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[3] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA
	TrigSel[4] = ADCTRIG_EPWM1_SOCA;	// ePWM1, ADCSOCA

   	// Configure ADC 
	ADC_SOC_CNF(ChSel, TrigSel, ACQPS, 17, 0); 

	// Configure ePWMs to generate ADC SOC pulses
	EPwm1Regs.ETSEL.bit.SOCAEN = 1;					// Enable ePWM1 SOCA pulse
	EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;		// SOCA from ePWM1 Zero event
	EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;			// Trigger ePWM1 SOCA on every event


	DPL_Init();
//----------------------------------------------------------------------
#if (INCR_BUILD == 1) 	// Open Loop Debug only
//----------------------------------------------------------------------

	// Lib Module connection to "nets" 
	//----------------------------------------
	// Connect the PWM Driver input to an input variable, Open Loop System
	ADCDRV_1ch_Rlt1 = &Ipfc;
	ADCDRV_1ch_Rlt2 = &Vbus;
	ADCDRV_1ch_Rlt3 = &VL_fb;
	ADCDRV_1ch_Rlt4 = &VN_fb;
	
	PWMDRV_1ch_UpDwnCnt_Duty1 = &DutyA;
	PWMDRV_1ch_UpDwnCnt_Duty2 = &DutyA;
	
	PWMDRV_1ch_UpDwnCnt_Duty4 = &Duty4A;
	
	// Initialize the net variables
	DutyA =_IQ24(0.1);//Variable initialized for open loop test
	Duty4A =_IQ24(0.0);
	VrectAvg = _IQ24(0.0);
	VrectRMS = _IQ24(0.0);
	VbusAvg = _IQ24(0.0);
	VinvSqr = _IQ24(0.0);
	Vrect = _IQ24(0.0);
	PFCIcmd = _IQ24(0.0);
	VbusVcmd = _IQ24(0.0);
	VbusTarget = _IQ24(0.0);
	VbusTargetSlewed = _IQ24(0.0);
    pfc_slew_temp = 0;
    start_flag = 0;
    temp_zero = 0;
		
#endif // (INCR_BUILD == 1),  
//--------------------------------------------------------------------------------	
	
	
//----------------------------------------------------------------------
#if (INCR_BUILD == 2) 	// Closed Current Loop Bridgeless PFC, Open Volt Loop
//----------------------------------------------------------------------
	// Lib Module connection to "nets" 
	ADCDRV_1ch_Rlt1 = &Ipfc;
	ADCDRV_1ch_Rlt2 = &Vbus;
	ADCDRV_1ch_Rlt3 = &VL_fb;
	ADCDRV_1ch_Rlt4 = &VN_fb;
	//connect the 2P2Z connections, for the inner Current Loop, Loop1
	CNTL_2P2Z_Ref1 = &PFCIcmd;
	CNTL_2P2Z_Out1 = &DutyA; //Comment to open the curr loop.Then specify open loop duty to boost  bus volt.
	CNTL_2P2Z_Fdbk1= &Ipfc;
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
	// PFC_BL_ICMD block connections
	PFC_BL_ICMD_Vcmd1 = &VbusVcmd;
	PFC_BL_ICMD_VinvSqr1=&VinvSqr;
	PFC_BL_ICMD_VacRect1=&Vrect;
	PFC_BL_ICMD_Out1=&PFCIcmd;
	PFC_BL_ICMD_VmaxOverVmin1=_IQ24(2.00);		// 3.5625 <=> 285V/80V
	PFC_BL_ICMD_Vpfc1 = &Vbus;
	PFC_BL_ICMD_Duty1 = &DutyA;
	PFC_BL_ICMD_VoutMaxOverVinMax1 = _IQ24(1.266);//(1.272);//VoutMax=521.4V, VinMax=409.8V(peak)= 289.8Vrms

	PWMDRV_1ch_UpDwnCnt_Duty1 = &DutyA;
	PWMDRV_1ch_UpDwnCnt_Duty2 = &DutyA;
	PWMDRV_1ch_UpDwnCnt_Duty4 = &PFCIcmd;//Use DPWM4A and ext RC filter to verify PFC reference current
	
	// Math_avg block connections - Instance 1
	MATH_EMAVG_In1=&Vrect;
	MATH_EMAVG_Out1=&VrectAvg;
	MATH_EMAVG_Multiplier1=_IQ30(0.000030);
	
	// Initialize the net variables
	DutyA =_IQ24(0.0);
	Duty4A =_IQ24(0.0);
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
	Dmax_I  = _IQ24(0.984375); 	
	Pgain_I = _IQ26(0.25);		
	Igain_I = _IQ26(0.06875);	 
	Dgain_I = _IQ26(0.0); 
	
	// Coefficient init	--- Coeeficient values in Q26
	// Use IQ Maths to generate floating point values for the CLA 
	CNTL_2P2Z_CoefStruct1.b2   =Dgain_I;                            // B2
    CNTL_2P2Z_CoefStruct1.b1   =(Igain_I-Pgain_I-Dgain_I-Dgain_I);  // B1
    CNTL_2P2Z_CoefStruct1.b0   =(Pgain_I + Igain_I + Dgain_I);      // B0
    CNTL_2P2Z_CoefStruct1.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct1.max  =Dmax_I;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min 
		
#endif // (INCR_BUILD == 2),  
//--------------------------------------------------------------------------------
//----------------------------------------------------------------------
#if (INCR_BUILD == 3) 	// Closed Current Loop & closed volt loop Bridgeless PFC
//----------------------------------------------------------------------
	// Lib Module connection to "nets" 
	ADCDRV_1ch_Rlt1 = &Ipfc;
	ADCDRV_1ch_Rlt2 = &Vbus;
	ADCDRV_1ch_Rlt3 = &VL_fb;
	ADCDRV_1ch_Rlt4 = &VN_fb;
	//connect the 2P2Z connections, for the inner Current Loop, Loop1
	CNTL_2P2Z_Ref1 = &PFCIcmd;
	CNTL_2P2Z_Out1 = &DutyA; 
	CNTL_2P2Z_Fdbk1= &Ipfc;
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
	// PFC_BL_ICMD block connections
	PFC_BL_ICMD_Vcmd1 = &VbusVcmd;
	PFC_BL_ICMD_VinvSqr1=&VinvSqr;
	PFC_BL_ICMD_VacRect1=&Vrect;
	PFC_BL_ICMD_Out1=&PFCIcmd;
	PFC_BL_ICMD_VmaxOverVmin1=_IQ24(2.00);		//  3.6225 <=>289.8V/80V,
	PFC_BL_ICMD_Vpfc1 = &Vbus;
	PFC_BL_ICMD_Duty1 = &DutyA;
	PFC_BL_ICMD_VoutMaxOverVinMax1 = _IQ24(1.266);//VoutMax=519V, VinMax=409.8V(peak)=289.8Vrms
	
	PWMDRV_1ch_UpDwnCnt_Duty1 = &DutyA;
	PWMDRV_1ch_UpDwnCnt_Duty2 = &DutyA;
	
	// Math_avg block connections - Instance 1
	MATH_EMAVG_In1=&Vrect;
	MATH_EMAVG_Out1=&VrectAvg;
	MATH_EMAVG_Multiplier1=_IQ30(0.000015);
	
	//PFC_BL_ICMD_VmaxOverVmin1=_IQ24(3.5625);		// 3.5625 <=>285V/80V
	//PFC_BL_ICMD_VmaxOverVmin1=_IQ24(5.1225);		// 5.1225 <=>409.8V/80V
	//PFC_BL_ICMD_VmaxOverVmin1=_IQ24(5.5);
	//PFC_BL_ICMD_VmaxOverVmin1=_IQ24(6.15);		// 6.15 <=>409.8V/66.63V
	
	//PWMDRV_1ch_UpDwnCnt_Duty4 = &DutyA;
	//PWMDRV_1ch_UpDwnCnt_Duty4 = &PFCIcmd;
	//PWMDRV_1ch_UpDwnCnt_Duty4 = &PFCIcmd_avg;
	//PWMDRV_1ch_UpDwnCnt_Duty4 = &Vrect;
//	PWMDRV_1ch_UpDwnCnt_Duty4 = &VinvSqr;
	
	// Initialize the net variables
	DutyA =_IQ24(0.0);
	Duty4A =_IQ24(0.0);
	VrectAvg = _IQ24(0.0);
	VbusAvg = _IQ24(0.0);
	VbusAvg2 = _IQ24(0.0);
	VinvSqr = _IQ24(0.0);
	Vrect = _IQ24(0.0);
	PFCIcmd = _IQ24(0.0);
	//PFCIcmd_avg = _IQ24(0.0);//new variable for avg current loop
	VbusVcmd = _IQ24(0.0);
	VbusTarget = _IQ24(0.0);
	VbusTargetSlewed = _IQ24(0.0);
    pfc_slew_temp = 0;
    start_flag = 0;
    run_flag = 0;
    temp_zero = 0;
    
    //--------------------------------------------------------------------------------
    // Coefficients for Inner Current Loop
    //--------------------------------------------------------------------------------
	// PID coefficients & Clamp values - Current loop (Q26), 100kHz Cloop sampling
	
	Dmax_I  = _IQ24(0.984375); //Max output from Cloop controller	
	
	/*
	Pgain_I = _IQ26(0.25);	//Orig=0.25,Tested also with Gain of 0.32. No instability. Gain= 0.55 is unstable at 220V, 10% load 
	Igain_I = _IQ26(0.06875);//	0.06875 
	Dgain_I = _IQ26(0.0); 
	
	Gui_Pgain_I = 250.00;//This GUI value is the actual P gain times 1000.00
	Gui_Igain_I = 68.75;//This GUI value is the actual I gain times 1000.00
	Gui_Dgain_I = 0.0;//This GUI value is the actual D gain times 1000.00
	
	//The following bn, an are calculated from the PID gains above for fs = 100kHz
	//In the GUI an, bn coeff are calculated in Q10 (5.10) format and then pass onto this
	//application program
	b2_Gui = 0; 
	b1_Gui = _IQ10(-0.1813); 
	b0_Gui = _IQ10(0.3188); 
	
	a2_Gui =  _IQ10(0);
	a1_Gui = _IQ10(1.0);
	a0_Gui = _IQ10(1.0);
	*/
	
	
	Pgain_I = _IQ26(0.25);	//Orig=0.25,Tested also with Gain of 0.32. No instability. Gain= 0.55 is unstable at 220V, 10% load 
	Igain_I = _IQ26(0.06875);//	0.06875 
	Dgain_I = _IQ26(0.0); 
	
	Gui_Pgain_I = 250.00;//This GUI value is the actual P gain times 1000.00
	Gui_Igain_I = 68.75;//This GUI value is the actual I gain times 1000.00
	Gui_Dgain_I = 0.0;//This GUI value is the actual D gain times 1000.00
	
	//The following bn, an are calculated from the PID gains above for fs = 100kHz
	//In the GUI an, bn coeff are calculated in Q10 (5.10) format and then pass onto this
	//application program
	b2_Gui = 0; 
	b1_Gui = _IQ10(-0.1813); 
	b0_Gui = _IQ10(0.3188); 
	
	a2_Gui =  _IQ10(0);
	a1_Gui = _IQ10(1.0);
	a0_Gui = _IQ10(1.0);
	
	// Coefficient init	--- Coefficient values in Q26
	// Use IQ Maths to generate floating point values for the CLA 
	
	//bn-coeffs for 150uH Inductor
	/*
	CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0338);//(0.02984);//(0.02401);//(0.01464);  // B2
    CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.6471);//(-0.5416);//(-0.4057);//(-0.2403);  // B1
    CNTL_2P2Z_CoefStruct1.b0   =_IQ26(2.224);//(1.527);//(0.7886);//(0.3774);  // B0
    */
    
    //bn-coeffs for 150uH Inductor
    /*
	CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.0325);// B2
    CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.5772);// B1
    CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.4657);  // B0
    
    CNTL_2P2Z_CoefStruct1.a2   =_IQ26(-0.129);                         // A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.129);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct1.max  =Dmax_I;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min 
	*/


	CNTL_2P2Z_CoefStruct1.b2   =Dgain_I;                            // B2
    CNTL_2P2Z_CoefStruct1.b1   =(Igain_I-Pgain_I-Dgain_I-Dgain_I);  // B1
    CNTL_2P2Z_CoefStruct1.b0   =(Pgain_I + Igain_I + Dgain_I);      // B0
    CNTL_2P2Z_CoefStruct1.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct1.max  =Dmax_I;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min 


//------------------------------------------------------------------------------
// Coefficients for Outer Voltage Loop
//------------------------------------------------------------------------------
	// PID coefficients & Clamp values - Voltage loop (Q26), 50kHz Vloop sampling
	
	Dmax_V  = _IQ24(0.999);//0.99);//Max output from Vloop controller	
	
//	Pgain_V = _IQ26(0.75);//1.75//0.75);	  
//	Igain_V = _IQ26(0.002);//(0.015);//0.0002//(0.0001);
	Pgain_V = _IQ26(0.25);//1.75//0.75);//(P=0.75 & I=0.002 results in THD 4.7% at 110Vin)	  
	Igain_V = _IQ26(0.0005);//(0.015);//0.0002//(0.0001); (P=0.25 & I=0.002 results in THD 4.5% at 110Vin) 
	Dgain_V =_IQ26(0.0);  //(P=0.15 & I=0.001 results in THD 4.5% at 110Vin, 8.5% at 220V)
	//(P=0.10 & I=0.0002 results in THD 4.5% at 110Vin, 8.4% at 220V, but transient deviation hits OVP limit)
			
	
	Gui_Pgain_V = 2500.00;//This GUI value is the actual P gain times 10000.00
	Gui_Igain_V = 5.0;//This GUI value is the actual I gain times 10000.00
	Gui_Dgain_V = 0.0;//This GUI value is the actual D gain times 10000.00			
				
	//Pgain_V	= Gui_Pgain_V*6710.89;		// Q26; Since GUI P gain (V loop) is scaled up by 10000.00, the scaling
										// factor in this case is 2e26/10000 = 6710
										
	//Igain_V	= Gui_Igain_V*6710.89;		// Q26.Since GUI I gain (V loop) is scaled up by 10000.00, the scaling
										// factor in this case is 2e26/10000 = 6710
										
	//Dgain_V	= Gui_Dgain_V*6710.89;		// Q26.Since GUI D gain (V loop) is scaled up by 10000.00, the scaling
										// factor in this case is 2e26/10000 = 6710			
				
				
	// Coefficient init	--- Coeeficient values in Q26
	// Use IQ Maths to generate floating point values for the CLA 
	CNTL_2P2Z_CoefStruct2.b2   =Dgain_V;                            // B2
    CNTL_2P2Z_CoefStruct2.b1   =(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    CNTL_2P2Z_CoefStruct2.b0   =(Pgain_V + Igain_V + Dgain_V);      // B0
    CNTL_2P2Z_CoefStruct2.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct2.a1   =_IQ26(1.0);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct2.max  =Dmax_V;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0); 					  	//Clamp Min   
    		
#endif // (INCR_BUILD == 3),

//--------------------------------------------------------------------------------

//Activate /TZ1 under s/w control to shut off PWM1A, PWM2A under DC bus overvoltage condition
		EALLOW;
		
		EPwm1Regs.TZSEL.bit.OSHT1 = 1;
		EPwm2Regs.TZSEL.bit.OSHT1 = 1;
		EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM1A will go low 
		EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM2A will go low
		
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
    PieVectTable.EPWM1_INT = &DPL_ISR;      	// Map Interrupt
   	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;      	// PIE level enable, Grp3 / Int1, ePWM1

	EPwm1Regs.CMPB = 80;						// ISR trigger point
   	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTRU_CMPB; 	// INT on CompareB-Up event
   	EPwm1Regs.ETSEL.bit.INTEN = 1;              // Enable INT
 //   EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;         // Generate INT on every event
 												
    EPwm1Regs.ETPS.bit.INTPRD = ET_2ND;         // Generate INT on every event
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
	
		if (VbusAvg > VBUS_OVP_THRSHLD)//Check for Vbus OV Condition
		{
		 	OV_flag = 1;	
		 	EALLOW;
		 	EPwm1Regs.TZFRC.bit.OST = 1;//Turn off PWM for OV condition
		 	EPwm2Regs.TZFRC.bit.OST = 1;//Turn off PWM for OV condition
		 	EDIS;	
		 	
		 	VbusTargetSlewed = 0;
		 	VbusTarget = 0;		
		 	Gui_Vbus_set = 0;	 	
		}			
	
	//Calculate RMS input voltage and input frequency
	
	sine_mainsV.Vin = Vrect >> 9; // input in IQ15 format
    SineAnalyzer_MACRO (sine_mainsV);
	VrectRMS = (sine_mainsV.Vrms)<< 9;//    Convert sine_mainsV.Vrms from Q15 to Q24 and save as VrectRMS
	Freq_Vin = sine_mainsV.SigFreq;// Q15
	//VrmsReal = _IQ15mpy (KvInv, sine_mainsV.Vrms);


	//Start of Non-linear Volt loop control
		if (run_flag == 1 && flag_NL_Vloop == 1) //If soft-start is over, PFC running normally & NL Vloop flag is set
		{
		
			error_v = VbusTargetSlewed - Vbus;

			if (error_v > VBUS_ERROR_NL_CNTRL_THRSHLD || ((-1)*(error_v))> VBUS_ERROR_NL_CNTRL_THRSHLD) 
				// (15V/519V)*4095*4095 = 484654.0
				//(12V/519V)*4095*4095 = 387912.0
			{
			CNTL_2P2Z_CoefStruct2.b1   =_IQ26(-4.495);//(-0.7495);//(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // 2.5, 0.005, B1
    		CNTL_2P2Z_CoefStruct2.b0   =_IQ26(4.505);//(0.7505);//(Pgain_V + Igain_V + Dgain_V);      // B0						;//Use NL v loop coefficients
			}
			else
			{
			CNTL_2P2Z_CoefStruct2.b1   =_IQ26(-0.2495);//Otherwise use normal v loop coefficients
			CNTL_2P2Z_CoefStruct2.b0   =_IQ26(0.2505);//KP=0.25, KI=0.0005
			}
		} //End of NL V loop Control


	//Start of Adaptive Current control loop
		if (disable_auto_cloop_coeff_change == 0)
		{
			if (VrectRMS <= 7588716)  
			{
			//(185/409)*2e24 = 7588716,
			//KP=73750
			CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.9722);// B1
    		CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.7097);  // B0
			}
			
			if (VrectRMS > 7588716 && VrectRMS <= 9639721)  
			{
			//(235/409)*2e24 = 9639721,
			//(185/409)*2e24 = 7588716,
			//(225/409)*2e24 = 9229520,Use this lower limit for modified RC filter in CS ckt
			
			//KP=53750
			//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.7085);// B1
    		//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.246);  // B0
    		
    		//KP=63750
			CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.8403);// B1
    		CNTL_2P2Z_CoefStruct1.b0   =_IQ26(1.4778);  // B0
    		
			}
			
			if (VrectRMS > 9639721 && VrectRMS <= 10049922)
			{
			// (235/409)*2e24 = 9639721,
			//(245/409)*2e24 = 10049922,
			 
			 //KP=33750
			//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.4449);// B1
    		//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.7824);  // B0
    		
    		//KP=13750
			CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.1812);// B1
    		CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.3187);  // B0
			}
			
			if (VrectRMS > 10049922 && VrectRMS <= 10255022)
			{
			// (245/409)*2e24 = 10049922,
			//(250/409)*2e24 = 10255022,
			
			 //KP=23750
			//CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.3131);// B1
    		//CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.5506);  // B0
    		
    		//KP=13750
			CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.1812);// B1
    		CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.3187);  // B0
			}
			
			if (VrectRMS > 10255022)
			{
			//(250/409)*2e24 = 10255022,
			 //KP=13750
			CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.1812);// B1
    		CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.3187);  // B0
			}
		}//End of Adaptive Current control loop

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
	// Channel On/Off control

/*	if(ChannelEnable == 1)
		VrefNetBus = Vbus_set;
	else
		VrefNetBus = 0;*/

//OV Protection moved to 10kHz ISR
/*		
	if (VbusAvg > VBUS_OVP_THRSHLD)//Check for Vbus OV Condition
		{
		 	OV_flag = 1;	
		 	EALLOW;
		 	EPwm1Regs.TZFRC.bit.OST = 1;
		 	EPwm2Regs.TZFRC.bit.OST = 1;
		 	EDIS;	
		 	
		 	VbusTargetSlewed = 0;
		 	VbusTarget = 0;		
		 	Gui_Vbus_set = 0;	 	
		}		*/							 	

	
	if (INCR_BUILD == 3)
	{
		
		Pgain_I = Gui_Pgain_I*67108.86;	// Q26. Since GUI P gain (I loop) is scaled up by 1000.00, the scaling
										// factor in this case is 2e26/1000 = 67108.864
										
		Igain_I = Gui_Igain_I*67108.86;	// Q26. Since GUI I gain (I loop) is scaled up by 1000.00, the scaling
										// factor in this case is 2e26/1000 = 67108.864
										
		Dgain_I = Gui_Dgain_I*67108.86;	// Q26. Since GUI D gain (I loop) is scaled up by 1000.00, the scaling
										// factor in this case is 2e26/1000 = 67108.864

		Pgain_V	= Gui_Pgain_V*6710.89;		// Q26; Since GUI P gain (V loop) is scaled up by 10000.00, the scaling
										// factor in this case is 2e26/10000 = 6710.8864
										
		Igain_V	= Gui_Igain_V*6710.89;		// Q26.Since GUI I gain (V loop) is scaled up by 10000.00, the scaling
										// factor in this case is 2e26/10000 = 6710.8864
										
		Dgain_V	= Gui_Dgain_V*6710.89;		// Q26.Since GUI D gain (V loop) is scaled up by 10000.00, the scaling
										// factor in this case is 2e26/10000 = 6710.8864
/*
// Current loop coefficient update
	CNTL_2P2Z_CoefStruct1.b2   = Dgain_I;                           // B2
    CNTL_2P2Z_CoefStruct1.b1   =(Igain_I-Pgain_I-Dgain_I-Dgain_I);  // B1
    CNTL_2P2Z_CoefStruct1.b0   =(Pgain_I + Igain_I + Dgain_I);      // B0
//    CNTL_2P2Z_CoefStruct1.a2   =0.0;                              	// A2 = 0
//    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);                       	// A1 = 1 
//    CNTL_2P2Z_CoefStruct1.max  =Dmax_I;					  		  	//Clamp Hi 
//    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min   
   
*/				
// Update Voltage loop coefficients 
/*	CNTL_2P2Z_CoefStruct2.b2   =Dgain_V;                            // B2
    CNTL_2P2Z_CoefStruct2.b1   =(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    CNTL_2P2Z_CoefStruct2.b0   =(Pgain_V + Igain_V + Dgain_V);      // B0
    CNTL_2P2Z_CoefStruct2.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct2.a1   =_IQ26(1.0);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct2.max  =Dmax_V;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0); 					  	//Clamp Min 
*/	

//Update coefficients for current loop.

		if (coeff_change == 1)
		{
			EPwm1Regs.ETSEL.bit.INTEN = 0;				// Disable INT

			if (pid2p2z_Gui == 0)
			{
			// Current loop coefficient update
				CNTL_2P2Z_CoefStruct1.b2 = Dgain_I;								// B2
				CNTL_2P2Z_CoefStruct1.b1 = (Igain_I - Pgain_I - Dgain_I - Dgain_I);	// B1
				CNTL_2P2Z_CoefStruct1.b0 = (Pgain_I + Igain_I + Dgain_I);			// B0
				CNTL_2P2Z_CoefStruct1.a2 = 0x00000000;							// A2
				CNTL_2P2Z_CoefStruct1.a1 = 0x04000000;							// A1 Corresponds to 1 in Q26
				//	Coef2P2Z_1[5] = Dmax;								// Clamp Hi limit (Q26)
				//	Coef2P2Z_1[6] = 0x00000000;
			}
			else
			{
				CNTL_2P2Z_CoefStruct1.b2 = b2_Gui*65536.0;						// B2 - I5Q10 scaled to I5Q26
				CNTL_2P2Z_CoefStruct1.b1 = b1_Gui*65536.0;						// B1
				CNTL_2P2Z_CoefStruct1.b0 = b0_Gui*65536.0;						// B0
				CNTL_2P2Z_CoefStruct1.a2 = a2_Gui*65536.0;						// A2
				CNTL_2P2Z_CoefStruct1.a1 = a1_Gui*65536.0;						// A1 
				//	Coef2P2Z_1[5] = Dmax;								// Clamp Hi limit (Q26)
				//	Coef2P2Z_1[6] = 0x00000000; 
			}

			EPwm1Regs.ETSEL.bit.INTEN = 1;				// Enable INT

			coeff_change = 0;
			disable_auto_cloop_coeff_change = 1;
			//Disable auto adjustment of Cloop coeff when the user uses 
			//the GUI to change the Cloop coeff
		}
	
	
// Update Voltage loop coefficients 	
			if (vloop_coeff_change == 1)
			{
			EPwm1Regs.ETSEL.bit.INTEN = 0;				// Disable INT

			CNTL_2P2Z_CoefStruct2.b2   = Dgain_V;                            // B2
    		CNTL_2P2Z_CoefStruct2.b1   = (Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    		CNTL_2P2Z_CoefStruct2.b0   = (Pgain_V + Igain_V + Dgain_V);      // B0
    		CNTL_2P2Z_CoefStruct2.a2   = 0x00000000;                              	// A2 = 0
    		CNTL_2P2Z_CoefStruct2.a1   = 0x04000000;//_IQ26(1.0);                       	// A1 = 1 
    		//CNTL_2P2Z_CoefStruct2.max  =Dmax_V;					  		  	//Clamp Hi 
    		//CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0); 					  	//Clamp Min 
	
			EPwm1Regs.ETSEL.bit.INTEN = 1;				// Enable INT

			vloop_coeff_change = 0;
			}
	
	}

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
//	Hist_Ipfc[HistPtr] = IpfcR;
	Hist_Vbus[HistPtr] = VbusR;
	Hist_VrectAvg[HistPtr] = (int16)(VrectAvg>>12);
	//VrectAvg is in Q24. So first make it Q12 and then change it to int16
	
	Hist_VrectRMS[HistPtr] = (int16)(VrectRMS>>12);
	//VrectRMS is in Q24. So first make it Q12 and then change it to int16
	
	Hist_Freq_Vin[HistPtr] = (int16)(Freq_Vin >>12);
	//Freq_Vin is in Q15.
	

	// Measurements
	//----------------------------------------------------------------
	// view following variables in Watch Window as:
	//		Gui_Ipfc = Q??
	//		Gui_Vbus = Q??
	//		Gui_VL_fb = Q??
	//		Gui_VN_fb = Q??


	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_Vbus[i];	//Q12 * 8 = Q15
	Gui_Vbus = ((long)temp_Scratch*(long)K_Vbus) >> 15;	//Q15*Q15 >> 15 = Q15

    temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_VrectAvg[i];	//Q12 * 8 = Q15
	Gui_VrectAvg = ((long)temp_Scratch*(long)K_VrectAvg) >> 15;	//Q15*Q15 >> 15 = Q15
	
	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_VrectRMS[i];	//Q12 * 8 = Q15
	Gui_VrectRMS = ((long)temp_Scratch*(long)K_Vrms) >> 15;	//Q15*Q15 >> 15 = Q15

	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_Freq_Vin[i];	//Q3 * 8 = Q6
		Gui_Freq_Vin = ((long)temp_Scratch);	//Q6

/*	temp_Scratch=0;
	for(i=0; i<HistorySize; i++)
		temp_Scratch = temp_Scratch + Hist_KDCM[i];	//Q12 * 8 = Q15
		Gui_KDCM = ((long)temp_Scratch)>> 3;	//Q15
		*/
		
		Gui_KDCM = KDCM;
		

	//Multiply with longs to get proper result then shift by 14 to turn it back into an int16
	//Vbus_set = ((long)Gui_Vbus_set*(long)iK_Vbus_set) >> 14;
	
	//Vbus_set = ((long)Gui_Vbus_set*(long)iK_Vbus_set)>>5;//Q15*Q14 >> 5 =>Q24
	VbusTarget = ((long)Gui_Vbus_set*(long)iK_Vbus_set)>>5;//Q15*Q14 >> 5 =>Q24
		
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
void C1(void)  // Update Coefficients	 
//------------------------------------------------------
{
	
	if ((INCR_BUILD == 1)||(INCR_BUILD == 2))
	{
		GpioDataRegs.GPASET.bit.GPIO12 = 1;		// Set High to turn the relay ON for BUILD 1 & 2.
	}
	
	if (INCR_BUILD == 3)
	{
		if (VbusTargetSlewed == 0) //
		{
			temp_zero = 0;
			//	temp_handle = 0;
			CNTL_2P2Z_Ref2 = &temp_zero;	// Slewed Voltage Command
//			start_flag = 0;
		}

//		if (start_flag == 1 && Vbus > VBUS_MIN)//Use this to start PFC from CCS watch window using "start_flag"
		if (run_flag == 0 && Vbus > VBUS_MIN)//Use this to start PFC in stand alone mode.
											//Comment this line and uncomment the line above to start PFC from CCS watch window using start_flag
		{
			VbusTargetSlewed = Vbus+ init_boost; 		// Start slewing the boost command from a value slightly greater than the PFC output voltage 
			CNTL_2P2Z_Ref2 = &VbusTargetSlewed;			// Slewed Voltage Command 
			
			//	start_flag = 1;							 	// This flag makes sure above code is executed only once when..
											 	// the VbusTarget command goes from zero to a value > 150V

		//-----------------
		//the next time CpuTimer2 'counter' reaches Period value go to C2
			C_Task_Ptr = &C2;	
		//-----------------
		}
		else	
		{
			//C_Task_Ptr = &C1;
			C_Task_Ptr = &C3;
		}
	}
}
//----------------------------------------
void C2(void) //Slew Rate ("Soft Start")
//----------------------------------------
{
	GpioDataRegs.GPASET.bit.GPIO12 = 1;		// Set High to turn the relay ON 


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
//	if ((-1)*(pfc_slew_temp) >= VbusSlewRate) // Negative Command
//	{
//		VbusTargetSlewed = VbusTargetSlewed - VbusSlewRate;
		VbusTargetSlewed = VBUS_TARGET;
		VbusTarget = VBUS_TARGET;
		Gui_Vbus_set = VBUS_RATED_VOLTS*64;//Q15, Set Gui Vbus set point to initial value VBUS_RATED_VOLTS
		start_flag = 0;
		run_flag = 1;
		C_Task_Ptr = &C3;
//	}
}

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C3
	C_Task_Ptr = &C1;	
	//-----------------
}


//-----------------------------------------
void C3(void) // 
//-----------------------------------------
{
	
	if (run_flag == 1) //If soft-start is over and PFC running normally
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
				//		VbusTargetSlewed = VBUS_TARGET;
				//		start_flag = 0;
			}
		}
		
	}


	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------

}



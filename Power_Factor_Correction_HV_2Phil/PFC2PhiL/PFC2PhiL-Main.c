//----------------------------------------------------------------------------------
//	FILE:			PwrLibTestBench-Main.C
//
//	Description:	Power Library Test Bench 
//
//	Version: 		1.0
//
//  Target:  		TMS320F2802x(PiccoloA 
//
//----------------------------------------------------------------------------------//  Copyright Texas Instruments © 2004-2009
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 18 Jun 2010 - HN
//----------------------------------------------------------------------------------
//
// PLEASE READ - Useful notes about this Project

// Although this project is made up of several files, the most important ones are:
//	 "{ProjectName}-Main.C"	- this file
//		- Application Initialization, Peripheral config,
//		- Application management
//		- Slower background code loops and Task scheduling
//	 "{ProjectName}-DevInit_F28xxx.C
//		- Device Initialization, e.g. Clock, PLL, WD, GPIO mapping
//		- Peripheral clock enables
//		- DevInit file will differ per each F28xxx device series, e.g. F280x, F2833x,
//	 "{ProjectName}-DPL-ISR.asm
//		- Assembly level library Macros and any cycle critical functions are found here
//	 "{ProjectName}-Settings.h"
//		- Global defines (settings) project selections are found here
//		- This file is referenced by both C and ASM files.

// Code is made up of sections, e.g. "FUNCTION PROTOTYPES", "VARIABLE DECLARATIONS" ,..etc
//	each section has FRAMEWORK and USER areas.
//  FRAMEWORK areas provide useful ready made "infrastructure" code which for the most part
//	does not need modification, e.g. Task scheduling, ISR call, GUI interface support,...etc
//  USER areas have functional example code which can be modified by USER to fit their appl.
//
// Code can be compiled with various build options (Incremental Builds IBx), these
//  options are selected in file "{ProjectName}-Settings.h".  Note: "Rebuild All" compile
//  tool bar button must be used if this file is modified.
//----------------------------------------------------------------------------------
#include "PFC2PhiL-Settings.h"
#include "PeripheralHeaderIncludes.h"
#include "DSP2802x_EPWM_defines.h"		
		
#include "DPlib.h"	
#include "IQmathLib.h"

#define DLOG_SIZE 256	// Uncomment for FLASH configuration only
//#define DLOG_SIZE 32	// Uncomment for RAM configuration only

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTION PROTOTYPES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
void A3(void);	//state A3
void A4(void);	//state A4

// B branch states
void B1(void);	//state B1
void B2(void);	//state B2
void B3(void);	//state B3
void B4(void);	//state B4

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
//----------------------------------------------------------------------------------

void DeviceInit(void);
void SCIA_Init();
void SerialHostComms();
#ifdef FLASH		
	void InitFlash();
#endif
void MemCopy();

//-------------------------------- DPLIB --------------------------------------------
void ADC_SOC_CNF(int ChSel[], int Trigsel[], int ACQPS[], int IntChSel, int mode);
void PWM_PFC2PHIL_CNF(int16 n, int16 Period);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - GENERAL
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -------------------------------- FRAMEWORK --------------------------------------

int16	VTimer0[4];					// Virtual Timers slaved off CPU Timer 0
int16	VTimer1[4];					// Virtual Timers slaved off CPU Timer 1
int16	VTimer2[4];					// Virtual Timers slaved off CPU Timer 2
int16	SerialCommsTimer;
int16	CommsOKflg;
int16	HRmode;
int16	BlinkStatePtr, LED_TaskPtr;

// Used for running BackGround in flash, and ISR in RAM
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;
extern Uint16 Cla1funcsLoadStart, Cla1funcsLoadEnd, Cla1funcsRunStart;

// Used for ADC Configuration 
int 	ChSel[16] =   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int		TrigSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int     ACQPS[16] =   {7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};
// ---------------------------------- USER -----------------------------------------
// ---------------------------- DPLIB Net Pointers ---------------------------------
// Declare net pointers that are used to connect the DP Lib Macros  here 

// ADCDRV_4ch 
extern volatile long *ADCDRV_4ch_RltPtrA; 	// instance #0
extern volatile long *ADCDRV_4ch_RltPtrB; 	// instance #1
extern volatile long *ADCDRV_4ch_RltPtrC; 	// instance #2
extern volatile long *ADCDRV_4ch_RltPtrD; 	// instance #3

// CONTROL_2P2Z
extern volatile long *CNTL_2P2Z_Ref1;	// instance #1
extern volatile long *CNTL_2P2Z_Out1;	// instance #1
extern volatile long *CNTL_2P2Z_Fdbk1;	// instance #1
extern volatile long *CNTL_2P2Z_Coef1; 	// instance #1
extern volatile long CNTL_2P2Z_DBUFF1[5];

extern volatile long *CNTL_2P2Z_Ref2;	// instance #2
extern volatile long *CNTL_2P2Z_Out2;	// instance #2
extern volatile long *CNTL_2P2Z_Fdbk2;	// instance #2
extern volatile long *CNTL_2P2Z_Coef2; 	// instance #2
extern volatile long CNTL_2P2Z_DBUFF2[5];

// PWMDRV_PFC2PhiL
extern volatile long *PWMDRV_PFC2PhiL_Duty1;// instance #1
extern volatile long *PWMDRV_PFC2PhiL_Adj1;	// instance #1

// DLOG_4CH
extern volatile long *DLOG_4ch_i1Ptr;
extern volatile long *DLOG_4ch_i2Ptr;
extern volatile long *DLOG_4ch_i3Ptr;
extern volatile long *DLOG_4ch_i4Ptr;
extern volatile int16 *DLOG_4ch_buff1Ptr;
extern volatile int16 *DLOG_4ch_buff2Ptr;
extern volatile int16 *DLOG_4ch_buff3Ptr;
extern volatile int16 *DLOG_4ch_buff4Ptr;
extern volatile long DLOG_4ch_TrigVal;
extern volatile int16 DLOG_4ch_PreScalar;
extern volatile int16 DLOG_4ch_Size; 

//MATH_AVG - instance #1
extern volatile long *MATH_EMAVG_In1;
extern volatile long *MATH_EMAVG_Out1;
extern volatile long MATH_EMAVG_Multiplier1;

//MATH_AVG - instance #2
extern volatile long *MATH_EMAVG_In2;
extern volatile long *MATH_EMAVG_Out2;
extern volatile long MATH_EMAVG_Multiplier2;

//PFC_INVSQR - instance #1
extern volatile long *PFC_INVSQR_In1;
extern volatile long *PFC_INVSQR_Out1;
extern volatile long PFC_INVSQR_VminOverVmax1;
extern volatile long PFC_INVSQR_Vmin1;

//PFC_ICMD - instance #1
extern volatile long *PFC_ICMD_Vcmd1;
extern volatile long *PFC_ICMD_VinvSqr1;
extern volatile long *PFC_ICMD_VacRect1;
extern volatile long *PFC_ICMD_Out1;
extern volatile long PFC_ICMD_VmaxOverVmin1;

// ---------------------------- DPLIB Variables ---------------------------------
// Declare the net variables being used by the DP Lib Macro here 

volatile long PFCShareAdj, PFCDuty; 
volatile long IphA,IphB,IpfcTotal,VpfcOut;
volatile long PFCIcmd, VpfcVcmd;
volatile long VpfcSet;
volatile long VacLineRect,VacLineAvg,InvAvgSqr;
volatile long LineGain;
volatile long VacLineScaled;

volatile long   VpfcSetSlewed = 0;		// Slewed set point for the voltage loop
volatile long   VpfcSlewRate = 25600;	// Voltage loop Slew rate adjustment (Q24)
volatile long   pfc_slew_temp = 0;		// Temp variable: used only if implementing 
										// slew rate control in the slower state machine

volatile long	shoulder = 0;			// Parameter used for adding a "shoulder" around the input voltage zero crossing
volatile long	InvSineComp = 0;
volatile long 	CompAmpltd = 0;
volatile long	InvVac = 32767;
volatile long 	MaxQ24ref = 0x00FFFFFF; // Maximum Q24 number

volatile long	Ipfc_avg = 0;
int16	Input_Power = 0;

int 	init_boost = 10240;		// Small boost command when PFC is enabled the first time

long	temp_handle = 0, temp_zero = 0;

int16 	auto_compensate = 1;
int16	Comp_temp1 = 0; 
int16	Comp_temp2 = 0;
int16	Scale_Factor = 4;
int16	Pwr_mult = 0;
int16	range = 0;

// System Flags
int16	FaultFlg;				// Flag to indicate Overcurrent trip
int16 	start_flag;

// CCS Debug Flags
int16	ClearFaultFlg;			// Command to clear the Overcurrent trip flag
int16	input_good = 0;


#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct1, "CNTL_2P2Z_Coef"); 
#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct2, "CNTL_2P2Z_Coef"); 
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct1;
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct2;

long Pgain_I,Igain_I,Dgain_I,Dmax_I;
long Pgain_V,Igain_V,Dgain_V,Dmax_V;

#pragma DATA_SECTION(DBUFF1,"DLOG_BUFF");
#pragma DATA_SECTION(DBUFF2,"DLOG_BUFF");
#pragma DATA_SECTION(DBUFF3,"DLOG_BUFF");
#pragma DATA_SECTION(DBUFF4,"DLOG_BUFF");
int16 DBUFF1[DLOG_SIZE];
int16 DBUFF2[DLOG_SIZE];
int16 DBUFF3[DLOG_SIZE];
int16 DBUFF4[DLOG_SIZE];

#ifdef FLASH	
int* DLOG_point;
int* DLOG_point_vac;  
int16 Vac_buf2[128];		// Uncomment for FLASH config
int16 Ipfc_buf2[128];		// Uncomment for FLASH config
//int16 Vac_buf2[25];		// Uncomment for RAM config
//int16 Ipfc_buf2[25];		// Uncomment for RAM config
#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - CCS WatchWindow / GUI support
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -------------------------------- FRAMEWORK --------------------------------------

//GUI support variables
// sets a limit on the amount of external GUI controls - increase as necessary
int16 	*varSetTxtList[16];					//16 textbox controlled variables
int16 	*varSetBtnList[16];					//16 button controlled variables
int16 	*varSetSldrList[16];				//16 slider controlled variables
int16 	*varGetList[16];					//16 variables sendable to GUI
int16 	*arrayGetList[16];					//16 arrays sendable to GUI	

// ---------------------------------- USER -----------------------------------------
extern void DacDrvCnf(int16 n, int16 DACval);
void Ramp_Current(void); 

// Monitor ("Get")						// Display as:
int16   Gui_VpfcOut=0;					// Q6
int16   Gui_VacLineAvg=0;				// Q6
int16   Gui_Ipfc1=0;					// Q12
int16   Gui_Ipfc2=0;					// Q12
int16   Gui_IpfcTotal=0;				// Q11
int16	Gui_Ipfc_AVG = 0;				// Q11
int16	Gui_CompAmpltd = 0;
int16	Gui_shoulder = 0;

// Configure ("Set")
int16   Gui_VpfcSet;					// Q6
int16	Gui_SetCompAmpltd = 0;
int16	Gui_Setshoulder = 0;
int16	Gui_dlog_trig = 0;

int16	Gui_Pgain_I = 250, Gui_Igain_I = 69, Gui_Dgain_I = 200, Itrip_level = 29669; 	// Current Loop P I D coefficients and DAC reference
int16	Gui_Pgain_V = 900, Gui_Igain_V = 1, Gui_Dgain_V = 300;							// Voltage Loop P I D coefficients

// History arrays are used for Running Average calculation (boxcar filter)
// Used for CCS display and GUI only, not part of control loop processing
int16	Hist_VpfcOut[HistorySize];			
int16	Hist_VacLineAvg[HistorySize];
int16	Hist_Ipfc1[HistorySize],Hist_Ipfc2[HistorySize];
int16	Hist_IpfcTotal[HistorySize];

//Scaling Constants (values found via spreadsheet (HVPFC2PHIL-Calculations.xls)
int16	K_VpfcOut;						// Q15
int16	K_VacLine;						// Q15
int16	K_Ipfc1;						// Q15
int16	K_Ipfc2;						// Q15
int16	K_IpfcTotal;					// Q15
int16	iK_VpfcSet;						// Q14

// Variables for background support only (no need to access)
int16	i;								// common use incrementer
Uint32	HistPtr, temp_Scratch; 			// Temp here means Temporary

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN CODE - starts here
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void main(void)
{
//=================================================================================
//	INITIALISATION - General
//=================================================================================

	// The DeviceInit() configures the clocks and pin mux registers 
	// The function is declared in {ProjectName}-DevInit_F2803/2x.c,
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
	CpuTimer1Regs.PRD.all =  mSec5;		// B tasks
	CpuTimer2Regs.PRD.all =  mSec0_5;	// C tasks

// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;
	C_Task_Ptr = &C1;

	BlinkStatePtr = 0;
	VTimer0[0] = 0;	
	VTimer1[0] = 0;
	VTimer2[0] = 0;
#ifdef	FLASH
	CommsOKflg = 0;
	SerialCommsTimer = 0;
#endif

// ---------------------------------- USER -----------------------------------------
//  put common initialization/variable definitions here

	for(i=0;i<HistorySize;i++)
	{
		Hist_VpfcOut[i]=0;
		Hist_Ipfc1[i]=0;
		Hist_Ipfc2[i]=0;
		Hist_VacLineAvg[1]=0;
		Hist_IpfcTotal[1]=0;
	}

	HistPtr=0;

#if defined(DSP2802x_DEVICE_H) || defined(DSP2803x_DEVICE_H)
   	K_Ipfc1 	= 20357;
	K_Ipfc2 	= 20357;
	K_IpfcTotal = 20357;
	K_VpfcOut	= 29011; 
	iK_VpfcSet 	= 18506;
	K_VacLine	= 25795;
#endif

//===============================================================================
//	INITIALISATION - GUI connections
//=================================================================================
// Use this section only if you plan to "Instrument" your application using the 
// Microsoft C# freeware GUI Template provided by TI

#ifdef FLASH
	//"Set" variables
	//---------------------------------------
	// assign GUI variable Textboxes to desired "setable" parameter addresses
	varSetTxtList[0] = &Gui_VpfcSet;		// Q6
	varSetTxtList[1] = &Itrip_level;		// Q15
	varSetTxtList[2] = &auto_compensate;	// Q0

	// assign GUI Buttons to desired flag addresses
	varSetBtnList[0] = &ClearFaultFlg;		// -

	// assign GUI Sliders to desired "setable" parameter addresses
	varSetSldrList[0] = &Gui_SetCompAmpltd;	// Q0
	varSetSldrList[1] = &Gui_Setshoulder;	// Q0
	varSetSldrList[2] = &Gui_dlog_trig;		// Q0

	varSetSldrList[3] = &Gui_Pgain_I;		// Q26/67108
	varSetSldrList[4] = &Gui_Igain_I;		// Q26/67108
	varSetSldrList[5] = &Gui_Dgain_I;		// Q26/67108

	varSetSldrList[6] = &Gui_Pgain_V;		// Q26/67108 
	varSetSldrList[7] = &Gui_Igain_V;		// Q26/67108
	varSetSldrList[8] = &Gui_Dgain_V;		// Q26/67108 

	//"Get" variables
	//---------------------------------------
	// assign a GUI "getable" parameter address
	varGetList[0] = &Gui_VacLineAvg;		// Q6
	varGetList[1] = &Gui_IpfcTotal;			// Q11
	varGetList[2] = &Gui_Ipfc1;				// Q12
	varGetList[3] = &Gui_Ipfc2;				// Q12
	varGetList[4] = &Gui_VpfcOut;			// Q6

	varGetList[5] = &Gui_CompAmpltd;		// Q0
	varGetList[6] = &Gui_shoulder;			// Q0

	varGetList[7] = &FaultFlg;				// -
	varGetList[8] = &Input_Power;			// Q2

	// assign a GUI "getable" parameter array address
	arrayGetList[0] = &Ipfc_buf2[0];		// Q11
	arrayGetList[1] = &Vac_buf2[0];			// Q6
#endif //(FLASH)

//==================================================================================
//	INCREMENTAL BUILD OPTIONS - NOTE: selected via {ProjectName-Settings.h}
//==================================================================================
// ---------------------------------- USER -----------------------------------------

//----------------------------------------------------------------------
#if (INCR_BUILD == 1) 	// Open Loop Two Phase Interleaved PFC PWM Driver
//----------------------------------------------------------------------
#define		Ipfc1R		AdcResult.ADCRESULT1	//
#define		Vac_rectR 	AdcResult.ADCRESULT2	//
#define		Ipfc2R	 	AdcResult.ADCRESULT4	//
#define		VpfcOutR	AdcResult.ADCRESULT5	//
	
	// ADC Channel Selection for Configuring the ADC
	//Map channel to ADC Pin
	ChSel[0] =  2;		// A2 - PFC Phase1 current - Dummy read
	ChSel[1] =  2;		// A2 - PFC Phase1 current - IpfcR1
	ChSel[2] =  1;		// A1 - VacLine Rect - Vac_rectR 
	ChSel[3] =  4;		// A4 - PFC Phase2 current- Dummy read
	ChSel[4] =  4;		// A4 - PFC Phase2 current - IpfcR2
	ChSel[5] =  3;		// A3 - PFC O/P Voltage - Vpfc_outR
   
	// Select Trigger Event 
	TrigSel[0] = ADCTRIG_EPWM2_SOCA;		// PFC Phase 1 current sampling triggered by EPWM2 SOCA
	TrigSel[1] = ADCTRIG_EPWM2_SOCA;		// PFC Phase 1 current sampling triggered by EPWM2 SOCA
	TrigSel[2] = ADCTRIG_EPWM2_SOCA;		// VacLine Rect sampling triggered by EPWM2 SOCA
	TrigSel[3] = ADCTRIG_EPWM2_SOCB;		// PFC Phase 2 current sampling triggered by EPWM2 SOCB
	TrigSel[4] = ADCTRIG_EPWM2_SOCB;		// PFC Phase 2 current sampling triggered by EPWM2 SOCB
	TrigSel[5] = ADCTRIG_EPWM2_SOCB;		// PFC O/P voltage sampling triggered by EPWM2 SOCB

	// Configure PWM1 with TBPRD of 300 clock cycles, in master mode
	PWM_PFC2PHIL_CNF(1, 300); // for 200 Khz swicthing frequency
	PWM_PFC2PHIL_CNF(2, 300); // ePWM2 used for ADC Trigger
	
	// Sync PWM2 with PWM1 with a phase shift = time required for one ADC dummy conversion
	EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;
	EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm2Regs.TBPHS.half.TBPHS = 15;
	
	ADC_SOC_CNF(ChSel,TrigSel,ACQPS,15,0);

	// Configure the Start of Conversion for the ADC. ePWM2 is the trigger.
	// At TBCNT1 = 0 convert Ipfc1 and VacLine and TBCNT1 = PRD convert Ipfc2 and Vpfcout
	// At TBCNT1 = 0 SOCA is triggered and at TBCNT1 = PRD SOCB is triggered
	EPwm2Regs.ETSEL.bit.SOCAEN = 1;
	EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;	// Use ZERO event as trigger 
    EPwm2Regs.ETPS.bit.SOCAPRD = ET_2ND;        // Generate pulse on 2nd event 

	EPwm2Regs.ETSEL.bit.SOCBEN = 1;
	EPwm2Regs.ETSEL.bit.SOCBSEL = ET_CTR_PRD;	// Use PRD event as trigger as well
    EPwm2Regs.ETPS.bit.SOCBPRD = ET_2ND;        // Generate pulse on 2nd event
 	
	// Digital Power (DP) library initialisation 
	DPL_Init();
	
	// Lib Module connection to "nets" 
	//----------------------------------------
	// Connect the PWM Driver input to an input variable, Open Loop System
	PWMDRV_PFC2PhiL_Duty1 = &PFCDuty;
	PWMDRV_PFC2PhiL_Adj1  = &PFCShareAdj;
	
	//ADCDRV_4CH block output connections
	ADCDRV_4ch_RltPtrA = &IphA;
	ADCDRV_4ch_RltPtrB = &VacLineRect;
	ADCDRV_4ch_RltPtrC = &IphB;
	ADCDRV_4ch_RltPtrD = &VpfcOut;

	// DLOG block connections 
	DLOG_4ch_i1Ptr =&IpfcTotal;
	DLOG_4ch_i2Ptr =&VacLineRect;
	DLOG_4ch_i3Ptr =&VpfcOut;
	DLOG_4ch_i4Ptr =&PFCDuty;
    	
	DLOG_4ch_buff1Ptr =DBUFF1;
	DLOG_4ch_buff2Ptr =DBUFF2;
	DLOG_4ch_buff3Ptr =DBUFF3;
	DLOG_4ch_buff4Ptr =DBUFF4;
	
	DLOG_4ch_TrigVal = _IQ24(0.1);
#ifdef FLASH
	DLOG_4ch_PreScalar = 16;	//Uncomment for FLASH config. DLOGBUF = 0x100 
#else
	DLOG_4ch_PreScalar = 128;	//Uncomment for RAM config. DLOGBUF changed from 0x100 to 0x20
#endif //(FLASH)

	DLOG_4ch_Size=DLOG_SIZE; 

#ifdef FLASH
 	DLOG_point = DBUFF1;		// Used for GUI comms
	DLOG_point_vac = DBUFF2; 	
#endif	

	// Initialize the net variables
	PFCDuty=_IQ24(0.0);
	PFCShareAdj =_IQ24(0.0);
	IphA=_IQ24(0.0);
	IphB=_IQ24(0.0);
	IpfcTotal=_IQ24(0.0);
#endif // (INCR_BUILD == 1)

//----------------------------------------------------------------------
#if (INCR_BUILD == 2) 	// Complete PFC Build
//----------------------------------------------------------------------
#define		Ipfc1R		AdcResult.ADCRESULT1	//
#define		Vac_rectR 	AdcResult.ADCRESULT2	//
#define		Ipfc2R	 	AdcResult.ADCRESULT4	//
#define		VpfcOutR	AdcResult.ADCRESULT5	//
	
	// ADC Channel Selection for Configuring the ADC
	//Map channel to ADC Pin
	ChSel[0] =  2;		// A2 - PFC Phase1 current - Dummy read
	ChSel[1] =  2;		// A2 - PFC Phase1 current - IpfcR1
	ChSel[2] =  1;		// A1 - VacLine Rect - Vac_rectR 
	ChSel[3] =  4;		// A4 - PFC Phase2 current- Dummy read
	ChSel[4] =  4;		// A4 - PFC Phase2 current - IpfcR2
	ChSel[5] =  3;		// A3 - PFC O/P Voltage - Vpfc_outR
   
	// Select Trigger Event 
	TrigSel[0] = ADCTRIG_EPWM2_SOCA;		// PFC Phase 1 current sampling triggered by EPWM2 SOCA
	TrigSel[1] = ADCTRIG_EPWM2_SOCA;		// PFC Phase 1 current sampling triggered by EPWM2 SOCA
	TrigSel[2] = ADCTRIG_EPWM2_SOCA;		// VacLine Rect sampling triggered by EPWM2 SOCA
	TrigSel[3] = ADCTRIG_EPWM2_SOCB;		// PFC Phase 2 current sampling triggered by EPWM2 SOCB
	TrigSel[4] = ADCTRIG_EPWM2_SOCB;		// PFC Phase 2 current sampling triggered by EPWM2 SOCB
	TrigSel[5] = ADCTRIG_EPWM2_SOCB;		// PFC O/P voltage sampling triggered by EPWM2 SOCB

	// Configure PWM1 with TBPRD of 300 clock cycles, in master mode
	PWM_PFC2PHIL_CNF(1, 300); // for 200 Khz swicthing frequency
	PWM_PFC2PHIL_CNF(2, 300); // ePWM2 used for ADC Trigger
	
	// Sync PWM2 with PWM1 with a phase shift = time required for one ADC dummy conversion
	EPwm2Regs.TBCTL.bit.PHSEN = TB_ENABLE;
	EPwm2Regs.TBCTL.bit.PHSDIR = TB_UP;
	EPwm2Regs.TBPHS.half.TBPHS = 15;
	
	ADC_SOC_CNF(ChSel,TrigSel,ACQPS,15,0); // use auto clr ADC int flag mode

	// Configure the Start of Conversion for the ADC. ePWM2 is the trigger.
	// At TBCNT1 = 0 convert Ipfc1 and VacLine and TBCNT1 = PRD convert Ipfc2 and Vpfcout
	// At TBCNT1 = 0 SOCA is triggered and at TBCNT1 = PRD SOCB is triggered
	EPwm2Regs.ETSEL.bit.SOCAEN = 1;
	EPwm2Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;	// Use ZERO event as trigger 
    EPwm2Regs.ETPS.bit.SOCAPRD = ET_2ND;        // Generate pulse on 2nd event 

	EPwm2Regs.ETSEL.bit.SOCBEN = 1;
	EPwm2Regs.ETSEL.bit.SOCBSEL = ET_CTR_PRD;	// Use PRD event as trigger as well
    EPwm2Regs.ETPS.bit.SOCBPRD = ET_2ND;        // Generate pulse on 2nd event
 
	// Digital Power (DP) library initialisation 
	DPL_Init();
	
	// Lib Module connection to "nets" 
	//----------------------------------------
	// Connect the PWM Driver input to an input variable, Open Loop System
	PWMDRV_PFC2PhiL_Duty1 = &PFCDuty;
	PWMDRV_PFC2PhiL_Adj1  = &PFCShareAdj;

	ADCDRV_4ch_RltPtrA = &IphA;
	ADCDRV_4ch_RltPtrB = &VacLineRect;
	ADCDRV_4ch_RltPtrC = &IphB;
	ADCDRV_4ch_RltPtrD = &VpfcOut;
	
	//connect the 2P2Z connections, for the inner Current Loop
	CNTL_2P2Z_Ref2 = &PFCIcmd;
	CNTL_2P2Z_Out2 = &PFCDuty;
	CNTL_2P2Z_Fdbk2= &IpfcTotal;
	CNTL_2P2Z_Coef2 = &CNTL_2P2Z_CoefStruct2.b2;
	
	//connect the 2P2Z connections, for the outer Voltage Loop
	CNTL_2P2Z_Ref1 = &VpfcSetSlewed;
	CNTL_2P2Z_Out1 = &VpfcVcmd;
	CNTL_2P2Z_Fdbk1= &VpfcOut;
	CNTL_2P2Z_Coef1 = &CNTL_2P2Z_CoefStruct1.b2;
	
	// Math_avg block connections - Instance 1
	MATH_EMAVG_In1=&VacLineScaled;
	MATH_EMAVG_Out1=&VacLineAvg;
	MATH_EMAVG_Multiplier1=_IQ30(0.00030);

	// Math_avg block connections - Instance 2
	MATH_EMAVG_In2=&IpfcTotal;;
	MATH_EMAVG_Out2=&Ipfc_avg;
	MATH_EMAVG_Multiplier2=_IQ30(0.00030);
	
	// INV_SQR block connections 
	PFC_INVSQR_In1=&VacLineAvg;
	PFC_INVSQR_Out1=&InvAvgSqr;
	PFC_INVSQR_VminOverVmax1=_IQ30(0.2807);		// 80V/285V
	PFC_INVSQR_Vmin1=_IQ24(0.17868);			// 80V
	
	// PFC_ICMD block connections
	PFC_ICMD_Vcmd1=&VpfcVcmd;
	PFC_ICMD_VinvSqr1=&InvAvgSqr;
	PFC_ICMD_VacRect1=&VacLineScaled;
	PFC_ICMD_Out1=&PFCIcmd;
	PFC_ICMD_VmaxOverVmin1=_IQ24(3.5625);		// 285V/80V
	
	// DLOG block connections 
	DLOG_4ch_i1Ptr =&IpfcTotal;
	DLOG_4ch_i2Ptr =&VacLineScaled;
	DLOG_4ch_i3Ptr =&PFCIcmd;
	DLOG_4ch_i4Ptr =&PFCDuty;
	
	DLOG_4ch_buff1Ptr =DBUFF1;
	DLOG_4ch_buff2Ptr =DBUFF2;
	DLOG_4ch_buff3Ptr =DBUFF3;
	DLOG_4ch_buff4Ptr =DBUFF4;
	
	DLOG_4ch_TrigVal = _IQ24(0.1);
#ifdef FLASH
	DLOG_4ch_PreScalar = 16;	//Uncomment for FLASH config. DLOGBUF = 0x100 
#else
	DLOG_4ch_PreScalar = 128;	//Uncomment for RAM config. DLOGBUF changed from 0x100 to 0x20
#endif //(FLASH)
	DLOG_4ch_Size=DLOG_SIZE; 

#ifdef FLASH
 	DLOG_point = DBUFF1;						// Used for GUI comms
	DLOG_point_vac = DBUFF2; 	
#endif	
 
    // Initialize the net variables
    
    // Coefficients for Inner Current Loop
	// PID coefficients & Clamping - Current loop (Q26)
	Dmax_I  = _IQ24(0.984375); 	
	Pgain_I = _IQ26(0.25);		
	Igain_I = _IQ26(0.06875);	 
	Dgain_I = _IQ26(0.0); 
	
	// Coefficient init	--- Coeeficient values in Q26
	// Use IQ Maths to generate floating point values for the CLA 
	CNTL_2P2Z_CoefStruct2.b2   =Dgain_I;                            // B2
    CNTL_2P2Z_CoefStruct2.b1   =(Igain_I-Pgain_I-Dgain_I-Dgain_I);  // B1
    CNTL_2P2Z_CoefStruct2.b0   =(Pgain_I + Igain_I + Dgain_I);      // B0
    CNTL_2P2Z_CoefStruct2.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct2.a1   =_IQ26(1.0);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct2.max  =Dmax_I;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0); 					  	//Clamp Min   
    
    // Coefficients for Outer Voltage Loop
	// PID coefficients & Clamping - Current loop (Q26)
	Dmax_V  = _IQ24(0.984375);	
	Pgain_V = _IQ26(0.015625);	  
	Igain_V = _IQ26(0.003906);  
	Dgain_V =_IQ26(0.0);  
				
	// Coefficient init	--- Coeeficient values in Q26
	// Use IQ Maths to generate floating point values for the CLA 
	CNTL_2P2Z_CoefStruct1.b2   =Dgain_V;                            // B2
    CNTL_2P2Z_CoefStruct1.b1   =(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    CNTL_2P2Z_CoefStruct1.b0   =(Pgain_V + Igain_V + Dgain_V);      // B0
    CNTL_2P2Z_CoefStruct1.a2   =0.0;                              	// A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);                       	// A1 = 1 
    CNTL_2P2Z_CoefStruct1.max  =Dmax_V;					  		  	//Clamp Hi 
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min   
    
    
	VpfcSet=_IQ24(0.0);
	VpfcOut=_IQ24(0.0);
	VpfcVcmd=_IQ24(0.0);
	PFCIcmd=_IQ24(0.0);
	PFCDuty=_IQ24(0.0);
	PFCShareAdj =_IQ24(0.0);
	VacLineRect=_IQ24(0.0);
	VacLineAvg=_IQ24(0.0);
	InvAvgSqr=_IQ24(0.0);
	LineGain=_IQ30(453.0/403.0);
	VacLineScaled=_IQ24(0.0);

#endif // (INCR_BUILD == 2)

	// Zero the buffers
	DLOG_BuffInit(DBUFF1, DLOG_SIZE);
	DLOG_BuffInit(DBUFF2, DLOG_SIZE);
	DLOG_BuffInit(DBUFF3, DLOG_SIZE);
	DLOG_BuffInit(DBUFF4, DLOG_SIZE);		

//==============================================================================
// Items common to all builds
//==============================================================================

// 	DACval = 10546 ~ 1.6 A
// 	DACval = 21098 ~ 3.2 A
// 	DACval = 29669 ~ 4.5 A

	DacDrvCnf(1, Itrip_level);			// Comp1, DACval
	DacDrvCnf(2, Itrip_level);			// Comp2, DACval


// Configure the Trip Mechanism for the PFC
	EALLOW;	
//===========================================================================
// Define an event (DCAEVT2) based on Comparator 1 Output
	EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = DC_COMP1OUT; // DCAH = Comparator 1 output
	EPwm1Regs.DCTRIPSEL.bit.DCBHCOMPSEL = DC_COMP2OUT; // DCBH = Comparator 2 output
	EPwm1Regs.TZDCSEL.bit.DCAEVT1 = TZ_DCAH_HI; // DCAEVT1 = DCAH high(will become active
	EPwm1Regs.TZDCSEL.bit.DCBEVT1 = TZ_DCBH_HI; // DCBEVT1 = DCAH high(will become active
												// as Comparator output goes high)
	EPwm1Regs.DCACTL.bit.EVT1SRCSEL = DC_EVT1; // DCAEVT2 = DCAEVT2 (not filtered)
	EPwm1Regs.DCACTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;// Take async path

	EPwm1Regs.DCBCTL.bit.EVT1SRCSEL = DC_EVT1; // DCAEVT2 = DCAEVT2 (not filtered)
	EPwm1Regs.DCBCTL.bit.EVT1FRCSYNCSEL = DC_EVT_ASYNC;// Take async path

// Enable DCAEVT1 and DCBEVT1 as a one-shot source
// Note: DCxEVT1 events can be defined as one-shot.
// DCxEVT2 events can be defined as cycle-by-cycle.
	EPwm1Regs.TZSEL.bit.DCAEVT1 = 1;
	EPwm1Regs.TZSEL.bit.DCBEVT1 = 1;
// What do we want the OST/CBC events to do?
// TZA events can force EPWMxA
// TZB events can force EPWMxB
	EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM1A will go low 
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWM1B will go low 

	EDIS;

	ClearFaultFlg  = 0;
	FaultFlg = 0;

	EPwm1Regs.TZCLR.bit.OST = 1;	// Clear any spurious OC trip

//All enabled ePWM module clocks are started with the first rising edge of 
//TBCLK aligned   
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;

//=================================================================================
//	INTERRUPT & ISR INITIALISATION (best to run this section after other initialisation)
//=================================================================================
//Also Set the appropriate # define's in the {ProjectName}-Settings.h 
//to enable interrupt management in the ISR

	EALLOW;
	PieVectTable.EPWM1_INT = &DPL_ISR;			// Map Interrupt
	EDIS;
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;			// PIE level enable, Grp3 / Int1
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;	// INT on ZERO event
	EPwm1Regs.ETSEL.bit.INTEN = 1; 				// Enable INT
	EPwm1Regs.ETPS.bit.INTPRD = ET_2ND;			// Generate INT on every other event

// Enable Peripheral, global Ints and higher priority real-time debug events:
	IER |= M_INT3; 
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM 

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

		VTimer0[0]++;			// virtual timer 0, instance 0 (spare)
		SerialCommsTimer++;
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

	Alpha_State_Ptr = &A0;		// Back to State A0
}


//=================================================================================
//	A - TASKS
//=================================================================================
//--------------------------------------------------------
void A1(void) // Fault Manaement and Control Coefficient re-calculations
//--------------------------------------------------------
{
	// Fault management
	if ( (*ePWM[1]).TZFLG.bit.OST == 1 )
	{
		FaultFlg = 1;
		Gui_VpfcSet = 0;
		VpfcSetSlewed = 0;
		input_good = 0;
	}
	else FaultFlg = 0;
	
	if ( ClearFaultFlg == 1 )
	{
		EALLOW;
		(*ePWM[1]).TZCLR.bit.OST = 1;	
		EDIS;
		ClearFaultFlg = 0;
	}

	Pgain_I = Gui_Pgain_I*67108;	// Q26
	Igain_I = Gui_Igain_I*67108;	// Q26
	Dgain_I = Gui_Dgain_I*67108;	// Q26

	Pgain_V	= Gui_Pgain_V*67108;		// Q26
	Igain_V	= Gui_Igain_V*67108;		// Q26
	Dgain_V	= Gui_Dgain_V*67108;		// Q26

// Current loop coefficient update
	CNTL_2P2Z_CoefStruct2.b2   = Dgain_I;                           // B2
    CNTL_2P2Z_CoefStruct2.b1   =(Igain_I-Pgain_I-Dgain_I-Dgain_I);  // B1
    CNTL_2P2Z_CoefStruct2.b0   =(Pgain_I + Igain_I + Dgain_I);      // B0
//    CNTL_2P2Z_CoefStruct2.a2   =0.0;                              	// A2 = 0
//    CNTL_2P2Z_CoefStruct2.a1   =_IQ26(1.0);                       	// A1 = 1 
//    CNTL_2P2Z_CoefStruct2.max  =Dmax_I;					  		  	//Clamp Hi 
//    CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0); 					  	//Clamp Min   
   
				
// Voltage loop coefficient update
	CNTL_2P2Z_CoefStruct1.b2   =Dgain_V;                            // B2
    CNTL_2P2Z_CoefStruct1.b1   =(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    CNTL_2P2Z_CoefStruct1.b0   =(Pgain_V + Igain_V + Dgain_V);      // B0
//    CNTL_2P2Z_CoefStruct1.a2   =0.0;                              	// A2 = 0
//    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.0);                       	// A1 = 1 
//    CNTL_2P2Z_CoefStruct1.max  =Dmax_V;					  		  	//Clamp Hi 
//    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0); 					  	//Clamp Min  
	
	Comp1Regs.DACVAL.bit.DACVAL = ((Itrip_level+1985)>>5);	// 1985 corresponds to 0.2V offset (Q15) - Refer to the excel sheet
	Comp2Regs.DACVAL.bit.DACVAL = ((Itrip_level+1985)>>5);	// DAC Value is in Q10

	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2 or A1
#ifdef FLASH
	A_Task_Ptr = A2;
#else
	A_Task_Ptr = A1;
#endif
	//-------------------
}
#ifdef FLASH
//-----------------------------------------------------------------
void A2(void) // SCI GUI
//-----------------------------------------------------------------
{	
		//	EPwm1Regs.ETCLR.bit.INT=0x1;
	//	PieCtrlRegs.PIEACK.bit.ACK3=0x1; 
	SerialHostComms();	// Uncomment for FLASH config
						// Serialport controls LED2 (GPIO-31)
						// Will not blink until GUI is connected 
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;	// To make task A3 active, change &A1 to &A3
	//-------------------
}
//-----------------------------------------
void A3(void)	//Coefficients update
//-----------------------------------------
{
	
	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}


//----------------------------------------------------------
void A4(void) 
//---------------------------------------------------------
{

	
	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}
#endif //(FLASH)

//=================================================================================
//	B - TASKS
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void) // Dashboard Measurements
//----------------------------------------
{	
// Current measurements calculated by:
//	Gui_Ipfc1 = Ipfc1Avg * K_Ipfc1, where Ipfc1Avg = sum of 8 IpfcR1 samples
//	Gui_Ipfc2 = Ipfc2Avg * K_Ipfc2, where Ipfc2Avg = sum of 8 IpfcR2 samples
//	Gui_IpfcTotal = IpfcTotalAvg * K_IpfcTotal, where Ipfc_totalAvg = sum of 8 IpfcTotal calculated values
//----------------------------------------------------------------
// view following variables in Watch Window as:
//		Gui_Ipfc1 = Q12
//		Gui_Ipfc2 = Q12
// 		Gui_IpfcTotal = Q11
// 		Gui_Ipfc_AVG = Q11

	HistPtr++;
	if (HistPtr >= HistorySize)	HistPtr = 0;

	// BoxCar Averages - Input Raw samples into BoxCar arrays
	//----------------------------------------------------------------
	Hist_Ipfc1[HistPtr]   = Ipfc1R;
	Hist_Ipfc2[HistPtr]   = Ipfc2R;
	Hist_VpfcOut[HistPtr] = VpfcOutR;
		
	temp_Scratch=0;
	for(i=0; i<8; i++)	temp_Scratch = temp_Scratch + Hist_Ipfc1[i];
	if (temp_Scratch > 1985)
		Gui_Ipfc1 = ( ((long) temp_Scratch- 1985) * (long) K_Ipfc1 ) >> 15;
	else
		Gui_Ipfc1 = 0;
	
	temp_Scratch=0;
	for(i=0; i<8; i++)	temp_Scratch = temp_Scratch + Hist_Ipfc2[i];
	if (temp_Scratch > 1985)
		Gui_Ipfc2 = ( ((long) temp_Scratch - 1985) * (long) K_Ipfc2 ) >> 15;
	else
		Gui_Ipfc2 = 0;
		
//	Gui_IpfcTotal=(Gui_Ipfc1+Gui_Ipfc2)>>1;
	Hist_IpfcTotal[HistPtr] = IpfcTotal>>12;		// IpfcTotal calculated in the ISR
	temp_Scratch=0;
	for(i=0; i<8; i++)	temp_Scratch = temp_Scratch + Hist_IpfcTotal[i];
	Gui_IpfcTotal = ( (long) temp_Scratch * (long) K_IpfcTotal ) >> 15; // offset adjustment for IpfcTotal already done in the ISR

	temp_Scratch=0;
	temp_Scratch = (Ipfc_avg>>9)*(1.11072);	// To get the RMS value multiply by 1.11072 = (PI/2)/SQRT(2) 
	Gui_Ipfc_AVG = ( (long) temp_Scratch * (long) K_IpfcTotal ) >> 15; 

// Voltage measurement calculated by:
//	Gui_VacLineAvg = VacLineAvg * K_VacLine,
//	Gui_Vpfcout = VpfcoutAvg * K_Vpfcout, where VpfcoutAvg = sum of 8 Vpfc_outR samples
//----------------------------------------------------------------
// view following variables in Watch Window as:
//		Gui_VacLineAvg = Q6
//		Gui_VpfcOut = Q6 
//		Gui_VpfcSet = Q6

	temp_Scratch=0;
	for(i=0; i<8; i++)	temp_Scratch = temp_Scratch + Hist_VpfcOut[i];
	Gui_VpfcOut = ( (long) temp_Scratch * (long) K_VpfcOut ) >> 15;
	
	temp_Scratch=0;
	if (Gui_VpfcSet == 0 && VpfcSetSlewed == 0)
		temp_Scratch = VacLineAvg>>9;				// Avg. value of filtered Input signal (No boost)
	else
		temp_Scratch = (VacLineAvg>>9)*(1.11072);	// To get the RMS value multiply by 1.11072 = (PI/2)/SQRT(2) 
	Gui_VacLineAvg = ( (long) temp_Scratch * (long) K_VacLine ) >> 15;
 
	VpfcSet = ( (long) Gui_VpfcSet * (long) iK_VpfcSet )>>5;
	
	Gui_CompAmpltd = CompAmpltd>>15; //Q15 = Q30>>15
	Gui_shoulder   = shoulder>>9;	 //Q15 = Q24>>9

	DLOG_4ch_TrigVal = (long)(Gui_dlog_trig)<<9;	//Q24 = Q15<<9	
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2 or B1
if (INCR_BUILD == 2)
		//-----------------
		B_Task_Ptr = &B2;	
		//-----------------
else 	B_Task_Ptr = &B1;
}

//----------------------------------------
void B2(void)
//----------------------------------------
{
#ifdef	FLASH
for (temp_Scratch = 0; temp_Scratch <128; temp_Scratch++)				
  {												
	Ipfc_buf2[temp_Scratch] = *(DLOG_point+temp_Scratch*2);		//Sample every 8th data sample
	Vac_buf2[temp_Scratch] = *(DLOG_point_vac+temp_Scratch*2); 	//and store it in a buffer

	Ipfc_buf2[temp_Scratch] = ( (long) Ipfc_buf2[temp_Scratch] * (long) K_IpfcTotal) >> 15;
	Vac_buf2[temp_Scratch] = ( (long) Vac_buf2[temp_Scratch] * (long) K_VacLine) >> 15;
  }
 #endif
Input_Power = ((long) Gui_Ipfc_AVG * (long) Gui_VacLineAvg)>>15; // Q15*Q15>>15	

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B3
	B_Task_Ptr = &B3;
	//-----------------
}

//----------------------------------------
void B3(void)  //  CompAmpltd and shoulder calculation based on Line-Load conditions
//----------------------------------------
{
if (auto_compensate == 1)
{
 switch (range)
	{
	case 0:
			if (Gui_VacLineAvg < 7680) 	// 120V
			{
				Scale_Factor = 1;
				shoulder = 102400;	   	// 102400(Q24) = 200(Q15)
				range = 0;
			}
			else  range = 1;
			break;

	case 1:
			if (Gui_VacLineAvg > 7488 && Gui_VacLineAvg < 8960) // 117V & 140V
			{
				Scale_Factor = 2;
				shoulder = 128000;		// 128000(Q24) = 250(Q15)
				range = 1;
			}
			else
			{
				if (Gui_VacLineAvg > 8960) range = 2;
				else range = 0;
			}
			break; 

	case 2:
			if (Gui_VacLineAvg > 8640 && Gui_VacLineAvg < 11520) // 135V & 180V
			{
				Scale_Factor = 4; 			// 3 or 4??
				shoulder = 153600;		// 153600(Q24) = 300(Q15)
				range = 2;
			}
			else
			{
				if (Gui_VacLineAvg > 11520) range = 3;
				else range = 1;
			}
			break;  

	case 3:
			if (Gui_VacLineAvg > 11200 && Gui_VacLineAvg < 14720) // 175V & 230V
			{
				Scale_Factor = 6;			// 6 or 8??
				shoulder = 204800;		// 204800(Q24) = 400(Q15)
				range = 3;
			}
			else
			{
				if (Gui_VacLineAvg > 14720) range = 4;
				else range = 2;
			}
			break;   

	case 4:
			if (Gui_VacLineAvg > 14400) // 225V
			{
				Scale_Factor = 7;			// 8 or 9??
				shoulder = 256000;			// 256000(Q24) = 500(Q15)
				range = 4;
			}
			else
				range = 3;
			break;   
	}

// Following calculations are done in Q15 for simplicity
	if (Gui_VpfcSet > 19200) // VpfcSet > 300V 
	{
		Comp_temp1 = (1250 - Input_Power)*Scale_Factor;

		if (Comp_temp1 < 0) Comp_temp1 = 0; // Changed This

		Comp_temp2 = (long)Comp_temp1 * ((long)Gui_VacLineAvg - 5440)/12800.0; // (Vacin - 85)/(285-85) --> 12800 = 200*Kscale
	}
	else
		Comp_temp2 = 0;

		CompAmpltd = (long)(Comp_temp2)<<15; //Q30 = Q15<<15

}

else				// if not auto_compensate
{
	CompAmpltd = (long)(Gui_SetCompAmpltd)<<15;	 //Q30 = Q15<<15
	shoulder   = (long)(Gui_Setshoulder)<<9;	 //Q24 = Q15<<9	
}
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;	
	//-----------------
}
#ifdef FLASH
//----------------------------------------
void B4(void) //  SPARE
//----------------------------------------
{
	
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;	
	//-----------------
}
#endif //(FLASH)

//=================================================================================
//	C - TASKS
//=================================================================================

//--------------------------------- USER ------------------------------------------

//------------------------------------------------------
void C1(void) 	 
//------------------------------------------------------
{
if (INCR_BUILD == 2)
{
if (Gui_VpfcSet == 0 && VpfcSetSlewed == 0) //
{
	temp_zero = 0;
	temp_handle = 0;
	CNTL_2P2Z_Ref1 = &temp_zero;	// Slewed Voltage Command
	start_flag = 0;
}

if (VpfcSet > 5551692 && start_flag == 0 && VpfcOut > 2960902)  	
{// Only when VpfcSet Command is > 150V, and Output rectified voltage is > 80V and coming..
// here for the first time (No boost initially).
// From the spreadsheet (HVPFC2PHIL-Calculations.xls), 
// Gui_VpfcOut = 150V = 10843 (Q15) = 10843*2^9 (Q24) = 5551692 
	VpfcSetSlewed = VpfcOut+ init_boost; 		// Start slewing the boost command from a value slightly greater than the PFC output voltage 
	CNTL_2P2Z_Ref1 = &VpfcSetSlewed;			// Slewed Voltage Command 
	start_flag = 1;							 	// This flag makes sure above code is executed only once when..
											 	// the VpfcSet command goes from zero to a value > 150V
}
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C2
	C_Task_Ptr = &C2;	
	//-----------------
}
else	C_Task_Ptr = &C1;
}

//----------------------------------------
void C2(void) 
//----------------------------------------
{
	if (VpfcOut > 5551616 && start_flag == 0 ) // if output voltage >150 V (I/P > 106 rms)
	{
		if (input_good >= 1000)		// Input has been good for 1.5 secs - 1.5 msec time for C2 tasks
			Gui_VpfcSet = 383*64;	// Desired Output * 2^6
		else input_good++;
	}


	if (Gui_VacLineAvg < 5760)	// 90V * 2^6
	{
		Gui_VpfcSet = 0;		// Desired Output * 2^6
		input_good = 0;
		VpfcSetSlewed = 0;
	} 

	Ramp_Current(); 
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C3
	C_Task_Ptr = &C3;	
	//-----------------
}


//-----------------------------------------
void C3(void) // Slew Rate ("Soft Start")
//-----------------------------------------
{
// pfcSlewRate has to be a positive value
pfc_slew_temp = VpfcSet - VpfcSetSlewed;

if (pfc_slew_temp >= VpfcSlewRate) // Positive Command
{
	VpfcSetSlewed = VpfcSetSlewed + VpfcSlewRate;
}
else
{
	if ((-1)*(pfc_slew_temp) >= VpfcSlewRate) // Negative Command
	{
		VpfcSetSlewed = VpfcSetSlewed - VpfcSlewRate;
	}
}

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------
}

#ifdef FLASH
//-----------------------------------------
void C4(void) //  SPARE
//-----------------------------------------
{

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------
}
#endif //(FLASH)
void Ramp_Current()
{
	if (Gui_VpfcSet > 300*64)		// Check if VpfcSet command is greater than 300V
	{
		if (Dmax_I < 16511044)		// 0x00FC0000-4028 = 16511044 so that this value doesn't overflow
		{							// 0x00FC0000 corresponds to 0.984375(Q24)
			Dmax_I = Dmax_I + 4028;
			CNTL_2P2Z_CoefStruct2.max = Dmax_I;
		}
		else
		{
			Dmax_I = 0x00FC0000;
			CNTL_2P2Z_CoefStruct2.max = Dmax_I;
		}		
	}
	else 	// If VpfcSet command is less than 300V re-intialise Max Current loop O/P (Dmax_I) to a low value
		{
				Dmax_I = 0x00100000; 				// 0.0625 in Q24 (Low value for Ramping Current)
				CNTL_2P2Z_CoefStruct2.max = Dmax_I; // 0.0625 in Q24
		}
		
}




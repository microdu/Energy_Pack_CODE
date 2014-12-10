//----------------------------------------------------------------------------------
//	FILE:			{Project Name}-Main.C
//
//	Description:	Works with hardware HVMtrCtrlPfcKit R[1.1]   
//
//  Target:  		TMS320F2803x(PiccoloB), 
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments © 2004-2012
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 
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
//	 "{ProjectName}-Settings.h"
//		- Global defines (settings) project selections are found here
//		- This file is referenced by both C and ASM files.
//	 "{ProjectName}-Shared.h.h"
//		- Variable defines and header includes that are shared b/w CLA and C28x
//   "{ProjectName}-Tasks_C.cla"
//		- CLA C file with the tasks 
//
// Code is made up of sections, e.g. "FUNCTION PROTOTYPES", "VARIABLE DECLARATIONS" ,..etc
//	each section has FRAMEWORK and USER areas.
//  FRAMEWORK areas provide useful ready made "infrastructure" code which for the most part
//	does not need modification, e.g. Task scheduling, ISR call, GUI interface support,...etc
//  USER areas have functional example code which can be modified by USER to fit their appl.
//
// Code can be compiled with various build options (Incremental Builds IBx), these
//  options are selected in file "{ProjectName}-Shared.h".  Note: "Rebuild All" compile
//  tool bar button must be used if this file is modified.
//----------------------------------------------------------------------------------

// Include files for device support, F2803x in this case
#include "PeripheralHeaderIncludes.h"
#include "IQmathLib.h"
#include "HVPM_Sensorless_CLA-Shared_C.h"
#include "DSP2803x_Cla_defines.h"
#include "DSP2803x_EPwm_defines.h"
#include "PWM_3phInv_Cnf.h"
#include "PWM_DAC_Cnf.h"
#include "DPlib.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTION PROTOTYPES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Add protoypes of functions being used in the project here 

void DeviceInit(void);
#ifdef FLASH		
	void InitFlash();
#endif
void MemCopy();
void CLA_C_Init();


// -------------------------------- FRAMEWORK --------------------------------------
// State Machine function prototypes
//----------------------------------------------------------------------------------
void ADC_SOC_CNF(int ChSel[], int Trigsel[], int ACQPS[], int IntChSel, int mode);
void PWM_1ch_CNF(int16 n, int16 period, int16 mode, int16 phase);
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

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - GENERAL
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// -------------------------------- FRAMEWORK --------------------------------------

int16	VTimer0[4];					// Virtual Timers slaved off CPU Timer 0
int16	VTimer1[4];					// Virtual Timers slaved off CPU Timer 1
int16	VTimer2[4];					// Virtual Timers slaved off CPU Timer 2

// Used for running BackGround in flash, and ISR in RAM
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;
// Used for copying CLA code from load location to RUN location 
extern Uint16 Cla1funcsLoadStart, Cla1funcsLoadEnd, Cla1funcsRunStart, Cla1Prog_Start;

// Used for ADC Configuration 
int 	ChSel[16] =   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int		TrigSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int     ACQPS[16] =   {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

//---------------------------------------------------------------------------
// Used to indirectly access all EPWM modules
volatile struct EPWM_REGS *ePWM[] = 
 				  { &EPwm1Regs,			//intentional: (ePWM[0] not used)
				  	&EPwm1Regs,
					&EPwm2Regs,
					&EPwm3Regs,
					&EPwm4Regs,
					&EPwm5Regs,
					&EPwm6Regs,
					&EPwm7Regs,
				  };

// Used to indirectly access all Comparator modules
volatile struct COMP_REGS *Comp[] = 
 				  { &Comp1Regs,			//intentional: (Comp[0] not used)
					&Comp1Regs,				  
					&Comp2Regs,
					&Comp3Regs, 
				  };
// ---------------------------------- USER -----------------------------------------
#define C28x_volatile volatile 

int LedBlinkCnt=4;

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


// Timing sync for background loops
// Timer period definitions found in PeripheralHeaderIncludes.h
	CpuTimer0Regs.PRD.all =  mSec5;		// A tasks
	CpuTimer1Regs.PRD.all =  mSec50;	// B tasks
	CpuTimer2Regs.PRD.all =  mSec1000;	// C tasks

// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;
	C_Task_Ptr = &C1;

	VTimer0[0] = 0;	
	VTimer1[0] = 0;
	VTimer2[0] = 0;
	
	CLA_C_Init();
// ---------------------------------- USER -----------------------------------------
//  put common initialization/variable definitions here

//==================================================================================
//	INCREMENTAL BUILD OPTIONS - NOTE: selected via {ProjectName-Settings.h
//==================================================================================
// ---------------------------------- USER -----------------------------------------

	// Configure PWM for the Inverter, pass period and dead band
	PWM_3phInv_CNF(1,2,3,3000,100)
	
	PWM_DAC_CNF(6,1000);
	PWM_DAC_CNF(7,1000);
	
	// for profiling
	PWM_1ch_CNF(5,10000,1,0);
	
   	// ADC Channel Selection for Configuring the ADC
	// The following configuration would configure the ADC for parameters needed for 
	
	// ADC Channel Selection for Configuring the ADC
	// The following configuration would configure the ADC for parameters needed for 
	
	//Map channel to ADC Pin
	// the dummy reads are to account for first sample issue in Rev 0 silicon
	// Please refer to the Errata and the datasheet, this would be tfixed in later versions of the silicon
    ChSel[0] = 1;						 // A1 - Phase U Current, Dummy
    ChSel[1] = 1;						 // A1 - Phase U Current,   
    ChSel[2] = 9;						 // B1 - Phase V Current
    ChSel[3] = 7;						 // A7 - Vdc Bus 
    
    // Select Trigger Event 
    TrigSel[0]= ADCTRIG_EPWM1_SOCA;
    TrigSel[1]= ADCTRIG_EPWM1_SOCA;
	TrigSel[2]= ADCTRIG_EPWM1_SOCA;
	TrigSel[3]= ADCTRIG_EPWM1_SOCA; 
     
	ADC_SOC_CNF(ChSel,TrigSel,ACQPS,1,2); // use auto clr ADC int flag mode
	
	// Set up Event Trigger with CNT_zero enable for Time-base of EPWM1 SOC						
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;     		// Enable SOCA 										
    EPwm1Regs.ETSEL.bit.SOCASEL = 2;//ET_CTR_ZERO;  // Enable CNT_zero event for SOCA 					
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;     	// Generate SOCA on the 1st event 					
	EPwm1Regs.ETCLR.bit.SOCA = 1;       		// Clear SOCA flag
	 
	// Commands from the C28x to the CLA
	// These variable are in the CPUtoCLAMsgRAM 
	
	lsw=0;
#if (DMC_CLA_BUILDLEVEL<LEVEL3)             // Speed reference (pu)
	SpeedRef=0.15;
#else
	SpeedRef=0.3;
#endif
	
	VqTesting=0.0;
	VdTesting=0.15;
	
	IqRef=0.1;
	IdRef=0.0;
	
	EnableFlag=0;
	
//====================================================================================
// INTERRUPTS & ISR INITIALIZATION (best to run this section after other initialization)
//====================================================================================

// Set Up CLA Task

// Task 1 has the option to be started by either EPWM1_INT or ADCINT1 
// In this case we will allow EPMWM1INT to start CLA Task 1
    EALLOW;
	
	Cla1Regs.MPISRCSEL1.bit.PERINT1SEL = CLA_INT1_ADCINT1 ;		// 0=ADCINT1    1=none    2=EPWM1INT
	
	// Configure the interrupt that woud occur each control cycles
    Cla1Regs.MIER.all = M_INT1;
  
	asm("   RPT #3 || NOP"); 

	EDIS;
   	
   //	Cla1ForceTask1andWait();


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
void A1(void)  // Dash Board Measurements
//--------------------------------------------------------
{

	
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2
	A_Task_Ptr = &A2;
	//-------------------
}

//--------------------------------------------------------
void A2(void)  // Panel Connect Disconnect
//-----------------------------------------------------------------
{	 
	
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A3;
	//-------------------
}

//--------------------------------------------------------
void A3(void)  // Talk to the Panel Emulator
//-----------------------------------------
{	

	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A4;
	//-----------------
}

//--------------------------------------------------------
void A4(void)  // Spare
//--------------------------------------------------------
{
	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}


//=================================================================================
//	B - TASKS
//=================================================================================
//----------------------------------------
void B1(void)  // MPPT Execution 
//----------------------------------------
{	

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2
	B_Task_Ptr = &B2;	
	//-----------------
}

//----------------------------------------
void B2(void) // Blink LED on the control CArd
//----------------------------------------
{
	// Toggle LD3 on control card to show execution of code
	if(LedBlinkCnt==0)
	{
		GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;	//turn on/off LD3 on the controlCARD
		LedBlinkCnt=4;
	}
	else
		LedBlinkCnt--;
			
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B3
	B_Task_Ptr = &B3;
	//-----------------
}

//----------------------------------------
void B3(void) // State Machine, Enable Disable Loops, User Controls  
//----------------------------------------
{

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B4
	B_Task_Ptr = &B1;	
	//-----------------
}

//=================================================================================
//	C - TASKS
//=================================================================================
//------------------------------------------------------
void C1(void) 	 // Spare
//------------------------------------------------------
{	
	
    
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C2
	C_Task_Ptr = &C2;	
	//-----------------

}

//----------------------------------------
void C2(void)   // Update Coefficients of the loops 
//----------------------------------------
{
	
		
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C3
	C_Task_Ptr = &C3;	
	//-----------------
}

//-----------------------------------------
void C3(void)   // SPARE
//-----------------------------------------
{

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C4
	C_Task_Ptr = &C4;	
	//-----------------
}

//-----------------------------------------
void C4(void) //  SPARE
//-----------------------------------------
{
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------
}

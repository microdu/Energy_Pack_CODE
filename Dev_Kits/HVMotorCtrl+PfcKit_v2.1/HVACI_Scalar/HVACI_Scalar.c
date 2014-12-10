/* ==============================================================================
System Name:  	HVACI_Scalar

File Name:	  	HVACI_Scalar.C

Description:	Primary system file for the Scalar Control of Induction Motors  

================================================================================= */

// Include header files used in the main function

#include "PeripheralHeaderIncludes.h"
#define   MATH_TYPE      IQ_MATH
#include "IQmathLib.h"
#include "HVACI_Scalar.h"
#include "HVACI_Scalar-Settings.h"
#include <math.h>

#ifdef FLASH
#pragma CODE_SECTION(MainISR,"ramfuncs");
#pragma CODE_SECTION(OffsetISR,"ramfuncs");
#endif

// Prototype statements for functions found within this file.
interrupt void MainISR(void);
interrupt void OffsetISR(void);
void DeviceInit();
void MemCopy();
void InitFlash();
void HVDMC_Protection(void);

// State Machine function prototypes
//------------------------------------
// Alpha states
void A0(void);	//state A0
void B0(void);	//state B0
void C0(void);	//state C0

// A branch states
void A1(void);	//state A1
void A2(void);	//state A2
void A3(void);	//state A3

// B branch states
void B1(void);	//state B1
void B2(void);	//state B2
void B3(void);	//state B3

// C branch states
void C1(void);	//state C1
void C2(void);	//state C2
void C3(void);	//state C3

// Variable declarations
void (*Alpha_State_Ptr)(void);	// Base States pointer
void (*A_Task_Ptr)(void);		// State pointer A branch
void (*B_Task_Ptr)(void);		// State pointer B branch
void (*C_Task_Ptr)(void);		// State pointer C branch

// Used for running BackGround in flash, and ISR in RAM
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;

int16	VTimer0[4];			// Virtual Timers slaved off CPU Timer 0 (A events)
int16	VTimer1[4]; 		// Virtual Timers slaved off CPU Timer 1 (B events)
int16	VTimer2[4]; 		// Virtual Timers slaved off CPU Timer 2 (C events)
int16	SerialCommsTimer;

// Global variables used in this system

_iq SpeedRef = _IQ(0.3);			// Speed reference (pu)  

float32 T = 0.001/ISR_FREQUENCY;    // Samping period (sec), see parameter.h 

Uint32 IsrTicker = 0;
Uint16 BackTicker = 0;
Uint16 lsw=0;			//Loop Switch 
Uint16 TripFlagDMC=0;	//PWM trip status 

// Default ADC initialization 
int ChSel[16]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int	TrigSel[16] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
int ACQPS[16]   = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

int16 DlogCh1 = 0;
int16 DlogCh2 = 0;
int16 DlogCh3 = 0;
int16 DlogCh4 = 0;

volatile Uint16 EnableFlag = FALSE;

Uint16 SpeedLoopPrescaler = 10;      // Speed loop prescaler   
Uint16 SpeedLoopCount = 1;           // Speed loop counter

Uint16 ClosedLoopFlag = FALSE;

// Instance a QEP interface driver 
QEP qep1 = QEP_DEFAULTS; 

// Instance a Capture interface driver 
CAPTURE cap1 = CAPTURE_DEFAULTS;

// Instance PI regulators to regulate the speed (only, took out d & q)
PI_CONTROLLER pi_spd = PI_CONTROLLER_DEFAULTS;

// Instance a PWM driver instance
PWMGEN pwm1 = PWMGEN_DEFAULTS;

// Instance a PWM DAC driver instance
PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

// Instance a Space Vector PWM modulator. 
SVGENMF svgen_mf1 = SVGENMF_DEFAULTS;

// Instance a V/Hz Scalar
VHZPROF vhz1 = VHZPROF_DEFAULTS;

// Instance a ramp controller to smoothly ramp the frequency
RMPCNTL rc1 = RMPCNTL_DEFAULTS;

// Instance a speed calculator based on QEP
SPEED_MEAS_QEP speed1 = SPEED_MEAS_QEP_DEFAULTS;

// Instance a speed calculator based on capture Qep (for eQep of 280x only)
SPEED_MEAS_CAP speed2 = SPEED_MEAS_CAP_DEFAULTS;

// Create an instance of DATALOG Module
DLOG_4CH dlog = DLOG_4CH_DEFAULTS;      


void main(void)
{
	
	DeviceInit();	// Device Life support & GPIO		

// Only used if running from FLASH
// Note that the variable FLASH is defined by the compiler

#ifdef FLASH
// Copy time critical code and Flash setup code to RAM
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the linker files. 
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
	InitFlash();	// Call the flash wrapper init function
#endif //(FLASH)

   // Waiting for enable flag set
   while (EnableFlag==FALSE) 
    { 
      BackTicker++;
    }

// Timing sync for background loops 
// Timer period definitions found in device specific PeripheralHeaderIncludes.h
	CpuTimer0Regs.PRD.all =  mSec1;		// A tasks
	CpuTimer1Regs.PRD.all =  mSec5;		// B tasks
	CpuTimer2Regs.PRD.all =  mSec50;	// C tasks

// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;
	C_Task_Ptr = &C1;
	

// Initialize PWM module	    
    pwm1.PeriodMax = SYSTEM_FREQUENCY*1000000*T/2;  // Prescaler X1 (T1), ISR period = T x 1
    pwm1.HalfPerMax=pwm1.PeriodMax/2;
    pwm1.Deadband  = 2.0*SYSTEM_FREQUENCY;     	    // 120 counts -> 2.0 usec for TBCLK = SYSCLK/1
    PWM_INIT_MACRO(1,2,3,pwm1)
    
// Initialize PWMDAC module
	pwmdac1.PeriodMax=500;		   	// @60Mhz, 1500->20kHz, 1000-> 30kHz, 500->60kHz
	pwmdac1.HalfPerMax=pwmdac1.PeriodMax/2;
	PWMDAC_INIT_MACRO(6,pwmdac1) 	// PWM 6A,6B
	PWMDAC_INIT_MACRO(7,pwmdac1) 	// PWM 7A,7B 
	
// Initialize DATALOG module
    dlog.iptr1 = &DlogCh1;
    dlog.iptr2 = &DlogCh2;
    dlog.iptr3 = &DlogCh3;
    dlog.iptr4 = &DlogCh4;
    dlog.trig_value = 0x1;
    dlog.size = 0x0C8;
    dlog.prescalar = 5;
    dlog.init(&dlog);

// Initialize ADC for DMC Kit Rev 1.1 	 
	ChSel[0]=1;		// Dummy meas. avoid 1st sample issue Rev0 Picollo*/
	ChSel[1]=1;		// ChSelect: ADC A1-> Phase A Current 
	ChSel[2]=9;		// ChSelect: ADC B1-> Phase B Current 
	ChSel[3]=3;		// ChSelect: ADC A3-> Phase C Current
	ChSel[4]=15;	// ChSelect: ADC B7-> Phase A Voltage
	ChSel[5]=14;	// ChSelect: ADC B6-> Phase B Voltage
	ChSel[6]=12;	// ChSelect: ADC B4-> Phase C Voltage
	ChSel[7]=7;		// ChSelect: ADC A7-> DC Bus  Voltage
	 
	ADC_MACRO_INIT(ChSel,TrigSel,ACQPS)

// Initialize QEP module
    qep1.LineEncoder = 2048;
    qep1.MechScaler = _IQ30(0.25/qep1.LineEncoder);
    qep1.PolePairs = POLES/2;
    qep1.CalibratedAngle = 0;
	QEP_INIT_MACRO(1,qep1)

// Initialize CAP module
	CAP_INIT_MACRO(1)

// Initialize the Speed module for QEP based speed calculation
    speed1.K1 = _IQ21(1/(BASE_FREQ*T));
    speed1.K2 = _IQ(1/(1+T*2*PI*5));  // Low-pass cut-off frequency
    speed1.K3 = _IQ(1)-speed1.K2;
    speed1.BaseRpm = 120*(BASE_FREQ/POLES);

// Initialize the Speed module for capture eQEP based speed calculation (low speed range)
    speed2.InputSelect = 1;
	speed2.BaseRpm 	   = 120*(BASE_FREQ/POLES);
	speed2.SpeedScaler = 60*(SYSTEM_FREQUENCY*1000000/(1*2048*speed2.BaseRpm));
	   	
// Initialize the PI module for speed
    
    pi_spd.Kp=_IQ(2.0);
	pi_spd.Ki=_IQ(T*SpeedLoopPrescaler/0.5);
	pi_spd.Umax =_IQ(0.95);
	pi_spd.Umin =_IQ(-0.95);
	
// Initialize the VHZ_PROF module
	vhz1.LowFreq = _IQ(0.2);
	vhz1.HighFreq = _IQ(0.5);
	vhz1.FreqMax = _IQ(1);
	vhz1.VoltMax = _IQ(0.95);
	vhz1.VoltMin = _IQ(0.2);
	
// Initialize the SVGEN_MF module
 	svgen_mf1.FreqMax = _IQ(6*BASE_FREQ*T);

// Call HVDMC Protection function
	HVDMC_Protection();	

// Reassign ISRs. 

	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.EPWM1_INT = &MainISR;
	EDIS;

// Enable PIE group 3 interrupt 1 for EPWM1_INT
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;

// Enable CNT_zero interrupt using EPWM1 Time-base
    EPwm1Regs.ETSEL.bit.INTEN = 1;   // Enable EPWM1INT generation 
    EPwm1Regs.ETSEL.bit.INTSEL = 1;  // Enable interrupt CNT_zero event
    EPwm1Regs.ETPS.bit.INTPRD = 1;   // Generate interrupt on the 1st event
	EPwm1Regs.ETCLR.bit.INT = 1;     // Enable more interrupts

// Enable CPU INT3 for EPWM1_INT:
	IER |= M_INT3;
// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

// IDLE loop. Just sit and loop forever:	
	for(;;)  //infinite loop
	{
		// State machine entry & exit point
		//===========================================================
		(*Alpha_State_Ptr)();	// jump to an Alpha state (A0,B0,...)
		//===========================================================

	}
} //END MAIN CODE

//=================================================================================
//	STATE-MACHINE SEQUENCING AND SYNCRONIZATION FOR SLOW BACKGROUND TASKS
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

	Alpha_State_Ptr = &A0;	// Back to State A0
}


//=================================================================================
//	A - TASKS (executed in every 1 msec)
//=================================================================================
//--------------------------------------------------------
void A1(void) // SPARE (not used)
//--------------------------------------------------------
{
	   if(EPwm1Regs.TZFLG.bit.OST==0x1)
	   TripFlagDMC=1;      // Trip on DMC (halt and IPM fault trip )
	
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2
	A_Task_Ptr = &A2;
	//-------------------
}

//-----------------------------------------------------------------
void A2(void) // SPARE (not used)
//-----------------------------------------------------------------
{	

	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A3
	A_Task_Ptr = &A3;
	//-------------------
}

//-----------------------------------------
void A3(void) // SPARE (not used)
//-----------------------------------------
{

	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}



//=================================================================================
//	B - TASKS (executed in every 5 msec)
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void) // Toggle GPIO-00
//----------------------------------------
{
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2
	B_Task_Ptr = &B2;	
	//-----------------
}

//----------------------------------------
void B2(void) //  SPARE
//----------------------------------------
{

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B3
	B_Task_Ptr = &B3;
	//-----------------
}

//----------------------------------------
void B3(void) //  SPARE
//----------------------------------------
{

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;	
	//-----------------
}


//=================================================================================
//	C - TASKS (executed in every 50 msec)
//=================================================================================

//--------------------------------- USER ------------------------------------------

//----------------------------------------
void C1(void) 	// Toggle GPIO-34 & GPIO-31
//----------------------------------------
{
	if(EPwm1Regs.TZFLG.bit.OST==0x1)			// TripZ for PWMs is low (fault trip)
	  { TripFlagDMC=1;      				   
	  GpioDataRegs.GPBTOGGLE.bit.GPIO42 = 1;
	  }	
	 
	if(GpioDataRegs.GPADAT.bit.GPIO15 == 1)		// Over Current Prot. for Integrated Power Module is high (fault trip)
	  { TripFlagDMC=1;      				   
	  GpioDataRegs.GPBTOGGLE.bit.GPIO44 = 1;
	  }		
	 	  
	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;	   // Turn on/off LD3 on the controlCARD
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C2
	C_Task_Ptr = &C2;	
	//-----------------

}


//----------------------------------------
void C2(void) //  SPARE
//----------------------------------------
{

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C3
	C_Task_Ptr = &C3;	
	//-----------------
}


//-----------------------------------------
void C3(void) //  SPARE
//-----------------------------------------
{

	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;	
	//-----------------
}


//MainISR 
interrupt void MainISR(void)
{

// Verifying the ISR
    IsrTicker++;

// =============================== LEVEL 1 ======================================
//	  Checks target independent modules, duty cycle waveforms
//	  Keep the motors disconnected at this level	
// ==============================================================================

#if (BUILDLEVEL==LEVEL1)	 

// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_PROF module and call the volt profile macro
// ------------------------------------------------------------------------------	
	vhz1.Freq = SpeedRef;    
	VHZ_PROF_MACRO(vhz1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SVGEN_MF module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
    svgen_mf1.Gain = vhz1.VoltOut;     
    svgen_mf1.Freq = vhz1.Freq;  
    SVGENMF_MACRO(svgen_mf1);
	
// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
    pwm1.MfuncC1 = svgen_mf1.Ta;  
    pwm1.MfuncC2 = svgen_mf1.Tb;   
    pwm1.MfuncC3 = svgen_mf1.Tc; 
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values	

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	pwmdac1.MfuncC1 = svgen_mf1.Ta; 
    pwmdac1.MfuncC2 = svgen_mf1.Tb; 
    PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B
    
    pwmdac1.MfuncC1 = svgen_mf1.Tc; 
    pwmdac1.MfuncC2 = svgen_mf1.Tb-svgen_mf1.Tc; 
	PWMDAC_MACRO(7,pwmdac1)		  					// PWMDAC 7A, 7B  

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
    DlogCh1 = (int16)_IQtoIQ15(svgen_mf1.Ta);
    DlogCh2 = (int16)_IQtoIQ15(svgen_mf1.Tb);
    DlogCh3 = (int16)_IQtoIQ15(svgen_mf1.Tc);
    DlogCh4 = (int16)_IQtoIQ15(svgen_mf1.Tb-svgen_mf1.Tc);

#endif // (BUILDLEVEL==LEVEL1)

// =============================== LEVEL 2 ======================================
//	  Level 2 verifies the open loop control and rotor speed measurement 
// ==============================================================================

#if (BUILDLEVEL==LEVEL2) 

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
    rc1.TargetValue = SpeedRef;		
	RC_MACRO(rc1);

// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_PROF module and call the volt profile macro
// ------------------------------------------------------------------------------	
	vhz1.Freq = rc1.SetpointValue;    
	VHZ_PROF_MACRO(vhz1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SVGEN_MF module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
    svgen_mf1.Gain = vhz1.VoltOut;     
    svgen_mf1.Freq = vhz1.Freq;  
    SVGENMF_MACRO(svgen_mf1);

// ------------------------------------------------------------------------------
//  Call the QEP macro (if incremental encoder used for speed sensing)
//  Connect inputs of the SPEED_FR module and call the speed calculation macro
// ------------------------------------------------------------------------------ 
    QEP_MACRO(1,qep1)

    speed1.ElecTheta = _IQ24toIQ((int32)qep1.ElecTheta);
    speed1.DirectionQep = (int32)(qep1.DirectionQep);
    SPEED_FR_MACRO(speed1)

// ------------------------------------------------------------------------------
//  Call the CAP macro (if sprocket or spur gear used for speed sensing)
//  Connect inputs of the SPEED_PR module and call the speed calculation macro
// ------------------------------------------------------------------------------ 
	CAP_MACRO(1,cap1) 

  	if(cap1.CapReturn ==0)             		   	     // Check the capture return
    {
    speed2.EventPeriod=(int32)(cap1.EventPeriod);    // Read out new event period
    SPEED_PR_MACRO(speed2)                 	    	 // Call the speed macro      
    } 

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
    pwm1.MfuncC1 = svgen_mf1.Ta;  
    pwm1.MfuncC2 = svgen_mf1.Tb;   
    pwm1.MfuncC3 = svgen_mf1.Tc; 
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values	
	    
// ------------------------------------------------------------------------------
//  Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	pwmdac1.MfuncC1 = svgen_mf1.Ta; 
    pwmdac1.MfuncC2 = svgen_mf1.Tb; 
    PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B
    
    pwmdac1.MfuncC1 = svgen_mf1.Tc; 
    pwmdac1.MfuncC2 = svgen_mf1.Tb-svgen_mf1.Tc; 
	PWMDAC_MACRO(7,pwmdac1)		  					// PWMDAC 7A, 7B  
    
// ------------------------------------------------------------------------------
//  Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
    DlogCh1 = (int16)_IQtoIQ15(svgen_mf1.Ta);
    DlogCh2 = (int16)_IQtoIQ15(svgen_mf1.Tb);
    DlogCh3 = (int16)_IQtoIQ15(speed1.Speed);
    DlogCh4 = (int16)_IQtoIQ15(vhz1.VoltOut); 


#endif // (BUILDLEVEL==LEVEL2)

// =============================== LEVEL 3 ======================================
//	Level 3 verifies the speed regulation performed by PI. The system 
//	speed loop is closed by using the measured speed. 
// ============================================================================== 

#if (BUILDLEVEL==LEVEL3)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
    rc1.TargetValue = SpeedRef;		
	RC_MACRO(rc1)
	
// ------------------------------------------------------------------------------
//    Connect inputs of the PI modules and compute the PI speed 
//    controller.
// ------------------------------------------------------------------------------  
    if (SpeedLoopCount == SpeedLoopPrescaler)
     {
      pi_spd.Ref = rc1.SetpointValue;
	  pi_spd.Fbk = speed1.Speed; 
	  PI_MACRO(pi_spd);
      SpeedLoopCount = 1;
     }
    else SpeedLoopCount ++; 
    
// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_PROF module for open-loop or closed-loop
// ------------------------------------------------------------------------------
    if (ClosedLoopFlag==FALSE)
	  {	
	  vhz1.Freq = rc1.SetpointValue;         		   // Open-loop V/f control 
	  pi_spd.ui=0; 
	  pi_spd.i1=0;
	  }
    else
	  vhz1.Freq = rc1.SetpointValue + pi_spd.Out;    // Speed closed-loop V/f control

// ------------------------------------------------------------------------------
//    Call the volt profile macro
// ------------------------------------------------------------------------------
   	VHZ_PROF_MACRO(vhz1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SVGEN_MF module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
    svgen_mf1.Gain = vhz1.VoltOut;     
    svgen_mf1.Freq = vhz1.Freq;  
    SVGENMF_MACRO(svgen_mf1);

// ------------------------------------------------------------------------------
//  Call the QEP macro (if incremental encoder used for speed sensing)
//  Connect inputs of the SPEED_FR module and call the speed calculation macro
// ------------------------------------------------------------------------------ 
    QEP_MACRO(1,qep1)

    speed1.ElecTheta = _IQ24toIQ((int32)qep1.ElecTheta);
    speed1.DirectionQep = (int32)(qep1.DirectionQep);
    SPEED_FR_MACRO(speed1)

// ------------------------------------------------------------------------------
//  Call the CAP macro (if sprocket or spur gear used for speed sensing)
//  Connect inputs of the SPEED_PR module and call the speed calculation macro
// ------------------------------------------------------------------------------ 
	CAP_MACRO(1,cap1) 

  	if(cap1.CapReturn ==0)             		   	     // Check the capture return
    {
    speed2.EventPeriod=(int32)(cap1.EventPeriod);    // Read out new event period
    SPEED_PR_MACRO(speed2)                 	    	 // Call the speed macro      
    } 

// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
//    update function.
// ------------------------------------------------------------------------------
    pwm1.MfuncC1 = svgen_mf1.Ta;  
    pwm1.MfuncC2 = svgen_mf1.Tb;   
    pwm1.MfuncC3 = svgen_mf1.Tc; 
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values	
	    
// ------------------------------------------------------------------------------
//  Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	pwmdac1.MfuncC1 = svgen_mf1.Ta; 
    pwmdac1.MfuncC2 = svgen_mf1.Tb; 
    PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B
    
    pwmdac1.MfuncC1 = svgen_mf1.Tc; 
    pwmdac1.MfuncC2 = svgen_mf1.Tb-svgen_mf1.Tc; 
	PWMDAC_MACRO(7,pwmdac1)		  					// PWMDAC 7A, 7B  
    
// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
    DlogCh1 = (int16)_IQtoIQ15(svgen_mf1.Ta);
    DlogCh2 = (int16)_IQtoIQ15(svgen_mf1.Tb);
    DlogCh3 = (int16)_IQtoIQ15(pi_spd.Ref);
    DlogCh4 = (int16)_IQtoIQ15(pi_spd.Fbk);


#endif // (BUILDLEVEL==LEVEL3)


// ------------------------------------------------------------------------------
//    Call the DATALOG update function.
// ------------------------------------------------------------------------------
    dlog.update(&dlog);

// Enable more interrupts from this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;


}	// ISR Ends Here



void HVDMC_Protection(void)
{

// Configure Trip Mechanism for the Motor control software
// -Cycle by cycle trip on CPU halt
// -One shot IPM trip zone trip 
// These trips need to be repeated for EPWM1 ,2 & 3

//===========================================================================
//Motor Control Trip Config, EPwm1,2,3
//===========================================================================
      EALLOW;
// CPU Halt Trip  
      EPwm1Regs.TZSEL.bit.CBC6=0x1;
      EPwm2Regs.TZSEL.bit.CBC6=0x1;
      EPwm3Regs.TZSEL.bit.CBC6=0x1;
      
      
      EPwm1Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT  
      EPwm2Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT      
      EPwm3Regs.TZSEL.bit.OSHT1   = 1;  //enable TZ1 for OSHT

// What do we want the OST/CBC events to do?
// TZA events can force EPWMxA
// TZB events can force EPWMxB

      EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low 
      EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
      
      EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low 
      EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
      
      EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low 
      EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
            
      EDIS;

     // Clear any spurious OV trip
      EPwm1Regs.TZCLR.bit.OST = 1;
      EPwm2Regs.TZCLR.bit.OST = 1;
      EPwm3Regs.TZCLR.bit.OST = 1;  
      
}//End of protection configuration

//===========================================================================
// No more.
//===========================================================================

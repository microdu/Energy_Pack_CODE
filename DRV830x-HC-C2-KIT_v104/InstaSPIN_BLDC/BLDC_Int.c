/* ==============================================================================
System Name:  	BLDC_Int

File Name:	  	BLDC_Int.c

Description:	Primary system file for the Real Implementation of Sensorless  
          		Trapeziodal Control of Brushless DC Motors (BLDC) using BEMF Integration

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DRV8312-EVM. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 06-08-2011	Version 1.0:  F2806x or F2803x target
===================================================================================*/

// Include header files used in the main function

#include "PeripheralHeaderIncludes.h"
#include "BLDC_Int-Settings.h"
#include "IQmathLib.h"
#include "BLDC_Int.h"
#include <math.h>

#ifdef DRV8301
union DRV8301_STATUS_REG_1 DRV8301_stat_reg1;
union DRV8301_STATUS_REG_2 DRV8301_stat_reg2;
union DRV8301_CONTROL_REG_1 DRV8301_cntrl_reg1;
union DRV8301_CONTROL_REG_2 DRV8301_cntrl_reg2;
Uint16 read_drv_status = 0;
#endif

#ifdef FLASH
#pragma CODE_SECTION(MainISR,"ramfuncs");
void MemCopy();
void InitFlash();
#endif

// Prototype statements for functions found within this file.
interrupt void MainISR(void);
void DeviceInit();

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

float32 T = 0.001/ISR_FREQUENCY;    // Samping period (sec), see parameter.h 

Uint32 IsrTicker = 0;
Uint16 BackTicker = 0;

Uint16 PreviousState;

Uint32 VirtualTimer = 0;
Uint16 SpeedLoopFlag = FALSE;
_iq  DfuncTesting = _IQ(0.3);
_iq  DfuncStartup = _IQ(0.1);
_iq CurrentStartup = _IQ(0.1);

Uint32 CmtnPeriodTarget = RAMP_END_RATE;
Uint32 CmtnPeriodSetpt = RAMP_START_RATE;
Uint32 RampDelay = 1;

_iq SpeedRef=_IQ(0.3);
#if defined(DRV8312)
_iq IRef = _IQ(0.1);
#endif
#if defined(DRV8301) || defined(DRV8302)
_iq IRef = _IQ(0.04);
#endif

int16 PwmDacCh1 = 0;
int16 PwmDacCh2 = 0;
int16 PwmDacCh3 = 0;
#if defined(DRV8301) || defined(DRV8302)
int16 PwmDacCh4 = 0;
#endif

int16 DlogCh1 = 0;
int16 DlogCh2 = 0;
int16 DlogCh3 = 0;
int16 DlogCh4 = 0;

//#if (BUILDLEVEL==LEVEL1)	 
//Uint16 DRV_RESET = 1;
//#else
Uint16 DRV_RESET = 0;
//#endif

volatile Uint16 EnableFlag = FALSE;
Uint16 RunMotor = FALSE;

_iq BemfA_offset = _IQ(0.0070);	//modify offset after calibration step
_iq BemfB_offset = _IQ(0.0072);	//modify offset after calibration step
_iq BemfC_offset = _IQ(0.0063);	//modify offset after calibration step
_iq IDC_offset = _IQ(0.5041);	//modify offset after calibration step
_iq cal_filt_gain;	 

// Instance PID regulator to regulate the DC-bus current and speed
PID_GRANDO_CONTROLLER pid1_idc = {PID_TERM_DEFAULTS,PID_PARAM_DEFAULTS,PID_DATA_DEFAULTS};
PID_GRANDO_CONTROLLER pid1_spd = {PID_TERM_DEFAULTS,PID_PARAM_DEFAULTS,PID_DATA_DEFAULTS};

// Instance a PWM driver instance
PWM_CNTL pwmcntl1 = PWM_CNTL_DEFAULTS;

// Instance a PWM DAC driver instance
PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

// Instance a ramp controller to smoothly ramp the frequency
RMPCNTL rc1 = RMPCNTL_DEFAULTS;

// Instance a RAMP3 Module
RMP3 rmp3 = RMP3_DEFAULTS;

// Instance a MOD6 Module
MOD6CNTDIR mod_dir1 = MOD6CNTDIR_DEFAULTS;

// Instance a IMPULSE Module
IMPULSE impl1 = IMPULSE_DEFAULTS;

//Instance an InstaSPIN_BLDC Module
INSTASPIN_BLDC InstaSPIN_BLDC1 = INSTASPIN_BLDC_DEFAULTS;

// Instance a SPEED_PR Module
SPEED_MEAS_CAP speed1 = SPEED_MEAS_CAP_DEFAULTS;

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


// Timing sync for background loops 
// Timer period definitions found in device specific PeripheralHeaderIncludes.h
	CpuTimer1Regs.PRD.all =  mSec1;		// A tasks
	CpuTimer2Regs.PRD.all =  mSec5;		// B tasks

// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;

   // Waiting for enable flag set
   while (EnableFlag==FALSE) 
    { 
      BackTicker++;
    }

// Initialize all the Device Peripherals:
// This function is found in DSP280x_CpuTimers.c
   InitCpuTimers();
// Configure CPU-Timer 0 to interrupt every ISR Period:
// 60MHz CPU Freq, ISR Period (in uSeconds)
// This function is found in DSP280x_CpuTimers.c
   ConfigCpuTimer(&CpuTimer0, SYSTEM_FREQUENCY, 1000/ISR_FREQUENCY);
   StartCpuTimer0();

#ifdef DRV8301
// Initialize SPI for communication to the DRV8301
	DRV8301_SPI_Init(&SpibRegs);
#endif
	
// Reassign ISRs. 
// Reassign the PIE vector for TINT0 to point to a different 
// ISR then the shell routine found in DSP280x_DefaultIsr.c.
// This is done if the user does not want to use the shell ISR routine
// but instead wants to use their own ISR.

	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.ADCINT1 = &MainISR;
	EDIS;   // This is needed to disable write to EALLOW protected registers

// Enable PIE group 1 interrupt 1 for ADCINT1
    PieCtrlRegs.PIEIER1.all = M_INT1;

// Enable CPU INT1 for ADCINT1:
	IER |= M_INT1;
	// Enable Global realtime interrupt DBGM

// Initialize the PWM control module
	pwmcntl1.PWMprd = SYSTEM_FREQUENCY*1000000*T/2;;				// Set the pwm period for this example
    PWM_CNTL_INIT_MACRO(pwmcntl1)
    PWM_CNTL_MACRO(pwmcntl1)

// Initialize PWMDAC module
	pwmdac1.PeriodMax = 500;   // 3000->10kHz, 1500->20kHz, 1000-> 30kHz, 500->60kHz
    pwmdac1.PwmDacInPointer0 = &PwmDacCh1;
    pwmdac1.PwmDacInPointer1 = &PwmDacCh2;
    pwmdac1.PwmDacInPointer2 = &PwmDacCh3;
#if defined(DRV8301) || defined(DRV8302)
    pwmdac1.PwmDacInPointer3 = &PwmDacCh4;
#endif
	PWMDAC_INIT_MACRO(pwmdac1)  


// Initialize DATALOG module
    dlog.iptr1 = &DlogCh1;
    dlog.iptr2 = &DlogCh2;
    dlog.iptr3 = &DlogCh3;
    dlog.iptr4 = &DlogCh4;
    dlog.trig_value = 0x1;
    dlog.size = 0x0C8;
#if (BUILDLEVEL < 5)
    dlog.prescalar = 25;
#else
    dlog.prescalar = 1;
#endif
    dlog.init(&dlog);

// Initialize ADC module (F2803XIDC_VEMF.H)
	ADC_MACRO_INIT() 

 // Initialize the SPEED_PR module 
 	speed1.InputSelect = 0;
 	speed1.BaseRpm = 120*(BASE_FREQ/POLES);
 	speed1.SpeedScaler = (Uint32)(ISR_FREQUENCY/(1*BASE_FREQ*0.001));

// Initialize RMPCNTL module
    rc1.RampDelayMax = 1;
    rc1.RampLowLimit = _IQ(-1.0);
    rc1.RampHighLimit = _IQ(1.0);

// Initialize RMP3 module
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = RampDelay;
    rmp3.Out = CmtnPeriodSetpt;
    rmp3.Ramp3Min = CmtnPeriodTarget;

//Initialize the INSTASPIN_BLDC module
	InstaSPIN_BLDC1.VaOffset = BemfA_offset;	//modify offset after calibration step
	InstaSPIN_BLDC1.VbOffset = BemfB_offset;	//modify offset after calibration step
	InstaSPIN_BLDC1.VcOffset = BemfC_offset;	//modify offset after calibration step
	InstaSPIN_BLDC1.Int_Threshold = _IQ(1.0);

// Initialize the PID_GRANDO_CONTROLLER module for dc-bus current
#if defined(DRV8312)
    pid1_idc.param.Kp = _IQ(0.104);
#endif
#if defined(DRV8301) || defined (DRV8302)
    pid1_idc.param.Kp = _IQ(0.25);
#endif
    pid1_idc.param.Kr = _IQ(1.0);
	pid1_idc.param.Ki = _IQ(T/0.05);
	pid1_idc.param.Kd = _IQ(0/T);
    pid1_idc.param.Km = _IQ(1.0);
    pid1_idc.param.Umax = _IQ(0.95);
    pid1_idc.param.Umin = _IQ(-0.95);

// Initialize the PID_GRANDO_CONTROLLER module for Speed 
    pid1_spd.param.Kp = _IQ(0.75);
    pid1_spd.param.Kr = _IQ(1.0);
	pid1_spd.param.Ki = _IQ(T/0.3);
	pid1_spd.param.Kd = _IQ(0/(T));
    pid1_spd.param.Km = _IQ(1.0);
    pid1_spd.param.Umax = _IQ(0.95);
    pid1_spd.param.Umin = _IQ(-0.95);
    
// Initialize the current offset calibration filter
	cal_filt_gain = _IQ(T/(T+TC_CAL));

// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM


// IDLE loop. Just sit and loop forever:	
	for(;;)  //infinite loop
	{
		BackTicker++;

		// State machine entry & exit point
		//===========================================================
		(*Alpha_State_Ptr)();	// jump to an Alpha state (A0,B0,...)
		//===========================================================

#ifdef DRV8301
		//read the status registers from the DRV8301
		if(read_drv_status)
		{
			DRV8301_stat_reg1.all = DRV8301_SPI_Read(&SpibRegs,STAT_REG_1_ADDR);
			DRV8301_stat_reg2.all = DRV8301_SPI_Read(&SpibRegs,STAT_REG_2_ADDR); 
			read_drv_status = 0;
		}
#endif		
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

	Alpha_State_Ptr = &A0;		// Allow C state tasks
}

//=================================================================================
//	A - TASKS (executed in every 1 msec)
//=================================================================================
//--------------------------------------------------------
void A1(void) // SPARE (not used)
//--------------------------------------------------------
{
	if (EnableFlag == FALSE)
	{
#if defined (DRV8312)
		PHASE_A_OFF;
		PHASE_B_OFF;
		PHASE_C_OFF;
#endif
#if defined(DRV8301) || defined(DRV8302)
		//de-assert the DRV830x EN_GATE pin
		GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
#endif

		RunMotor = FALSE;
		
		EALLOW;
	 	EPwm1Regs.TZFRC.bit.OST=1;
		EPwm2Regs.TZFRC.bit.OST=1;
		EPwm3Regs.TZFRC.bit.OST=1;
	 	EDIS;
	}
	else if((EnableFlag == TRUE) && (RunMotor == FALSE))
	{
#ifdef DRV8302
#if DRV_GAIN == 10
		GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;	// GAIN = 10
#elif DRV_GAIN == 40
		GpioDataRegs.GPASET.bit.GPIO25 = 1;		// GAIN = 40
#else
#error  Invalid GAIN setting for DRV8302!!
#endif
		//GpioDataRegs.GPACLEAR.bit.GPIO24 = 1;	// M_OC - cycle by cycle current limit
		GpioDataRegs.GPASET.bit.GPIO24 = 1;		// M_OC - fault on OC
#endif
		//if we want the power stage active we need to enable the DRV830x
		//and configure it.
		if(DRV_RESET == 0)
		{
#if defined(DRV8301) || defined(DRV8302)
			//assert the DRV830x EN_GATE pin
			GpioDataRegs.GPBSET.bit.GPIO39 = 1;

			DELAY_US(50000);		//delay to allow DRV830x supplies to ramp up
			
#ifdef DRV8301
			DRV8301_cntrl_reg1.bit.GATE_CURRENT = 0;		// full current 1.7A
//			DRV8301_cntrl_reg1.bit.GATE_CURRENT = 1;		// med current 0.7A
//			DRV8301_cntrl_reg1.bit.GATE_CURRENT = 2;		// min current 0.25A
			DRV8301_cntrl_reg1.bit.GATE_RESET = 0;			// Normal Mode
			DRV8301_cntrl_reg1.bit.PWM_MODE = 0;			// six independant PWMs
//			DRV8301_cntrl_reg1.bit.OC_MODE = 0;				// current limiting when OC detected
			DRV8301_cntrl_reg1.bit.OC_MODE = 1;				// latched OC shutdown
//			DRV8301_cntrl_reg1.bit.OC_MODE = 2;				// Report on OCTWn pin and SPI reg only, no shut-down
//			DRV8301_cntrl_reg1.bit.OC_MODE = 3;				// OC protection disabled
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 0;			// OC @ Vds=0.060V
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 4;			// OC @ Vds=0.097V
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 6;			// OC @ Vds=0.123V
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 9;			// OC @ Vds=0.175V
			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 15;			// OC @ Vds=0.358V
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 16;			// OC @ Vds=0.403V
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 17;			// OC @ Vds=0.454V
//			DRV8301_cntrl_reg1.bit.OC_ADJ_SET = 18;			// OC @ Vds=0.511V
			DRV8301_cntrl_reg1.bit.Reserved = 0;
			
//			DRV8301_cntrl_reg2.bit.OCTW_SET = 0;			// report OT and OC
			DRV8301_cntrl_reg2.bit.OCTW_SET = 1;			// report OT only
#if DRV_GAIN == 10
			DRV8301_cntrl_reg2.bit.GAIN = 0;				// CS amplifier gain = 10
#elif DRV_GAIN == 20
			DRV8301_cntrl_reg2.bit.GAIN = 1;				// CS amplifier gain = 20
#elif DRV_GAIN == 40
			DRV8301_cntrl_reg2.bit.GAIN = 2;				// CS amplifier gain = 40
#elif DRV_GAIN == 80
			DRV8301_cntrl_reg2.bit.GAIN = 3;				// CS amplifier gain = 80
#endif
			DRV8301_cntrl_reg2.bit.DC_CAL_CH1 = 0;			// not in CS calibrate mode
			DRV8301_cntrl_reg2.bit.DC_CAL_CH2 = 0;			// not in CS calibrate mode
			DRV8301_cntrl_reg2.bit.OC_TOFF = 0;				// normal mode
			DRV8301_cntrl_reg2.bit.Reserved = 0;
			
			//write to DRV8301 control register 1, returns status register 1 
			DRV8301_stat_reg1.all = DRV8301_SPI_Write(&SpibRegs,CNTRL_REG_1_ADDR,DRV8301_cntrl_reg1.all);
			//write to DRV8301 control register 2, returns status register 1
			DRV8301_stat_reg1.all = DRV8301_SPI_Write(&SpibRegs,CNTRL_REG_2_ADDR,DRV8301_cntrl_reg2.all);
#endif
#endif
		}
		

		speed1.InputSelect = 0;
		speed1.NewTimeStamp = 0;
		speed1.OldTimeStamp = 0;
		speed1.EventPeriod = 0;
		speed1.Speed = 0;
		VirtualTimer = 0;
		
		rc1.EqualFlag = 0;
		rc1.RampDelayCount = 0;
		rc1.TargetValue = 0;
		if(DfuncTesting > _IQ(0.0))
		{
			mod_dir1.CntDirection = _IQ(1.0);
		}
		else
		{
			mod_dir1.CntDirection = _IQ(-1.0);
		}
		rc1.SetpointValue = _IQmpy(DfuncStartup,mod_dir1.CntDirection);
		
		rmp3.DesiredInput = CmtnPeriodTarget;
    	rmp3.Out = CmtnPeriodSetpt;
		rmp3.Ramp3DelayCount = 0;
		rmp3.Ramp3DoneFlag = 0; 

		impl1.Counter = 0;
		impl1.Out = 0;
		
		mod_dir1.Counter = 0;
		mod_dir1.TrigInput = 0;
		
		SpeedLoopFlag = FALSE;
		
		pid1_idc.data.d1 = 0;
		pid1_idc.data.d2 = 0;
		pid1_idc.data.i1 = 0;
		pid1_idc.data.ud = 0;
		pid1_idc.data.ui = 0;
		pid1_idc.data.up = 0;
		pid1_idc.data.v1 = 0;
		pid1_idc.data.w1 = 0;
		pid1_idc.term.Out = 0;

		pid1_spd.data.d1 = 0;
		pid1_spd.data.d2 = 0;
		pid1_spd.data.i1 = 0;
		pid1_spd.data.ud = 0;
		pid1_spd.data.ui = 0;
		pid1_spd.data.up = 0;
		pid1_spd.data.v1 = 0;
		pid1_spd.data.w1 = 0;
		pid1_spd.term.Out = 0;
		
		RunMotor = TRUE;
			
		EALLOW;
			EPwm1Regs.TZCLR.bit.OST=1;
			EPwm2Regs.TZCLR.bit.OST=1;
			EPwm3Regs.TZCLR.bit.OST=1;
		EDIS;
	}

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
	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
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

// ==============================================================================
// =============================== MAIN ISR =====================================
// ==============================================================================


interrupt void MainISR(void)
{
// Verifying the ISR
    IsrTicker++;

	if(RunMotor)
	{
// =============================== LEVEL 1 ======================================
// ============================================================================== 

#if (BUILDLEVEL==LEVEL1)	 

	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));

	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	
// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the RAMP Control 3 macro.
// ------------------------------------------------------------------------------
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = RampDelay;
	RC3_MACRO(rmp3)

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
	impl1.Period = rmp3.Out; 
	IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Mod 6 counter macro.
// ------------------------------------------------------------------------------      
	mod_dir1.TrigInput = impl1.Out;
	MOD6CNTDIR_MACRO(mod_dir1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation 
//	  update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
	pwmcntl1.Duty = DfuncStartup;
	PWM_CNTL_MACRO(pwmcntl1)
	  
// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.Vag); 
	PwmDacCh3 = _IQtoQ15(InstaSPIN_BLDC1.Vbg);
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vcg);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
	DlogCh3 = _IQtoQ15(InstaSPIN_BLDC1.Vbg); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vcg);

#endif // (BUILDLEVEL==LEVEL1)

// =============================== LEVEL 2 ======================================
// ==============================================================================

#if (BUILDLEVEL==LEVEL2) 

	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));

	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	
// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
// ------------------------------------------------------------------------------
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = RampDelay;
	RC3_MACRO(rmp3)	

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
	impl1.Period = rmp3.Out; 
	IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(DfuncStartup > _IQ(0.0))
	{
		mod_dir1.CntDirection = _IQ(1.0);
	}
	else
	{
		mod_dir1.CntDirection = _IQ(-1.0);
	}
	mod_dir1.TrigInput = impl1.Out;
	MOD6CNTDIR_MACRO(mod_dir1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation
//    update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
	pwmcntl1.Duty = DfuncStartup;
	PWM_CNTL_MACRO(pwmcntl1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.Vag); 
	PwmDacCh3 = _IQtoQ15(InstaSPIN_BLDC1.Vbg);
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vcg);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
	DlogCh3 = _IQtoQ15(InstaSPIN_BLDC1.Vbg); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vcg);

#endif // (BUILDLEVEL==LEVEL2)

// =============================== LEVEL 3 ======================================
// ============================================================================== 

#if (BUILDLEVEL==LEVEL3) 

	_iq IDCfdbk;
	_iq VsenseA;
	_iq VsenseB;
	_iq VsenseC;
	
// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	VsenseA =  _IQ15toIQ((AdcResult.ADCRESULT1<<3))-BemfA_offset;
	VsenseB =  _IQ15toIQ((AdcResult.ADCRESULT2<<3))-BemfB_offset; 
	VsenseC =  _IQ15toIQ((AdcResult.ADCRESULT3<<3))-BemfC_offset;
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1;
// ------------------------------------------------------------------------------
//  LPF to average the calibration offsets
//  Use the offsets calculated here to initialize BemfA_offset, BemfB_offset
//  and BemfC_offset so that they are used for the remaining build levels
// ------------------------------------------------------------------------------
    BemfA_offset = _IQmpy(cal_filt_gain,VsenseA) + BemfA_offset;
    BemfB_offset = _IQmpy(cal_filt_gain,VsenseB) + BemfB_offset;
    BemfC_offset = _IQmpy(cal_filt_gain,VsenseC) + BemfC_offset;
    IDC_offset = _IQmpy(cal_filt_gain,IDCfdbk) + IDC_offset;

// ------------------------------------------------------------------------------
//  force all PWMs to 0% duty cycle
// ------------------------------------------------------------------------------
	EPwm1Regs.CMPA.half.CMPA=0;	// PWM 1A - PhaseA 
	EPwm2Regs.CMPA.half.CMPA=0;	// PWM 2A - PhaseB
	EPwm3Regs.CMPA.half.CMPA=0;	// PWM 3A - PhaseC  
 
#endif // (BUILDLEVEL==LEVEL3)

// =============================== LEVEL 4 ======================================
// ============================================================================== 

#if (BUILDLEVEL==LEVEL4)

//	_iq IDCfdbk;
	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));
/*#if defined (DRV8312)
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1;
#endif    
#if defined(DRV8301) || defined(DRV8302)
    IDCfdbk=-((_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1);
#endif*/

// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
// ------------------------------------------------------------------------------
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = RampDelay;
	RC3_MACRO(rmp3)	

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
	impl1.Period = rmp3.Out; 
	IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(DfuncStartup > _IQ(0.0))
	{
		mod_dir1.CntDirection = _IQ(1.0);
	}
	else
	{
		mod_dir1.CntDirection = _IQ(-1.0);
	}
	mod_dir1.TrigInput = impl1.Out;
	MOD6CNTDIR_MACRO(mod_dir1)

// ------------------------------------------------------------------------------
//    Connect inputs of the INSTASPIN_BLDC module and call the INSTASPIN_BLDC function.
// ------------------------------------------------------------------------------
	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	InstaSPIN_BLDC1.State = mod_dir1.Counter;						// Update the state
	InstaSPIN_BLDC(&InstaSPIN_BLDC1);
	   
// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation
//    update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
	pwmcntl1.Duty = DfuncStartup;
	PWM_CNTL_MACRO(pwmcntl1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	PwmDacCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase));
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	DlogCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase)); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);

#endif // (BUILDLEVEL==LEVEL4)

// =============================== LEVEL 5 ======================================
// ==============================================================================

#if (BUILDLEVEL==LEVEL5) 

//	_iq IDCfdbk;
	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));
/*#if defined (DRV8312)
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1;
#endif    
#if defined(DRV8301) || defined(DRV8302)
    IDCfdbk=-((_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1);
#endif*/

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(rc1.SetpointValue > _IQ(0.0))
	{
		mod_dir1.CntDirection = _IQ(1.0);
	}
	else
	{
		mod_dir1.CntDirection = _IQ(-1.0);
	}
	PreviousState = mod_dir1.Counter; 
	MOD6CNTDIR_MACRO(mod_dir1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
// ------------------------------------------------------------------------------  
	//while counting up we want positive speed 
	if((mod_dir1.Counter==5)&&(PreviousState==4)&&(mod_dir1.TrigInput)) 
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		SpeedLoopFlag = TRUE;
	}
	//while counting down we want negative speed 
	else if((mod_dir1.Counter==0)&&(PreviousState==1)&&(mod_dir1.TrigInput))
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		speed1.Speed = _IQmpy(speed1.Speed,_IQ(-1.0));
		speed1.SpeedRpm = _IQmpy(speed1.SpeedRpm,_IQ(-1.0));
		SpeedLoopFlag = TRUE;
	} 

// ------------------------------------------------------------------------------
//    Connect inputs of the INSTASPIN_BLDC module and call the INSTASPIN_BLDC function.
// ------------------------------------------------------------------------------
	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	InstaSPIN_BLDC1.State = mod_dir1.Counter;						// Update the state
	InstaSPIN_BLDC(&InstaSPIN_BLDC1);
	mod_dir1.TrigInput = InstaSPIN_BLDC1.Comm_Trig;

	if(rmp3.Ramp3DoneFlag == 0)
	{
		rc1.SetpointValue = _IQmpy(DfuncStartup,mod_dir1.CntDirection);
// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
// ------------------------------------------------------------------------------
		rmp3.DesiredInput = CmtnPeriodTarget;
		rmp3.Ramp3Delay = RampDelay;
		RC3_MACRO(rmp3)	

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
		impl1.Period = rmp3.Out;
		IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
		mod_dir1.TrigInput = impl1.Out;

	}
	else
	{
		// ------------------------------------------------------------------------------
		//    Connect inputs of the RMP module and call the Ramp control macro.
		// ------------------------------------------------------------------------------
    	rc1.TargetValue = DfuncTesting;
   		RC_MACRO(rc1)
	}

// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation
//    update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
	pwmcntl1.Duty = rc1.SetpointValue;
	PWM_CNTL_MACRO(pwmcntl1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	PwmDacCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase));
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	DlogCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase)); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);

#endif // (BUILDLEVEL==LEVEL5)

// =============================== LEVEL 6 ======================================
// ==============================================================================

#if (BUILDLEVEL==LEVEL6) 

	_iq IDCfdbk;
	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;
	_iq IRef1;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));
#if defined (DRV8312)
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1;
#endif    
#if defined(DRV8301) || defined(DRV8302)
    IDCfdbk=-((_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(IRef > _IQ(0.0))
	{
		mod_dir1.CntDirection = _IQ(1.0);
	}
	else
	{
		mod_dir1.CntDirection = _IQ(-1.0);
	}
	PreviousState = mod_dir1.Counter; 
	MOD6CNTDIR_MACRO(mod_dir1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
// ------------------------------------------------------------------------------  
	//while counting up we want positive speed 
	if((mod_dir1.Counter==5)&&(PreviousState==4)&&(mod_dir1.TrigInput)) 
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		SpeedLoopFlag = TRUE;
	}
	//while counting down we want negative speed 
	else if((mod_dir1.Counter==0)&&(PreviousState==1)&&(mod_dir1.TrigInput))
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		speed1.Speed = _IQmpy(speed1.Speed,_IQ(-1.0));
		speed1.SpeedRpm = _IQmpy(speed1.SpeedRpm,_IQ(-1.0));
		SpeedLoopFlag = TRUE;
	} 

// ------------------------------------------------------------------------------
//    Connect inputs of the INSTASPIN_BLDC module and call the INSTASPIN_BLDC function.
// ------------------------------------------------------------------------------
	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	InstaSPIN_BLDC1.State = mod_dir1.Counter;						// Update the state
	InstaSPIN_BLDC(&InstaSPIN_BLDC1);
	mod_dir1.TrigInput = InstaSPIN_BLDC1.Comm_Trig;

	IRef1 = IRef;
	if(rmp3.Ramp3DoneFlag == 0)
	{
		IRef1 = _IQmpy(CurrentStartup,mod_dir1.CntDirection); //use startup current during initial ramp
// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
// ------------------------------------------------------------------------------
		rmp3.DesiredInput = CmtnPeriodTarget;
		rmp3.Ramp3Delay = RampDelay;
		RC3_MACRO(rmp3)	

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
		impl1.Period = rmp3.Out;
		IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
		mod_dir1.TrigInput = impl1.Out;

	}
// ------------------------------------------------------------------------------
//    Connect inputs of the PID_REG3 module and call the PID current controller
//	  macro.
// ------------------------------------------------------------------------------  
	pid1_idc.term.Ref = IRef1;
	pid1_idc.term.Fbk = _IQmpy(IDCfdbk,mod_dir1.CntDirection);
	PID_GR_MACRO(pid1_idc)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation
//    update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
    pwmcntl1.Duty = pid1_idc.term.Out;
	PWM_CNTL_MACRO(pwmcntl1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	PwmDacCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase));
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	DlogCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase)); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
#endif // (BUILDLEVEL==LEVEL6)


// =============================== LEVEL 7 ======================================
// ============================================================================== 

#if (BUILDLEVEL==LEVEL7)

//	_iq IDCfdbk;
	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));
/*#if defined (DRV8312)
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1;
#endif    
#if defined(DRV8301) || defined(DRV8302)
    IDCfdbk=-((_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1);
#endif*/

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(SpeedRef > _IQ(0.0))
	{
		mod_dir1.CntDirection = _IQ(1.0);
	}
	else
	{
		mod_dir1.CntDirection = _IQ(-1.0);
	}
	PreviousState = mod_dir1.Counter; 
	MOD6CNTDIR_MACRO(mod_dir1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
// ------------------------------------------------------------------------------  
	//while counting up we want positive speed 
	if((mod_dir1.Counter==5)&&(PreviousState==4)&&(mod_dir1.TrigInput)) 
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		SpeedLoopFlag = TRUE;
	}
	//while counting down we want negative speed 
	else if((mod_dir1.Counter==0)&&(PreviousState==1)&&(mod_dir1.TrigInput))
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		speed1.Speed = _IQmpy(speed1.Speed,_IQ(-1.0));
		speed1.SpeedRpm = _IQmpy(speed1.SpeedRpm,_IQ(-1.0));
		SpeedLoopFlag = TRUE;
	} 

// ------------------------------------------------------------------------------
//    Connect inputs of the INSTASPIN_BLDC module and call the INSTASPIN_BLDC function.
// ------------------------------------------------------------------------------
	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	InstaSPIN_BLDC1.State = mod_dir1.Counter;						// Update the state
	InstaSPIN_BLDC(&InstaSPIN_BLDC1);
	mod_dir1.TrigInput = InstaSPIN_BLDC1.Comm_Trig;

	if((rmp3.Ramp3DoneFlag == 0) || (SpeedLoopFlag == FALSE))
	{
// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
// ------------------------------------------------------------------------------
		rmp3.DesiredInput = CmtnPeriodTarget;
		rmp3.Ramp3Delay = RampDelay;
		RC3_MACRO(rmp3)	

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
		impl1.Period = rmp3.Out;
		IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
		mod_dir1.TrigInput = impl1.Out;
	}

// ------------------------------------------------------------------------------
//    Connect inputs of the PID_REG3 module and call the PID speed controller
//	  macro.
// ------------------------------------------------------------------------------  
      pid1_spd.term.Ref = SpeedRef;
      pid1_spd.term.Fbk = speed1.Speed;
      PID_GR_MACRO(pid1_spd)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation
//    update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
// Switch from fixed duty-cycle or controlled Speed duty-cycle by SpeedLoopFlag variable
	if(SpeedLoopFlag == FALSE)
	{
		pwmcntl1.Duty = _IQmpy(DfuncStartup,mod_dir1.CntDirection);                 // fixed duty-cycle
	}
	else 
	{
		pwmcntl1.Duty = pid1_spd.term.Out;   // controlled Speed duty-cycle
	}
	PWM_CNTL_MACRO(pwmcntl1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	PwmDacCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase));
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	DlogCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase)); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);

#endif // (BUILDLEVEL==LEVEL7)

// =============================== LEVEL 8 ======================================
// ============================================================================== 

#if (BUILDLEVEL==LEVEL8)

	_iq IDCfdbk;
	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;

// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));
#if defined (DRV8312)
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1;
#endif    
#if defined(DRV8301) || defined(DRV8302)
    IDCfdbk=-((_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IDC_offset)<<1);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(SpeedRef > _IQ(0.0))
	{
		mod_dir1.CntDirection = _IQ(1.0);
	}
	else
	{
		mod_dir1.CntDirection = _IQ(-1.0);
	}
	PreviousState = mod_dir1.Counter; 
	MOD6CNTDIR_MACRO(mod_dir1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_PR module and call the speed calculation macro.
// ------------------------------------------------------------------------------  
	//while counting up we want positive speed 
	if((mod_dir1.Counter==5)&&(PreviousState==4)&&(mod_dir1.TrigInput)) 
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		SpeedLoopFlag = TRUE;
	}
	//while counting down we want negative speed 
	else if((mod_dir1.Counter==0)&&(PreviousState==1)&&(mod_dir1.TrigInput))
	{
		speed1.TimeStamp = VirtualTimer;
		SPEED_PR_MACRO(speed1);
		speed1.Speed = _IQmpy(speed1.Speed,_IQ(-1.0));
		SpeedLoopFlag = TRUE;
	} 

// ------------------------------------------------------------------------------
//    Connect inputs of the INSTASPIN_BLDC module and call the INSTASPIN_BLDC function.
// ------------------------------------------------------------------------------
	InstaSPIN_BLDC1.Vag = iqVaIn - InstaSPIN_BLDC1.VaOffset;		// Adjust for offset of Va_in
	InstaSPIN_BLDC1.Vbg = iqVbIn - InstaSPIN_BLDC1.VbOffset;		// Adjust for offset of Vb_in
	InstaSPIN_BLDC1.Vcg = iqVcIn - InstaSPIN_BLDC1.VcOffset;		// Adjust for offset of Vc_in
	InstaSPIN_BLDC1.State = mod_dir1.Counter;						// Update the state
	InstaSPIN_BLDC(&InstaSPIN_BLDC1);
	mod_dir1.TrigInput = InstaSPIN_BLDC1.Comm_Trig;

	if((rmp3.Ramp3DoneFlag == 0) || (SpeedLoopFlag == FALSE))
	{
// ------------------------------------------------------------------------------
//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
// ------------------------------------------------------------------------------
		rmp3.DesiredInput = CmtnPeriodTarget;
		rmp3.Ramp3Delay = RampDelay;
		RC3_MACRO(rmp3)	

// ------------------------------------------------------------------------------
//    Connect inputs of the IMPULSE module and call the Impulse macro.
// ------------------------------------------------------------------------------      
		impl1.Period = rmp3.Out;
		IMPULSE_MACRO(impl1)	

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
		mod_dir1.TrigInput = impl1.Out;
	}
// ------------------------------------------------------------------------------
//    Connect inputs of the PID_REG3 module and call the PID speed controller
//	  macro.
// ------------------------------------------------------------------------------  
   	pid1_spd.term.Ref = SpeedRef;
   	pid1_spd.term.Fbk = speed1.Speed;
   	PID_GR_MACRO(pid1_spd)


// ------------------------------------------------------------------------------
//    Connect inputs of the PID_REG3 module and call the PID current controller
//	  macro.
// ------------------------------------------------------------------------------  
// Switch from fixed duty-cycle or controlled Speed duty-cycle by SpeedLoopFlag variable
	if(SpeedLoopFlag == FALSE)
	{
      pid1_idc.term.Ref = _IQmpy(CurrentStartup,mod_dir1.CntDirection);
	}
	else 
	{
      	pid1_idc.term.Ref = pid1_spd.term.Out;
	}
	pid1_idc.term.Fbk = _IQmpy(IDCfdbk,mod_dir1.CntDirection);
	PID_GR_MACRO(pid1_idc)
// ------------------------------------------------------------------------------
//    Connect inputs of the PWM_DRV module and call the PWM signal generation
//    update macro.
// ------------------------------------------------------------------------------
	pwmcntl1.State = (int16)mod_dir1.Counter;
    pwmcntl1.Duty = pid1_idc.term.Out;
	PWM_CNTL_MACRO(pwmcntl1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
	PwmDacCh1 = (int16)(mod_dir1.Counter * 4096.0L);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	PwmDacCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase));
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)mod_dir1.Counter; 
	DlogCh2 = _IQtoQ15(InstaSPIN_BLDC1.V_int); 
	DlogCh3 = _IQtoQ15(*(InstaSPIN_BLDC1.Vphase)); 
	DlogCh4 = _IQtoQ15(InstaSPIN_BLDC1.Vag);

#endif // (BUILDLEVEL==LEVEL8)

// ------------------------------------------------------------------------------
//    Call the PWMDAC update macro.
// ------------------------------------------------------------------------------
	PWMDAC_MACRO(pwmdac1) 

// ------------------------------------------------------------------------------
//    Call the DATALOG update function.
// ------------------------------------------------------------------------------
    dlog.update(&dlog);

// ------------------------------------------------------------------------------
//    Increase virtual timer and force 15 bit wrap around
// ------------------------------------------------------------------------------
	VirtualTimer++;
	VirtualTimer &= 0x00007FFF;
   
	}//end if(RunMotor)
   
   

#if (DSP2803x_DEVICE_H==1)
/* Enable more interrupts from this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
*/

	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;		// Clear ADCINT1 flag reinitialize for next SOC	

// Acknowledge interrupt to recieve more interrupts from PIE group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

#endif

#if (DSP280x_DEVICE_H==1)
// Enable more interrupts from this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

// Acknowledge interrupt to recieve more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
#endif


}// ISR Ends Here


//===========================================================================
// No more.
//===========================================================================

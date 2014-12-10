/* ==============================================================================
System Name:  	DRV83xx EVM Integrated Flash Image for use with GUI

File Name:	    BLDC_Int_GUI_DRV83xx.c

Description:	Primary system file for the Implementation of BEMF Integration with
				GUI control
				
Originator:		Digital control systems Group - Texas Instruments 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 xx Aug 2011: Jon Warriner  
=================================================================================  */

// Include header files used in the main function
#include "PeripheralHeaderIncludes.h"
#include "BLDC_Int_GUI_DRV83xx-Settings.h"
#include "IQmathLib.h"
#include "BLDC_Int_GUI_DRV83xx.h"
#include "Graph_Data.h"
#include "Commros_user.h"

#include <math.h>

// Prototype statements for functions found within this file.
interrupt void MainISR(void);
void DeviceInit();
void MemCopy();
void InitFlash();
void Gui_DBUFF_init();

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

//GUI variables
//Want to place these in their own memory section that won't move around as
//non-gui variables are added/deleted.  
#pragma DATA_SECTION(Gui,"GUIVARS");
struct GUI_VARS Gui;

// Global variables used in this system (but not by the GUI)
// Define the electrical motor parametes
float RS; 		// Stator resistance (ohm) 
float RR;   	// Rotor resistance (ohm) 
float LS;   	// Stator inductance (H) 
float LR;   	// Rotor inductance (H) 	
float LM;   	// Magnatizing inductance (H)

// Define the base quantites for PU system conversion
float BASE_VOLTAGE; // Base peak phase voltage (volt)
float BASE_CURRENT; // Base peak phase current (amp)
float32 T;   // Samping period (sec) 

_iq BemfA = 0;
_iq BemfB = 0;
_iq BemfC = 0;
_iq IDC_offset = _IQ(0.5);
_iq IA_offset = _IQ(0.5);
_iq IRef=_IQ(0.0); 				// Id reference (pu) 
_iq SpeedRef=_IQ(0.0); 			// Speed reference (pu) 
_iq cal_filt_gain;
_iq spd_filt_gain;
_iq DfuncRun;

Uint32 VirtualTimer = 0;
Uint32 IsrTicker = 0;
Uint32 CmtnPeriodTarget;
Uint32 CmtnPeriodSetpt;

int32 RAMP_START_RATE;
int32 RAMP_END_RATE;

Uint16 CALIBRATE_FLAG = 1;
Uint16 CALIBRATE_TIME = 0x07FF;		//give the calibration filters about 100ms (~10tc) to settle	 
Uint16 BackTicker = 0;
Uint16 PreviousState;
Uint16 SpeedLoopFlag = FALSE;
Uint16 RunBLDC_Int=0;
Uint16 ISR_FREQUENCY=20;		// Define the ISR frequency (kHz)
Uint16 PWM_FREQUENCY=20;		// Define the ISR frequency (kHz)
Uint16 control_mode = 0;

int16	VTimer0[4];			// Virtual Timers slaved off CPU Timer 0 (A events)
int16	VTimer1[4]; 		// Virtual Timers slaved off CPU Timer 1 (B events)
int16	VTimer2[4]; 		// Virtual Timers slaved off CPU Timer 2 (C events)
int16 K_VdcBus = 6632;
int16	i;								// common use incrementer
_iq DlogCh1 = 0;
_iq DlogCh2 = 0;
_iq DlogCh3 = 0;
int16 LedBlinkCnt=50;
int16 DlogCurrElementIndex=0;

int16 PwmDacCh1 = 0;
int16 PwmDacCh2 = 0;
int16 PwmDacCh3 = 0;
#if defined(DRV8301) || defined(DRV8302)
int16 PwmDacCh4 = 0;
#endif

_iq *dlogptr1;
_iq *dlogptr2;
_iq *dlogptr3;

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
struct GRAPH_DATA gdata;

#ifdef DRV8301
union DRV8301_STATUS_REG_1 DRV8301_stat_reg1;
union DRV8301_STATUS_REG_2 DRV8301_stat_reg2;
union DRV8301_CONTROL_REG_1 DRV8301_cntrl_reg1;
union DRV8301_CONTROL_REG_2 DRV8301_cntrl_reg2;
Uint16 read_drv_status = 0;
#endif

Uint32 BemfTrigCnt = 0;
Uint32 BemfLastTrigCnt = 0;
Uint16 GoodTrigCnt = 0;
Uint16 ClosedCommutationFlag = 0;
Uint16 GoodTrigCntTrip = 20;

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

// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;
//	C_Task_Ptr = &C1;

// Configure CPU-Timer 2 to interrupt every ISR Period:
// 60MHz CPU Freq, ISR Period (in uSeconds)
// This function is found in DSP280x_CpuTimers.c
//jpw	InitCpuTimers();
//jpw	ConfigCpuTimer(&CpuTimer0, 60, SYSTEM_FREQUENCY*10/ISR_FREQUENCY);
//jpw    StartCpuTimer0();

#ifdef DRV8301
// Initialize SPI for communication to the DRV8301
	DRV8301_SPI_Init(&SpibRegs);
#endif
	
// Timing sync for background loops 
// Timer period definitions found in device specific PeripheralHeaderIncludes.h
//	CpuTimer0Regs.PRD.all =  mSec1;		// 
	CpuTimer1Regs.PRD.all =  mSec1;		// A tasks
	CpuTimer2Regs.PRD.all =  mSec5;		// B tasks 

	Gui.Ref = _IQ(0.3);										//written by the Reference knob in the GUI
	Gui.CurrentDisplay = _IQ(0.0);							//read by the GUI to display motor current
	Gui.DfuncStartup = _IQ(0.1);							//written by the GUI to set startup duty cycle
#if defined(DRV8312)
	Gui.CurrentStartup = _IQ(0.1);							//written by the GUI to set startup duty cycle
#endif
#if defined(DRV8301) || defined(DRV8302)
	Gui.CurrentStartup = _IQ(0.02);							//written by the GUI to set startup duty cycle
#endif
	Gui.Threshold = _IQ(0.4);								//written by the GUI to set Integration threshold
#if defined(DRV8312)
	Gui.Current_Kp = _IQ(0.1);
#endif
#if defined(DRV8301) || defined(DRV8302)
	Gui.Current_Kp = _IQ(1.0);
#endif
	Gui.Velocity_Kp = _IQ(0.5);
	
	Gui.I_max = _IQ(0.95);

	Gui.RampUpTime = 25;
	Gui.CommErrorMax = 1.0;
	Gui.TripCnt = 3;
	Gui.AdvancedStartup = 0;

	Gui.SpeedRPM = 0;										//read by the GUI to display motor speed in RPM
	Gui.BEGIN_START_RPM = 50;								//written by the GUI to set start speed of ramp up controller
	Gui.END_START_RPM = 100;								//written by the GUI to set end speed of ramp up controller

	Gui.current_mode = 0;
	Gui.velocity_mode = 0;
	Gui.ResetFault = 0;
	Gui.POLES = 8;  	// Number of poles
	Gui.Current_Ki = 20;
	Gui.Velocity_Ki = 3;
	Gui.Prescaler = 1;

	Gui.OverVoltage=0;
	Gui.DRVFaultFlag=0;
	Gui.DRVOTWFlag = 0;
	Gui.EnableFlag = FALSE;
	Gui.VdcBus = 0;
#if defined(DRV8312)
	Gui.Max_VDC = 2880;
	Gui.Min_VDC = 1920;
#endif
#if defined(DRV8301) || defined(DRV8302)
	Gui.Max_VDC = 6000;
	Gui.Min_VDC = 800;
#endif

// Reassign ISRs. 
	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.ADCINT1 = &MainISR;
	EDIS;   // This is needed to disable write to EALLOW protected registers

// Enable PIE group 1 interrupt 1 for ADCINT1
    PieCtrlRegs.PIEIER1.all = M_INT1;

	InitCommros();
	
	// Define the base quantites for PU system conversion
	BASE_VOLTAGE = 66.32;		// Base peak phase voltage (volt), maximum measurable DC Bus
#if defined(DRV8312)
	BASE_CURRENT = 8.6;			// Base peak phase current (amp)
#endif
#if defined(DRV8301) || defined(DRV8302)
//options for BASE_CURRENT based on DRV830x current-sense amplifier gain setting
//NOTE: DRV8302 can only be set to gain of 10 or 40
	BASE_CURRENT = 82.5;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 10)
//	BASE_CURRENT = 41.25;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 20)
//	BASE_CURRENT = 20.625;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 40)
//	BASE_CURRENT = 10.3125;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 80)
#endif
	Gui.BASE_FREQ =  200;			// Base electrical frequency (Hz) 

	// Define the ISR frequency (kHz)
	ISR_FREQUENCY=20;
	PWM_FREQUENCY=ISR_FREQUENCY;
	T = 0.001/ISR_FREQUENCY;    // Samping period (sec), see parameter.h
				
// Initialize the PWM control module
	pwmcntl1.PWMprd = SYSTEM_FREQUENCY*1000000*T/2;				// Set the pwm period for this example
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

	RAMP_START_RATE = (PWM_FREQUENCY*1000)*60.0/Gui.BEGIN_START_RPM/COMMUTATES_PER_E_REV/(Gui.POLES/2.0);
	RAMP_END_RATE = (PWM_FREQUENCY*1000)*60.0/Gui.END_START_RPM/COMMUTATES_PER_E_REV/(Gui.POLES/2.0);
	CmtnPeriodTarget = RAMP_END_RATE;
	CmtnPeriodSetpt = RAMP_START_RATE;

// Initialize DATALOG module
    gdata.ch1_ptr = &DlogCh1;
    gdata.ch2_ptr = &DlogCh2;
    gdata.ch3_ptr = &DlogCh3;
    gdata.trig_value = 1;
    gdata.size = 0x100;
    gdata.prescalar = Gui.Prescaler;
    gdata.holdoff = 10000;
    Graph_Data_Init(&gdata);
	dlogptr1=DLOG_4CH_buff1;
	dlogptr2=DLOG_4CH_buff2;
	dlogptr3=DLOG_4CH_buff3;

// Initialize ADC module (F2803XIDC_VEMF.H)
	ADC_MACRO_INIT() 

 // Initialize the SPEED_PR module 
 	speed1.InputSelect = 0;
 	speed1.BaseRpm = 120*(Gui.BASE_FREQ/Gui.POLES);
 	speed1.SpeedScaler = (Uint32)(ISR_FREQUENCY/(1*(float32)Gui.BASE_FREQ*0.001));

// Initialize RMPCNTL module
    rc1.RampDelayMax = 1;
    rc1.RampLowLimit = _IQ(-1.0);
    rc1.RampHighLimit = _IQ(1.0);

// Initialize RMP3 module
	rmp3.DesiredInput = CmtnPeriodTarget;
	rmp3.Ramp3Delay = (Uint32)(((float32)Gui.RampUpTime * 0.001)/((float32)(CmtnPeriodSetpt - CmtnPeriodTarget) * T));
    rmp3.Out = CmtnPeriodSetpt;
    rmp3.Ramp3Min = CmtnPeriodTarget;

//Initialize the INSTASPIN_BLDC module
	InstaSPIN_BLDC1.VaOffset = 0;
	InstaSPIN_BLDC1.VbOffset = 0;
	InstaSPIN_BLDC1.VcOffset = 0;
	InstaSPIN_BLDC1.Int_Threshold = Gui.Threshold;

// Initialize the PID_GRANDO_CONTROLLER module for dc-bus current
    pid1_idc.param.Kp = Gui.Current_Kp;
    pid1_idc.param.Kr = _IQ(1.0);
	pid1_idc.param.Ki = _IQ(T * (float32)Gui.Current_Ki);
	pid1_idc.param.Kd = _IQ(0/T);
    pid1_idc.param.Km = _IQ(1.0);
    pid1_idc.param.Umax = _IQ(0.95);
    pid1_idc.param.Umin = _IQ(-0.95);

// Initialize the PID_GRANDO_CONTROLLER module for Speed 
    pid1_spd.param.Kp = Gui.Velocity_Kp;
    pid1_spd.param.Kr = _IQ(1.0);
	pid1_spd.param.Ki = _IQ(T * (float32)Gui.Velocity_Ki);
	pid1_spd.param.Kd = _IQ(0/T);
    pid1_spd.param.Km = _IQ(1.0);
    pid1_spd.param.Umax = Gui.I_max;
    pid1_spd.param.Umin = -Gui.I_max;
    
// Initialize the current offset calibration filter
	cal_filt_gain = _IQ(T/(T+TC_CAL));

// Initialize the current offset calibration filter
	spd_filt_gain = _IQ(T/(T+TC_SPD));

// Enable CPU INT1 for ADCINT1:
	IER |= M_INT1;

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
		ServiceRoutine(&commros);
		
		//try to detect and reset any SCI errors
		//Specifically recover from a break detect that
		//results when the USB cable is plugged in.
		//The FTDI chip spews some long pulses on its
		//Tx pin at power-up.  If C2000 is running when
		//this happens it triggers SCI break detect.
		if(SciaRegs.SCIRXST.bit.RXERROR)
		{
			GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
			SciaRegs.SCICTL1.bit.SWRESET = 0;
			SciaRegs.SCICTL1.bit.SWRESET = 1;
		}
		else
		{
			GpioDataRegs.GPASET.bit.GPIO31 = 1;
		}

		// Update current and velocity loop PI gains from GUI
    	pid1_idc.param.Kp = Gui.Current_Kp;
		pid1_idc.param.Ki = _IQ(T * (float32)Gui.Current_Ki);
    	pid1_spd.param.Kp = Gui.Velocity_Kp;
		pid1_spd.param.Ki = _IQ(T * (float32)Gui.Velocity_Ki);

		gdata.prescalar = Gui.Prescaler;
		GoodTrigCntTrip = (Uint16)(((Gui.CommErrorMax / T) * 0.001) + 0.5);
		
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
	if(CpuTimer1Regs.TCR.bit.TIF == 1)
	{
		CpuTimer1Regs.TCR.bit.TIF = 1;	// clear flag

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
	if(CpuTimer2Regs.TCR.bit.TIF == 1)
	{
		CpuTimer2Regs.TCR.bit.TIF = 1;				// clear flag

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
void A1(void) // Used to enable disable PWM's for the inverter
//--------------------------------------------------------
{
	if(Gui.EnableFlag==FALSE)
	{
		RunBLDC_Int=0;

		//shut down all PWMs
#if defined (DRV8312)
		PHASE_A_OFF;
		PHASE_B_OFF;
		PHASE_C_OFF;
#endif
#if defined(DRV8301) || defined(DRV8302)
		//de-assert the DRV830x EN_GATE pin
		GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
#endif
		EPwm1Regs.CMPA.half.CMPA=0;	// PWM 1A - PhaseA 
		EPwm2Regs.CMPA.half.CMPA=0;	// PWM 2A - PhaseB
		EPwm3Regs.CMPA.half.CMPA=0;	// PWM 3A - PhaseC  

		if(Gui.ResetFault == 1)
		{
#if defined (DRV8312)
			//reset the DRV chip
			GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
			GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
#endif
#if defined(DRV8301) || defined(DRV8302)
			//de-assert the DRV830x EN_GATE pin
			GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
#endif
		  	Gui.ResetFault = 2;
		}
		else if(Gui.ResetFault == 2)
		{
		  	//reset the trip zone
		  	EALLOW;
			EPwm1Regs.TZCLR.bit.OST=1;
			EPwm2Regs.TZCLR.bit.OST=1;
			EPwm3Regs.TZCLR.bit.OST=1;
			EDIS;
		  	
			Gui.ResetFault = 0;
			Gui.DRVFaultFlag = 0;
		}
		  
		//zero all displayed variables when disabled
		Gui.SpeedRPM = 0;
		speed1.SpeedRpm = 0;
		speed1.Speed = 0;
		Gui.CurrentDisplay = 0;
		CALIBRATE_FLAG = 1;
		InstaSPIN_BLDC1.VaOffset = 0;
		InstaSPIN_BLDC1.VbOffset = 0;
		InstaSPIN_BLDC1.VcOffset = 0;
		IDC_offset = _IQ(0.5);
		SpeedLoopFlag=FALSE;

		
	}	
	// if motor type is not defined 
	if((Gui.EnableFlag == TRUE) && (RunBLDC_Int == FALSE))
	{
		// Read GUI_SpeedRef Only when you have enabled the motor
		SpeedRef = Gui.Ref;

		if((RunBLDC_Int == 0) && (Gui.DRVFaultFlag == 0))
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
			DRV8301_cntrl_reg2.bit.GAIN = 0;				// CS amplifier gain = 10
//			DRV8301_cntrl_reg2.bit.GAIN = 1;				// CS amplifier gain = 20
//			DRV8301_cntrl_reg2.bit.GAIN = 2;				// CS amplifier gain = 40
//			DRV8301_cntrl_reg2.bit.GAIN = 3;				// CS amplifier gain = 80
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
			// Define the base quantites for PU system conversion
			BASE_VOLTAGE = 66.32;		// Base peak phase voltage (volt), maximum measurable DC Bus
#if defined(DRV8312)
			BASE_CURRENT = 8.6;			// Base peak phase current (amp)
#endif
#if defined(DRV8301) || defined(DRV8302)
//options for BASE_CURRENT based on DRV830x current-sense amplifier gain setting
//NOTE: DRV8302 can only be set to gain of 10 or 40
			BASE_CURRENT = 82.5;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 10)
//			BASE_CURRENT = 41.25;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 20)
//			BASE_CURRENT = 20.625;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 40)
//			BASE_CURRENT = 10.3125;		// Base peak phase current (amp) , maximum measurable peak current (with DRV830x gain set to 80)
#endif

			// Define the ISR frequency (kHz)
			ISR_FREQUENCY=20;
			T = 0.001/ISR_FREQUENCY;    // Samping period (sec), see parameter.h

 			speed1.InputSelect = 0;
 			speed1.BaseRpm = 120*(Gui.BASE_FREQ/Gui.POLES);
 			speed1.SpeedScaler = (Uint32)(ISR_FREQUENCY/(1*(float32)Gui.BASE_FREQ*0.001));
			speed1.InputSelect = 0;
			speed1.NewTimeStamp = 0;
			speed1.OldTimeStamp = 0;
			speed1.EventPeriod = 0;
			speed1.Speed = 0;
			VirtualTimer = 0;
		
			rc1.EqualFlag = 0;
			rc1.RampDelayCount = 0;
			rc1.TargetValue = 0;
					
			//The speed at the end of the ramp should always be greater than the speed at the beginning of the ramp
			//If the GUI entries violate this use
			//BEGIN_START_RPM = 50
			//BEGIN_END_RPM = 100
			if(Gui.END_START_RPM > Gui.BEGIN_START_RPM)
			{
				RAMP_START_RATE = (PWM_FREQUENCY*1000)*60.0/Gui.BEGIN_START_RPM/COMMUTATES_PER_E_REV/(Gui.POLES/2.0);
				RAMP_END_RATE = (PWM_FREQUENCY*1000)*60.0/Gui.END_START_RPM/COMMUTATES_PER_E_REV/(Gui.POLES/2.0);
			}
			else
			{
				RAMP_START_RATE = (PWM_FREQUENCY*1000)*60.0/50/COMMUTATES_PER_E_REV/(Gui.POLES/2.0);
				RAMP_END_RATE = (PWM_FREQUENCY*1000)*60.0/100/COMMUTATES_PER_E_REV/(Gui.POLES/2.0);
			}
			
			CmtnPeriodTarget = RAMP_END_RATE;
			CmtnPeriodSetpt = RAMP_START_RATE;
			rmp3.Ramp3Delay = (Uint32)(((float32)Gui.RampUpTime * 0.001)/((float32)(CmtnPeriodSetpt - CmtnPeriodTarget) * T));
			rmp3.DesiredInput = CmtnPeriodTarget;
    		rmp3.Out = CmtnPeriodSetpt;
			rmp3.Ramp3Min = CmtnPeriodTarget;
			rmp3.Ramp3DelayCount = 0;
			rmp3.Ramp3DoneFlag = 0; 

			impl1.Counter = 0;
			impl1.Out = 0;
		
			mod_dir1.Counter = 0;
		
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
    		pid1_spd.param.Umax = Gui.I_max;
    		pid1_spd.param.Umin = -Gui.I_max;

			DfuncRun = Gui.DfuncStartup;
			BemfTrigCnt = 0;
			BemfLastTrigCnt = 0;
			GoodTrigCnt = 0;
			ClosedCommutationFlag = 0;
			InstaSPIN_BLDC1.V_int = 0;
			
			//set control_mode based on check box settings from GUI
			//VOLTAGE		0			//open-loop volage mode only
			//CURRENT		1			//closed-loop current control
			//VELOCITY		2			//closed-loop velocity control
			//CASCADE		3			//cascaded closed-loop velocity->current control
			//velocity loop check box writes either 0 (disabled) or 2 (enabled)
			//current loop check box writes either 0 (disabled) or 1 (enabled)
			control_mode = Gui.velocity_mode + Gui.current_mode;
			
			RunBLDC_Int=1;

			EALLOW;
				EPwm1Regs.TZCLR.bit.OST=1;
				EPwm2Regs.TZCLR.bit.OST=1;
				EPwm3Regs.TZCLR.bit.OST=1;
			EDIS;
		}
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
	A_Task_Ptr = &A1;
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
void B1(void) // Toggle GPIO-34 
//----------------------------------------
{
	if(RunBLDC_Int==1)
	{		
		if(LedBlinkCnt==0)
		{
			GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;	//turn on/off LD3 on the controlCARD
			LedBlinkCnt=50;
		}
		else
			LedBlinkCnt--;
	}
	else
	{
		if(LedBlinkCnt==0)
		{
			GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;	//turn on/off LD3 on the controlCARD
			LedBlinkCnt=100;
		}
		else
			LedBlinkCnt--;
	}
	
	Gui.VdcBus = ( (long) AdcResult.ADCRESULT6  * (long) K_VdcBus ) >> 12;

	if(Gui.VdcBus < Gui.Min_VDC)
	{
		Gui.OverVoltage = 0;
	}
	else if(Gui.VdcBus > Gui.Max_VDC)
	{
		Gui.OverVoltage = 2;
	}
	else
	{
		Gui.OverVoltage = 1;
	}
	
	if(Gui.OverVoltage != 1)
	{
		EPwm1Regs.TZFRC.bit.OST = 1;
		EPwm2Regs.TZFRC.bit.OST = 1;		
		EPwm3Regs.TZFRC.bit.OST = 1;		
	}
	
	//ignore FAULTn when disabled
	//There is a blip on this pin when the DRV8301 ENABLE is de-asserted
	if((GpioDataRegs.GPADAT.bit.GPIO14 == 0) && RunBLDC_Int)
	{
		Gui.DRVFaultFlag=1;
	}

	if(GpioDataRegs.GPADAT.bit.GPIO13 == 1)
	{
		Gui.DRVOTWFlag = 0;
	}
	else
	{
		Gui.DRVOTWFlag = 1;
	}
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
	B_Task_Ptr = &B1;
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

//MainISR 
interrupt void MainISR(void)
{
	_iq IDCfdbk;
	_iq iqVaIn;
	_iq iqVbIn;
	_iq iqVcIn;
	_iq iqIA;

	GpioDataRegs.GPASET.bit.GPIO22 = 1;
	
// Verifying the ISR
    IsrTicker++;

if(RunBLDC_Int==0)
{
   Gui_DBUFF_init();	
} 
// PMSM Motor
if (RunBLDC_Int==1 && Gui.VdcBus>Gui.Min_VDC && Gui.VdcBus<Gui.Max_VDC)
{ 
	if(CALIBRATE_FLAG)
	{
// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
		iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3))-InstaSPIN_BLDC1.VaOffset;
		iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3))-InstaSPIN_BLDC1.VbOffset; 
		iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3))-InstaSPIN_BLDC1.VcOffset;
    	iqIA=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IA_offset)<<1;
    	IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT5<<3)-IDC_offset)<<1;
// ------------------------------------------------------------------------------
//  LPF to average the calibration offsets
//  Use the offsets calculated here to initialize BemfA_offset, BemfB_offset
//  and BemfC_offset so that they are used for the remaining build levels
// ------------------------------------------------------------------------------
    	InstaSPIN_BLDC1.VaOffset = _IQmpy(cal_filt_gain,iqVaIn) + InstaSPIN_BLDC1.VaOffset;
    	InstaSPIN_BLDC1.VbOffset = _IQmpy(cal_filt_gain,iqVbIn) + InstaSPIN_BLDC1.VbOffset;
    	InstaSPIN_BLDC1.VcOffset = _IQmpy(cal_filt_gain,iqVcIn) + InstaSPIN_BLDC1.VcOffset;
    	IA_offset = _IQmpy(cal_filt_gain,iqIA) + IA_offset;
    	IDC_offset = _IQmpy(cal_filt_gain,IDCfdbk) + IDC_offset;

// ------------------------------------------------------------------------------
//  force all PWMs to 0% duty cycle
// ------------------------------------------------------------------------------
		PHASE_A_ON;
		PHASE_B_ON;
		PHASE_C_ON;

		EPwm1Regs.CMPA.half.CMPA=0;	// PWM 1A - PhaseA 
		EPwm2Regs.CMPA.half.CMPA=0;	// PWM 2A - PhaseB
		EPwm3Regs.CMPA.half.CMPA=0;	// PWM 3A - PhaseC  
 	
 		CALIBRATE_FLAG++;
 		CALIBRATE_FLAG &= CALIBRATE_TIME;
 
	}
	else
	{
// ------------------------------------------------------------------------------
//    ADC conversion and offset adjustment 
// ------------------------------------------------------------------------------
	iqVaIn =  _IQ15toIQ((AdcResult.ADCRESULT1<<3));
	iqVbIn =  _IQ15toIQ((AdcResult.ADCRESULT2<<3)); 
	iqVcIn =  _IQ15toIQ((AdcResult.ADCRESULT3<<3));
   	iqIA=(_IQ15toIQ(AdcResult.ADCRESULT4<<3)-IA_offset)<<1;
#if defined (DRV8312)
    IDCfdbk=(_IQ15toIQ(AdcResult.ADCRESULT5<<3)-IDC_offset)<<1;
#endif    
#if defined(DRV8301) || defined(DRV8302)
    IDCfdbk=-((_IQ15toIQ(AdcResult.ADCRESULT5<<3)-IDC_offset)<<1);
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the MOD6 module and call the Modulo 6 counter macro.
// ------------------------------------------------------------------------------   
	if(Gui.Ref > _IQ(0.0))
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
	
	if((ClosedCommutationFlag == 0) || (SpeedLoopFlag == FALSE))
	{
		// ------------------------------------------------------------------------------
		//    Connect inputs of the RMP3 module and call the Ramp control 3 macro.
		// ------------------------------------------------------------------------------
//		rmp3.DesiredInput = CmtnPeriodTarget;
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
					
		//using advanced startup to count commutation matches?
		if(Gui.AdvancedStartup)
		{
			//When the BEMF signals it's time to commutate, compare
			//against the forced commutation period
			if(InstaSPIN_BLDC1.Comm_Trig)
			{
				BemfLastTrigCnt = BemfTrigCnt;
				BemfTrigCnt = 0;
				//check if the forced commutation period and the BEMF commutation period
				//are within the allowed error window
				if(labs(BemfLastTrigCnt - impl1.Period) < GoodTrigCntTrip)
				{
					GoodTrigCnt++;
				}
				else
				{
					GoodTrigCnt = 0;
				}
				BemfLastTrigCnt = 0;
			}
			else
			{
				BemfTrigCnt++;
			}
			//Check if there are enough commutation matches to switch to closed commutation mode
			if(GoodTrigCnt > Gui.TripCnt)
			{
				ClosedCommutationFlag = 1;
			}								
		}
		//if we're not using advanced startup just switch to closed
		//commutation mode at the end of the startup ramp.
		else if(rmp3.Ramp3DoneFlag != 0)
		{
			ClosedCommutationFlag = 1;
		}
	}

	switch(control_mode)
		{
			case VOLTAGE:
				if(ClosedCommutationFlag == 0)
				{
					SpeedLoopFlag = 1;		//no need to wait for speed feedback update in this mode.
					rc1.SetpointValue = _IQmpy(Gui.DfuncStartup,mod_dir1.CntDirection);
				}
				else
				{
					// ------------------------------------------------------------------------------
					//    Connect inputs of the RMP module and call the Ramp control macro.
					// ------------------------------------------------------------------------------
      				rc1.TargetValue = Gui.Ref;
   	  				RC_MACRO(rc1)
				}
				// ------------------------------------------------------------------------------
				//    Connect inputs of the PWM_DRV module and call the PWM signal generation
				//    update macro.
				// ------------------------------------------------------------------------------
				pwmcntl1.State = (int16)mod_dir1.Counter;
				pwmcntl1.Duty = rc1.SetpointValue;
				PWM_CNTL_MACRO(pwmcntl1)
		
				break;
		
			case CURRENT:
				IRef = Gui.Ref;
				if(ClosedCommutationFlag == 0)
				{
					SpeedLoopFlag = 1;		//no need to wait for speed feedback update in this mode.
					IRef = _IQmpy(Gui.CurrentStartup,mod_dir1.CntDirection); //use startup current during initial ramp
				}
				// ------------------------------------------------------------------------------
				//    Connect inputs of the PID_REG3 module and call the PID current controller
				//	  macro.
				// ------------------------------------------------------------------------------  
      			pid1_idc.term.Ref = IRef;
 				pid1_idc.term.Fbk = _IQmpy(IDCfdbk,mod_dir1.CntDirection);
      			PID_GR_MACRO(pid1_idc)

				// ------------------------------------------------------------------------------
				//    Connect inputs of the PWM_DRV module and call the PWM signal generation
				//    update macro.
				// ------------------------------------------------------------------------------
				pwmcntl1.State = (int16)mod_dir1.Counter;
				pwmcntl1.Duty = pid1_idc.term.Out;
				PWM_CNTL_MACRO(pwmcntl1)		
				break;

			case VELOCITY:
     			SpeedRef = Gui.Ref;

				// ------------------------------------------------------------------------------
				//    Connect inputs of the PID_REG3 module and call the PID current controller
				//	  macro.
				// ------------------------------------------------------------------------------  
				pwmcntl1.State = (int16)mod_dir1.Counter;
				// Switch from fixed duty-cycle or controlled Speed duty-cycle by SpeedLoopFlag variable
      			if((ClosedCommutationFlag == 0) || (SpeedLoopFlag == FALSE))
        		{
        			pwmcntl1.Duty = _IQmpy(Gui.DfuncStartup,mod_dir1.CntDirection);                 // fixed duty-cycle
      			}
      			else 
      			{
					// ------------------------------------------------------------------------------
					//    Connect inputs of the PID_REG3 module and call the PID speed controller
					//	  macro.
					// ------------------------------------------------------------------------------  
      				pid1_spd.term.Ref = SpeedRef;
      				pid1_spd.term.Fbk = speed1.Speed;
      				PID_GR_MACRO(pid1_spd)
      				
        			pwmcntl1.Duty = pid1_spd.term.Out;   // controlled Speed duty-cycle
      			}
				PWM_CNTL_MACRO(pwmcntl1)		
				break;
		
			case CASCADE:
     			SpeedRef = Gui.Ref;
				// ------------------------------------------------------------------------------
				//    Connect inputs of the PID_REG3 module and call the PID current controller
				//	  macro.
				// ------------------------------------------------------------------------------  
      			if((ClosedCommutationFlag == 0) || (SpeedLoopFlag == FALSE))
				{
      				pid1_idc.term.Ref = _IQmpy(Gui.CurrentStartup,mod_dir1.CntDirection);
				}
				else 
				{
					// ------------------------------------------------------------------------------
					//    Connect inputs of the PID_REG3 module and call the PID speed controller
					//	  macro.
					// ------------------------------------------------------------------------------  
      				pid1_spd.term.Ref = SpeedRef;
      				pid1_spd.term.Fbk = speed1.Speed;
      				PID_GR_MACRO(pid1_spd)
      				
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
				break;
		
			default:
				// ------------------------------------------------------------------------------
				//    Connect inputs of the PWM_DRV module and call the PWM signal generation
				//    update macro.
				// ------------------------------------------------------------------------------
				pwmcntl1.State = 0;
				pwmcntl1.Duty = 0;
				PWM_CNTL_MACRO(pwmcntl1)
				break;
		

		}

	//Filter the displayed speed
	Gui.Speed = _IQmpy(spd_filt_gain,speed1.Speed - Gui.Speed) + Gui.Speed;
	Gui.SpeedRPM = _IQmpy(speed1.BaseRpm,Gui.Speed);
	
// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module 
// ------------------------------------------------------------------------------	
/*	PwmDacCh1 = _IQtoQ15(InstaSPIN_BLDC1.V_int);
	PwmDacCh2 = _IQtoQ15(InstaSPIN_BLDC1.Vag); 
	PwmDacCh3 = _IQtoQ15(iqIA);
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = _IQtoQ15(InstaSPIN_BLDC1.V_int);
#endif*/
	PwmDacCh1 = _IQtoQ15(InstaSPIN_BLDC1.V_int);
	PwmDacCh2 = (int16)(mod_dir1.Counter * 4096.0L); 
	PwmDacCh3 = (int16)(GoodTrigCnt * 4096.0L);
#if defined(DRV8301) || defined(DRV8302)
	PwmDacCh4 = (int16)impl1.Out;
#endif

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module 
// ------------------------------------------------------------------------------
	DlogCh1 = InstaSPIN_BLDC1.V_int;
	DlogCh2 = InstaSPIN_BLDC1.Vag; 
	DlogCh3 = iqIA; 

// ------------------------------------------------------------------------------
//    Increase virtual timer and force 15 bit wrap around
// ------------------------------------------------------------------------------
	VirtualTimer++;
	VirtualTimer &= 0x00007FFF;
	
//	Gui.SpeedRPM = speed1.SpeedRpm;
	Gui.CurrentDisplay = IDCfdbk;
    InstaSPIN_BLDC1.Int_Threshold = Gui.Threshold;
	}

}


// ------------------------------------------------------------------------------
//    Call the PWMDAC update macro.
// ------------------------------------------------------------------------------
	PWMDAC_MACRO(pwmdac1)

// ------------------------------------------------------------------------------
//    Call the DATALOG update function.
// ------------------------------------------------------------------------------
	Graph_Data_Update(&gdata);

	GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;

#if (DSP2803x_DEVICE_H==1)||(DSP280x_DEVICE_H==1)||(F2806x_DEVICE_H==1)
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;		// Clear ADCINT1 flag reinitialize for next SOC	

// Acknowledge interrupt to recieve more interrupts from PIE group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
#endif

}

void Gui_DBUFF_init()
{
	if(DlogCurrElementIndex!=gdata.size)
	{
		*dlogptr1=0;
		dlogptr1++;
		*dlogptr2=0;
		dlogptr2++;
		*dlogptr3=0;
		dlogptr3++;
		DlogCurrElementIndex++;
	}
	else
	{
		dlogptr1=DLOG_4CH_buff1;
		dlogptr2=DLOG_4CH_buff2;
		dlogptr3=DLOG_4CH_buff3;
		DlogCurrElementIndex=0;
	}

}
//===========================================================================
// No more.
//===========================================================================




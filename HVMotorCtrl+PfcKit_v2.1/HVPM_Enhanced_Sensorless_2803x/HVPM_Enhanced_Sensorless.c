/* ==============================================================================
System Name:  	HVPM_Enhanced_Sensorless

File Name:	  	HVPM_Enhanced_Sensorless.C

Description:	Primary system file for the Enhanced Implementation of Sensorless
          		Field Orientation Control for Three Phase Permanent-Magnet
          		Synchronous Motor (PMSM)

     *************************************************************************
     * RELEASE NOTES :-                                                      *
     * ================                                                      *
     *    This release is an enhanced version of TI's earlier SMO based      *
     * HVPM_sensorless FOC solution offering, with enhancements to SMO       *
     * itself and to zero and low speed operation due to high frequency      *
     * injection (HFI).                                                      *
     *                                                                       *
     * The enhancements are listed below:                                    *
     * - FOC from zero speed                                                 *
     * - bidirectional speed control through zero speed                      *
     * - Lower speeds, not covered by SMO, are now possible due to HFI       *
     * - SMO filter lag compensation                                         *
     * - Inverter dead band compensation                                     *
     * - Smooth transition between HFI and SMO methods                       *
     *                                                                       *
     * BUILD LEVELs 1 through 6 from the earlier version are retained        *
     *                                                                       *
     * BUILD LEVEL 7                                                         *
     * =============                                                         *
     * The executable for enhanced SMO is in esmo.lib file, which is part of *
     * the control suite. The user is free to evaluate the function of esmo  *
     * using this executable. For access to its executable code, the user is *
     * advised to contact the nearest TI sales office or distributor.        *
     *                                                                       *
     * BUILD LEVEL 8/9                                                       *
     * ===============                                                       *
     * The executable for Initial Position Detection (IPD) and               *
     * High Frequency Injection (HFI) are in hfi.lib file, which are not     *
     * part of the downloadable version of control suite.                    *
     * Any attempt to build LEVEL8/9 code will result in error.              *
     * For access to the library and/or executable code, the user is advised *
     * to contact the nearest TI sales office or distributor.                *
     * ***********************************************************************
     *************************************************************************
     * RELEASE NOTES :-                                                      *
     * ================                                                      *
     * This release is an enhanced version of TI's earlier SMO based         *
     * HVPM_sensorless FOC solution                                          *
     *                                                                       *
     * The general enhancements contained in eSMO.lib:                       *
     * - bidirectional speed control through zero speed                      *
     * - SMO filter lag compensation                                         *
     * - Inverter dead band compensation                                     *
     *                                                                       *
	 * The optional enahancements contained in hfi.lib:				         *                  
	 * - FOC from zero speed                                                 *
     * - Lower speeds, not covered by SMO, are now possible due to HFI       *
	 *   for initial (IPD), zero, and low-speed operation                    *
     * - Smooth transition between HFI and SMO methods                       *
	 * Contact TI sales for access to hfi.lib                                *
     *                                                                       *
     * BUILD LEVELs 1 through 6 from the earlier version are retained        *
     *                                                                       *
     * BUILD LEVEL 7                                                         *
     * =============                                                         *
     * The executable for enhanced SMO is in esmo.lib file, delivered in     *
     * public version of controlSUITE. The user is free to evaluate the      *
     * functionality of esmo  using this executable. 						 *
     *                                                                       *
     * BUILD LEVEL 8/9                                                       *
     * ===============                                                       *
     * The executable for Initial Position Detection (IPD) and               *
     * High Frequency Injection (HFI) are in hfi.lib file, which are not     *
     * part of the downloadable version of controlSUITE.                     *
     * Any attempt to build LEVEL8/9 code will result in error.              *
     * For access to the library and/or executable code, the user is advised *
     * to contact their TI or distributor FAE.				                 *
     * ***********************************************************************

=================================================================================  */

// Include header files used in the main function

#include "PeripheralHeaderIncludes.h"
#define   MATH_TYPE      IQ_MATH
#include "IQmathLib.h"
#include "HVPM_Enhanced_Sensorless.h"
#include "HVPM_Enhanced_Sensorless-Settings.h"
#include <math.h>

// ===============================================================================
// -------- new additions for eSMO/IPD --------
#if (BUILDLEVEL == LEVEL7) || (BUILDLEVEL == LEVEL8) || (BUILDLEVEL == LEVEL9)
  #include "angle_math.h"       	// Include header for ANGLE MATH group of objects
  #include "esmopos.h"              // Include header for eSMO and related objects
#endif

#if (BUILDLEVEL == LEVEL8) || (BUILDLEVEL == LEVEL9)
  #include "transition.h"       	// Include header for TRANSITION object
  #include "hfi.h"       	        // Include header for HFI group of objects
#endif
// ===============================================================================

#ifdef FLASH
  #pragma CODE_SECTION(MainISR,"ramfuncs");
  #pragma CODE_SECTION(OffsetISR,"ramfuncs");
#endif


// ============================================================================
// Macro definitions
#define  SIGN(A)  (A < 0 ? -1 : 1)
// ============================================================================

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
Uint16 OffsetFlag=0;
_iq offsetA=0;
_iq offsetB=0;
_iq offsetC=0;

#define  OFFSET_LPF_K  (OFFSET_FILTER_CORNER_FREQ * 0.001 / ISR_FREQUENCY)
_iq K1 = _IQ(1.0 - OFFSET_LPF_K);   //Offset filter coefficient K1
_iq K2 = _IQ(OFFSET_LPF_K);         //Offset filter coefficient K2

_iq VdTesting = _IQ(0.0);			// Vd reference (pu)
_iq VqTesting = _IQ(0.15);			// Vq reference (pu)
_iq IdRef = _IQ(0.0);				// Id reference (pu)
#if (BUILDLEVEL<LEVEL8)
  _iq IqRef = _IQ(0.1);				// Iq reference (pu)
#else
  _iq IqRef = _IQ(0.0);			    // Iq reference (pu)
#endif

// Speed reference (pu)
#if (BUILDLEVEL<LEVEL3)
  _iq  SpeedRef = _IQ(0.15);        // For Open Loop tests
#elif BUILDLEVEL < LEVEL6
  _iq  SpeedRef = _IQ(0.3);         // For Closed Loop tests
#elif BUILDLEVEL < LEVEL8
  _iq  SpeedRef = _IQ(0.2);         // For Closed Loop tests
#else
  _iq  SpeedRef = _IQ(0.101);       // IPD + FHI + eSMO
#endif

float32 T = 0.001/ISR_FREQUENCY;    // Samping period (sec), see parameter.h

Uint32 IsrTicker = 0;
Uint16 BackTicker = 0;
Uint16 lsw=0;
Uint16 TripFlagDMC=0;				// PWM trip status

// Default ADC initialization
int ChSel[16]   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int	TrigSel[16] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
int ACQPS[16]   = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

int16 DlogCh1 = 0;
int16 DlogCh2 = 0;
int16 DlogCh3 = 0;
int16 DlogCh4 = 0;


volatile Uint16 EnableFlag = FALSE;
Uint16 LockRotorFlag = FALSE;

Uint16 SpeedLoopPrescaler = 10;      // Speed loop prescaler
Uint16 SpeedLoopCount = 1;           // Speed loop counter

#if BUILDLEVEL < LEVEL7
  // Instance an SMO position estimator
  SMOPOS smo1 = SMOPOS_DEFAULTS;
#endif

// Instance a sliding-mode position observer constant Module
SMOPOS_CONST smo1_const = SMOPOS_CONST_DEFAULTS;

// Instance a QEP interface driver
QEP qep1 = QEP_DEFAULTS;

// Instance a few transform objects
CLARKE clarke1 = CLARKE_DEFAULTS;
PARK park1 = PARK_DEFAULTS;
IPARK ipark1 = IPARK_DEFAULTS;

// Instance PI regulators to regulate the d and q  axis currents, and speed
PI_CONTROLLER pi_spd = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_id  = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_iq  = PI_CONTROLLER_DEFAULTS;

// Instance a PWM driver instance
PWMGEN pwm1 = PWMGEN_DEFAULTS;

// Instance a PWM DAC driver instance
PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

// Instance a Space Vector PWM modulator. This modulator generates a, b and c
// phases based on the d and q stationery reference frame inputs
SVGEN svgen1 = SVGEN_DEFAULTS;

// Instance a ramp controller to smoothly ramp the frequency
RMPCNTL rc1 = RMPCNTL_DEFAULTS;

//	Instance a ramp generator to simulate an Angle
RAMPGEN rg1 = RAMPGEN_DEFAULTS;

//	Instance a phase voltage calculation
PHASEVOLTAGE volt1 = PHASEVOLTAGE_DEFAULTS;

// Instance a speed calculator based on QEP
SPEED_MEAS_QEP speed1 = SPEED_MEAS_QEP_DEFAULTS;

// Instance a speed calculator based on sliding-mode position observer
SPEED_ESTIMATION speed3 = SPEED_ESTIMATION_DEFAULTS;

// Create an instance of DATALOG Module
DLOG_4CH dlog = DLOG_4CH_DEFAULTS;

#if (BUILDLEVEL == LEVEL7) || (BUILDLEVEL == LEVEL8) || (BUILDLEVEL == LEVEL9)
  _iq  startCnt,               // cntr to help transition from lsw 1 --> 2
       smoK_set = _IQ(0.35);   // sliding mode gain - final value

  // Instance an eSMO position estimator
  ESMOPOS  esmo1 = ESMOPOS_DEFAULTS;

  // Instance a PI controller for eSMO angle tuning
  PI_CONTROLLER  pi_smo = PI_CONTROLLER_DEFAULTS;

  // Instance an inverter PWM dead band compensator
  DBC dbc1 = DBC_DEFAULTS;
#endif

#if (BUILDLEVEL == LEVEL8) || (BUILDLEVEL == LEVEL9)

  #define  RESET_STATE      0
  #define  IPD_STATE        1
  #define  RUN_STATE        2

  #define  TRANS_SPD_HI     _IQ(0.10)
  #define  TRANS_SPD_LO     _IQ(0.06)
  #define  TRANS_SCALE      20          /* must be < 1/(HI-LO) such that max k2 ~= 0.8 */

  #define  INDEX_CNT        8
  #define  VOLT_PU(A)       _IQ( A/(sqrt(3)*BASE_VOLTAGE) )
  #define  HFI_VOLT_RUN     50.0        /* max duty cycle below this Vdc (in V) @ RUN */
  #define  HFI_VOLT_IPD     80.0        /* max duty cycle below this Vdc (in V) @ IPD */

// *************************************************
//  Reset PI controller to zero initial conditions
// *************************************************
#define RESET_PI(v)                                             \
	v.Out = 0;                                                  \
    v.i1  = 0;
	
  // Instance a transition regulator to work with various rotor angle estimates
  TRANSITION  transition1 = TRANSITION_DEFAULTS;

  // Instance a high pass filter
  HPF  hpf_Iq = HPF_DEFAULTS;

  // Instance a high pass filter coefficients structure
  HPF_COEFF   hpf_coeff1 = HPF_COEFF_DEFAULTS;

  // Instance a High Frequency Injection controller
  HFI hfi1 = HFI_DEFAULTS;

  // Instance a magnetic polarity identifier
  NS_ID  ns_id1 = NS_ID_DEFAULTS;
#endif

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
    dlog.size = 0x00c8;
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
    qep1.LineEncoder = 2500;
    qep1.MechScaler = _IQ30(0.25/qep1.LineEncoder);
    qep1.PolePairs = POLES/2;
    qep1.CalibratedAngle = 0;
    QEP_INIT_MACRO(1,qep1)

// Initialize the Speed module for QEP based speed calculation
    speed1.K1 = _IQ21(1/(BASE_FREQ*T));
    speed1.K2 = _IQ(1/(1+T*2*PI*5));  // Low-pass cut-off frequency
    speed1.K3 = _IQ(1)-speed1.K2;
    speed1.BaseRpm = 120*(BASE_FREQ/POLES);

// Initialize the SPEED_EST module SMOPOS based speed calculation
    speed3.K1 = _IQ21(1/(BASE_FREQ*T));
    speed3.K2 = _IQ(1/(1+T*2*PI*5));  // Low-pass cut-off frequency
    speed3.K3 = _IQ(1)-speed3.K2;
    speed3.BaseRpm = 120*(BASE_FREQ/POLES);

// Initialize the RAMPGEN module
    rg1.StepAngleMax = _IQ(BASE_FREQ*T);

// Initialize the SMOPOS constant module
	smo1_const.Rs = RS;
	smo1_const.Ls = LS;
	smo1_const.Ib = BASE_CURRENT;
	smo1_const.Vb = BASE_VOLTAGE;
	smo1_const.Ts = T;
	SMO_CONST_MACRO(smo1_const)

#if (BUILDLEVEL <= LEVEL6)// Initialize the SMOPOS module
 	smo1.Fsmopos = _IQ(smo1_const.Fsmopos);
 	smo1.Gsmopos = _IQ(smo1_const.Gsmopos);
 	smo1.Kslide  = _IQ(0.05308703613);
 	smo1.Kslf    = _IQ(0.1057073975);
#else
 	// Initialize eSMO parameters
 	esmo1.Fsmopos  = _IQ(smo1_const.Fsmopos);
 	esmo1.Gsmopos  = _IQ(smo1_const.Gsmopos);
 	esmo1.Kslide   = _IQ(0.05308703613);
	esmo1.base_wTs = _IQ(BASE_FREQ*T);

	// Initialize the PI module for smo angle filter
    pi_smo.Kp   = _IQ(5.0);
 	pi_smo.Ki   = _IQdiv(_IQ(100*0.001/ISR_FREQUENCY), pi_smo.Kp);
 	pi_smo.Umax = _IQ(1.0);
 	pi_smo.Umin = _IQ(-1.0);

 	// Dead Band Compensation (DBC) parameters
	dbc1.gain  = _IQ(0.5);
	dbc1.scale = _IQdiv(_IQdiv2(pwm1.Deadband), dbc1.Ith);
	dbc1.Kdtc  = _IQmpy(dbc1.scale, dbc1.gain);
#endif

#if (BUILDLEVEL == LEVEL8) || (BUILDLEVEL == LEVEL9)
	// Initialize HFI parameters

	// HFI magnitude (duty cycle) parameters
	hfi1.dutyMax  = _IQ(0.65);             //0.24 310V //_IQ(0.4) 180V  // IPD 0.3 squ 16 // 0.4 squ 8
	hfi1.volt_ipd = VOLT_PU(HFI_VOLT_IPD);
	hfi1.volt_run = VOLT_PU(HFI_VOLT_RUN);

	// HFI frequency (timing) parameters
	hfi1.Squ_PRD_set  = INDEX_CNT;        /* 16 IPD // 8 low speed for Bosch // 1 for E-bike */
	hfi1.HFI_Time1 = 400;
	hfi1.HFI_Time2 = 450;

	hfi1.base_wTs = _IQ(BASE_FREQ*T);

	// Initialize HPF parameters
	hpf_coeff1.freq = _IQ(18.0);   // in Hz
	hpf_coeff1.PiT  = _IQ(PI*T);
	HPF_INIT(&hpf_coeff1);

 	// Initialize angle transition parameters
 	transition1.spdLo = TRANS_SPD_LO;
 	transition1.spdHi = TRANS_SPD_HI;
 	transition1.scale = TRANS_SCALE;

 	// Initialize NS determination parameters
 	ns_id1.cntON     = 5;
 	ns_id1.cntPRD    = 30;
 	ns_id1.PWM_ch[0] = 1;		// epwm1
 	ns_id1.PWM_ch[1] = 2;		// epwm2
 	ns_id1.PWM_ch[2] = 3;		// epwm3
 	ns_id1.PWM_PeriodMax = pwm1.PeriodMax;
#endif

// Initialize the PI module for speed
    pi_spd.Kp   = _IQ(1.5);  //10.);    //3.0);//_IQ(1.5);
	pi_spd.Ki   = _IQ(T*SpeedLoopPrescaler/0.2);//_IQ(0.005);  //0.003);  //
	pi_spd.Umax = _IQ(0.95);
	pi_spd.Umin = _IQ(-0.95);

// Initialize the PI module for Id
	pi_id.Kp   = _IQ(1.0);//_IQ(4.0);//_IQ(0.5);
	pi_id.Ki   = _IQ(T/0.01);//_IQ(T/0.04);  //*RS/LS);   ///0.04);
	pi_id.Umax = _IQ(0.3);
	pi_id.Umin = _IQ(-0.3);

// Initialize the PI module for Iq
	pi_iq.Kp   = _IQ(0.5);
	pi_iq.Ki   = _IQ(T/0.04);  //*RS/LS);   ///0.04);
	pi_iq.Umax = _IQ(0.8);
	pi_iq.Umin = _IQ(-0.8);

//  Note that the vectorial sum of d-q PI outputs should be less than 1.0 which refers to maximum duty cycle for SVGEN.
//  Another duty cycle limiting factor is current sense through shunt resistors which depends on hardware/software implementation.
//  Depending on the application requirements 3,2 or a single shunt resistor can be used for current waveform reconstruction.
//  The higher number of shunt resistors allow the higher duty cycle operation and better dc bus utilization.
//  The users should adjust the PI saturation levels carefully during open loop tests (i.e pi_id.Umax, pi_iq.Umax and Umins) as in project manuals.
//  Violation of this procedure yields distorted current waveforms and unstable closed loop operations which may damage the inverter.


//Call HVDMC Protection function
	HVDMC_Protection();

// Reassign ISRs.

	EALLOW;	// This is needed to write to EALLOW protected registers

	PieVectTable.ADCINT1 = &OffsetISR;

// Enable PIE group 1 interrupt 1 for ADC1_INT
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;

// Enable EOC interrupt(after the 4th conversion)

	AdcRegs.ADCINTOVFCLR.bit.ADCINT1=1;
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1=1;
	AdcRegs.INTSEL1N2.bit.INT1CONT=1;  //
	AdcRegs.INTSEL1N2.bit.INT1SEL=4;
	AdcRegs.INTSEL1N2.bit.INT1E=1;

// Enable CPU INT1 for ADC1_INT:
	IER |= M_INT1;

// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;	// Enable Global realtime interrupt DBGM

	EDIS;


// IDLE loop. Just sit and loop forever:
	for(;;)  //infinite loop
	{
		// State machine entry & exit point
		//===========================================================
		(*Alpha_State_Ptr)();	// jump to an Alpha state (A0,B0,...)
		//===========================================================

	}
} //END MAIN CODE


// ****************************************************************************
// ****************************************************************************
// ****************************************************************************

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
void C1(void) 	// Toggle GPIO-34
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

// ****************************************************************************
// ****************************************************************************
// ****************************************************************************

// MainISR
interrupt void MainISR(void)
{
// Verifying the ISR
    IsrTicker++;

// =============================== LEVEL 1 ======================================
//	  Checks target independent modules, duty cycle waveforms and PWM update
//	  Keep the motors disconnected at this level
// ==============================================================================

#if (BUILDLEVEL==LEVEL1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)

// ------------------------------------------------------------------------------
//  Connect inputs of the INV_PARK module and call the inverse park trans. macro
//	There are two option for trigonometric functions:
//  IQ sin/cos look-up table provides 512 discrete sin and cos points in Q30 format
//  IQsin/cos PU functions interpolate the data in the lookup table yielding higher resolution.
// ------------------------------------------------------------------------------
	ipark1.Ds = VdTesting;
	ipark1.Qs = VqTesting;

	//ipark1.Sine  =_IQ30toIQ(IQsinTable[_IQtoIQ9(rg1.Out)]);
	//ipark1.Cosine=_IQ30toIQ(IQcosTable[_IQtoIQ9(rg1.Out)]);

	ipark1.Sine=_IQsinPU(rg1.Out);
	ipark1.Cosine=_IQcosPU(rg1.Out);
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = svgen1.Ta;
	pwmdac1.MfuncC2 = svgen1.Tb;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = svgen1.Tc;
	pwmdac1.MfuncC2 = svgen1.Tb-svgen1.Tc;
	PWMDAC_MACRO(7,pwmdac1)	   						// PWMDAC 7A, 7B

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(svgen1.Ta);
	DlogCh2 = (int16)_IQtoIQ15(svgen1.Tb);
	DlogCh3 = (int16)_IQtoIQ15(svgen1.Tc);
	DlogCh4 = (int16)_IQtoIQ15(svgen1.Tb-svgen1.Tc);

#endif // (BUILDLEVEL==LEVEL1)

// =============================== LEVEL 2 ======================================
//	  Level 2 verifies the analog-to-digital conversion, offset compensation,
//    clarke/park transformations (CLARKE/PARK), phase voltage calculations
// ==============================================================================

#if (BUILDLEVEL==LEVEL2)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha  = clarke1.Alpha;
	park1.Beta   = clarke1.Beta;
	park1.Angle  = rg1.Out;
	park1.Sine   = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);
	PARK_MACRO(park1)

// ------------------------------------------------------------------------------
//	Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds  = VdTesting;
	ipark1.Qs  = VqTesting;
	ipark1.Sine=park1.Sine;
	ipark1.Cosine=park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//  Connect inputs of the VOLT_CALC module and call the phase voltage calc. macro
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);	// DC Bus voltage meas.
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = svgen1.Ta;
	pwmdac1.MfuncC2 = svgen1.Tb;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = svgen1.Tc;
	pwmdac1.MfuncC2 = svgen1.Tb-svgen1.Tc;
	PWMDAC_MACRO(7,pwmdac1)

// ------------------------------------------------------------------------------
//  Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(svgen1.Ta);
	DlogCh2 = (int16)_IQtoIQ15(volt1.VphaseA);
	DlogCh3 = (int16)_IQtoIQ15(clarke1.As);
	DlogCh4 = (int16)_IQtoIQ15(clarke1.Bs);


#endif // (BUILDLEVEL==LEVEL2)

// =============================== LEVEL 3 ======================================
//	Level 3 verifies the dq-axis current regulation performed by PID and speed
//	measurement modules
// ==============================================================================
//  lsw=0: lock the rotor of the motor
//  lsw=1: close the current loop


#if (BUILDLEVEL==LEVEL3)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw==0)rc1.TargetValue = 0;
	else rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta = clarke1.Beta;
	if(lsw==0) park1.Angle = 0;
	else if(lsw==1) park1.Angle = rg1.Out;

	park1.Sine = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);

	PARK_MACRO(park1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PID_REG3 module and call the PID IQ controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_iq.Ref = 0;
	else if(lsw==1) pi_iq.Ref = IqRef;
	pi_iq.Fbk = park1.Qs;
	PI_MACRO(pi_iq)

// ------------------------------------------------------------------------------
//  Connect inputs of the PI module and call the PID ID controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_id.Ref = _IQ(0.05);
	else pi_id.Ref = IdRef;
	pi_id.Fbk = park1.Ds;
	PI_MACRO(pi_id)

// ------------------------------------------------------------------------------
//	Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds = pi_id.Out;
	ipark1.Qs = pi_iq.Out ;
	ipark1.Sine   = park1.Sine;
	ipark1.Cosine = park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//    Call the QEP calculation module
// ------------------------------------------------------------------------------
	QEP_MACRO(1,qep1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_FR module and call the speed calculation macro
// ------------------------------------------------------------------------------
	speed1.ElecTheta = _IQ24toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	SPEED_FR_MACRO(speed1)

// ------------------------------------------------------------------------------
//  Connect inputs of the VOLT_CALC module and call the phase voltage calc. macro
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);	// DC Bus voltage meas.
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = speed1.ElecTheta;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = clarke1.As;
	pwmdac1.MfuncC2 = clarke1.Bs;
	PWMDAC_MACRO(7,pwmdac1)

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(rg1.Out);
	DlogCh2 = (int16)_IQtoIQ15(speed1.ElecTheta);
	DlogCh3 = (int16)_IQtoIQ15(clarke1.As);
	DlogCh4 = (int16)_IQtoIQ15(clarke1.Bs);

#endif // (BUILDLEVEL==LEVEL3)


// =============================== LEVEL 4 ======================================
//	  Level 4 verifies the estimated rotor position and speed estimation
//	  performed by SMOPOS and SPEED_EST modules, respectively.
// ==============================================================================
//  lsw=0: lock the rotor of the motor
//  lsw=1: close the current loop

#if (BUILDLEVEL==LEVEL4)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw==0)rc1.TargetValue = 0;
	else rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta  = clarke1.Beta;
	if(lsw==0) park1.Angle = 0;
	else if(lsw==1) park1.Angle = rg1.Out;

	park1.Sine   = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);

	PARK_MACRO(park1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PI module and call the PID IQ controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_iq.Ref = 0;
	else if(lsw==1) pi_iq.Ref = IqRef;
	pi_iq.Fbk = park1.Qs;
	PI_MACRO(pi_iq)

// ------------------------------------------------------------------------------
//  Connect inputs of the PI module and call the PID ID controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_id.Ref = _IQ(0.05);
	else pi_id.Ref = 0;
	pi_id.Fbk = park1.Ds;
	PI_MACRO(pi_id)

// ------------------------------------------------------------------------------
//	Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds = pi_id.Out;
	ipark1.Qs = pi_iq.Out;
	ipark1.Sine=park1.Sine;
	ipark1.Cosine=park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//    Call the QEP calculation module
// ------------------------------------------------------------------------------
	QEP_MACRO(1,qep1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_FR module and call the speed calculation macro
// ------------------------------------------------------------------------------
	speed1.ElecTheta = _IQ24toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	SPEED_FR_MACRO(speed1)

// ------------------------------------------------------------------------------
//  Connect inputs of the VOLT_CALC module and call the phase voltage calc. macro
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SMO_POS module and call the sliding-mode observer macro
// ------------------------------------------------------------------------------
	smo1.Ialpha = clarke1.Alpha;
	smo1.Ibeta  = clarke1.Beta;
	smo1.Valpha = volt1.Valpha;
	smo1.Vbeta  = volt1.Vbeta;
	SMO_MACRO(smo1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_EST module and call the estimated speed macro
// ------------------------------------------------------------------------------
	speed3.EstimatedTheta = smo1.Theta;
	SE_MACRO(speed3)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = qep1.ElecTheta;
	pwmdac1.MfuncC2 = smo1.Theta;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = clarke1.As;
	PWMDAC_MACRO(7,pwmdac1)

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(qep1.ElecTheta);
	DlogCh2 = (int16)_IQtoIQ15(smo1.Theta);
	DlogCh3 = (int16)_IQtoIQ15(rg1.Out);
	DlogCh4 = (int16)_IQtoIQ15(clarke1.As);


#endif // (BUILDLEVEL==LEVEL4)


// =============================== LEVEL 5 ======================================
//	  Level 5 verifies the speed regulator performed by PI module.
//	  The system speed loop is closed by using the measured speed as a feedback.
// ==============================================================================
//  lsw=0: lock the rotor of the motor
//  lsw=1: close the current loop
//  lsw=2: close the speed loop

#if (BUILDLEVEL==LEVEL5)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw==0)rc1.TargetValue = 0;
	else rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta = clarke1.Beta;

	if(lsw==0) park1.Angle = 0;
	else if(lsw==1) park1.Angle = rg1.Out;
	else park1.Angle = smo1.Theta;

	park1.Sine = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);

	PARK_MACRO(park1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID speed controller macro
// ------------------------------------------------------------------------------
   if (SpeedLoopCount==SpeedLoopPrescaler)
	 {
	  pi_spd.Ref = rc1.SetpointValue;
	  pi_spd.Fbk = speed1.Speed;
	  PI_MACRO(pi_spd);
	  SpeedLoopCount=1;
	 }
	else SpeedLoopCount++;

	if(lsw==0 || lsw==1)	{pi_spd.ui=0; pi_spd.i1=0;}

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID IQ controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_iq.Ref = 0;
	else if(lsw==1) pi_iq.Ref = IqRef;
	else pi_iq.Ref =  pi_spd.Out;
	pi_iq.Fbk = park1.Qs;
	PI_MACRO(pi_iq)

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID ID controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_id.Ref = _IQ(0.05);
	else pi_id.Ref = 0;
	pi_id.Fbk = park1.Ds;
	PI_MACRO(pi_id)

// ------------------------------------------------------------------------------
//  Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds = pi_id.Out;
	ipark1.Qs = pi_iq.Out;
	ipark1.Sine=park1.Sine;
	ipark1.Cosine=park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//    Call the QEP calculation module
// ------------------------------------------------------------------------------
	QEP_MACRO(1,qep1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_FR module and call the speed calculation macro
// ------------------------------------------------------------------------------
	speed1.ElecTheta = _IQ24toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	SPEED_FR_MACRO(speed1)

// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_CALC module and call the phase voltage macro
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SMO_POS module and call the sliding-mode observer macro
// ------------------------------------------------------------------------------
	smo1.Ialpha = clarke1.Alpha;
	smo1.Ibeta  = clarke1.Beta;
	smo1.Valpha = volt1.Valpha;
	smo1.Vbeta  = volt1.Vbeta;
	SMO_MACRO(smo1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_EST module and call the estimated speed macro
// ------------------------------------------------------------------------------
	speed3.EstimatedTheta = smo1.Theta;
	SE_MACRO(speed3)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = qep1.ElecTheta;
	pwmdac1.MfuncC2 = smo1.Theta;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = clarke1.As;
	PWMDAC_MACRO(7,pwmdac1)

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(qep1.ElecTheta);
	DlogCh2 = (int16)_IQtoIQ15(rg1.Out);
	DlogCh3 = (int16)_IQtoIQ15(clarke1.As);
	DlogCh4 = (int16)_IQtoIQ15(smo1.Theta);

#endif // (BUILDLEVEL==LEVEL5)

// =============================== LEVEL 6 ======================================
//	  Level 6 verifies the speed regulator performed by PI module.
//	  The system speed loop is closed by using the estimated speed as a feedback.
// ==============================================================================
//  lsw=0: lock the rotor of the motor
//  lsw=1: close the current loop
//  lsw=2: close the speed loop

#if (BUILDLEVEL==LEVEL6)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw==0)rc1.TargetValue = 0;
	else rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta  = clarke1.Beta;

	if(lsw==0) park1.Angle = 0;
	else if(lsw==1) park1.Angle = rg1.Out;
	else park1.Angle = smo1.Theta;

	park1.Sine = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);

	PARK_MACRO(park1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID speed controller macro
// ------------------------------------------------------------------------------
   if (SpeedLoopCount==SpeedLoopPrescaler)
	 {
	  pi_spd.Ref = rc1.SetpointValue;
	  pi_spd.Fbk = speed3.EstimatedSpeed;
	  PI_MACRO(pi_spd);
	  SpeedLoopCount=1;
	 }
	else SpeedLoopCount++;

	if(lsw==0 || lsw==1)	{pi_spd.ui=0; pi_spd.i1=0;}

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID IQ controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_iq.Ref = 0;
	else if(lsw==1) pi_iq.Ref = IqRef;
	else pi_iq.Ref =  pi_spd.Out;
	pi_iq.Fbk = park1.Qs;
	PI_MACRO(pi_iq)

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID ID controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_id.Ref = _IQ(0.05);
	else pi_id.Ref = 0;
	pi_id.Fbk = park1.Ds;
	PI_MACRO(pi_id)

// ------------------------------------------------------------------------------
//  Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds = pi_id.Out;
	ipark1.Qs = pi_iq.Out;
	ipark1.Sine=park1.Sine;
	ipark1.Cosine=park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//    Call the QEP calculation module
// ------------------------------------------------------------------------------
	QEP_MACRO(1,qep1);

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_FR module and call the speed calculation macro
// ------------------------------------------------------------------------------
	speed1.ElecTheta = _IQ24toIQ((int32)qep1.ElecTheta);
	speed1.DirectionQep = (int32)(qep1.DirectionQep);
	SPEED_FR_MACRO(speed1)

// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_CALC module and call the phase voltage macro
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SMO_POS module and call the sliding-mode observer macro
// ------------------------------------------------------------------------------
	if (lsw==2 && smo1.Kslide<_IQ(0.25)) smo1.Kslide=smo1.Kslide+_IQ(0.00001);
	// Low Kslide responds better to loop transients
	// Increase Kslide for better torque response after closing the speed loop

	smo1.Ialpha = clarke1.Alpha;
	smo1.Ibeta  = clarke1.Beta;
	smo1.Valpha = volt1.Valpha;
	smo1.Vbeta  = volt1.Vbeta;
	SMO_MACRO(smo1)

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_EST module and call the estimated speed macro
// ------------------------------------------------------------------------------
	speed3.EstimatedTheta = smo1.Theta;
	SE_MACRO(speed3)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;
	PWM_MACRO(1,2,3,pwm1)							// Calculate the new PWM compare values

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = qep1.ElecTheta;
	pwmdac1.MfuncC2 = smo1.Theta;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = clarke1.As;
	PWMDAC_MACRO(7,pwmdac1)

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(clarke1.As);
	DlogCh2 = (int16)_IQtoIQ15(clarke1.Bs);
	DlogCh3 = (int16)_IQtoIQ15(volt1.VphaseA);
	DlogCh4 = (int16)_IQtoIQ15(qep1.ElecTheta);

#endif // (BUILDLEVEL==LEVEL6)

// =============================== LEVEL 7 ======================================
//	  Level 7 is similar to level 6 except that it uses an enhance SMO for
//    rotor position detection and speed feedback calculation
// ==============================================================================
//  lsw=0: lock the rotor of the motor
//  lsw=1: close the current loop
//  lsw=2: close the speed loop

    /*************************************************************************
     * NOTE:-                                                                *
     * The executable for enhanced SMO is in esmo.lib file.                  *
     * The user is free to evaluate the function of esmo using this          *
     * executable. For access to its source code, the user is advised        *
     * to contact the nearest TI sales office.                               *
     * ***********************************************************************
     */

#if (BUILDLEVEL == LEVEL7)
// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw == 0)
		rc1.TargetValue = 0;
	else
		rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)
	ANGLE_WRAP(rg1.Out);

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)
	clarke1.Cs = -clarke1.As - clarke1.Bs;
//	clarke1.Cs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT3)-offsetC); // Phase C curr.
//	CLARKE1_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha = clarke1.Alpha;
	park1.Beta  = clarke1.Beta;

	if(lsw==0) park1.Angle = 0;
	else if(lsw==1) park1.Angle = rg1.Out;
	else park1.Angle = esmo1.Theta;

	park1.Sine   = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);

	PARK_MACRO(park1)

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID speed controller macro
// ------------------------------------------------------------------------------
   if (SpeedLoopCount==SpeedLoopPrescaler)
	 {
	  pi_spd.Ref = rc1.SetpointValue;
	  pi_spd.Fbk = speed3.EstimatedSpeed;
	  PI_MACRO(pi_spd);
	  SpeedLoopCount=1;
	 }
	else SpeedLoopCount++;

	if(lsw==0 || lsw==1)	{pi_spd.ui=0; pi_spd.i1=0;}

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID IQ controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_iq.Ref = 0;
	else if(lsw==1) pi_iq.Ref = SIGN(rc1.SetpointValue)*IqRef;
	else pi_iq.Ref =  pi_spd.Out;
	pi_iq.Fbk = park1.Qs;
	PI_MACRO(pi_iq)

// ------------------------------------------------------------------------------
//    Connect inputs of the PI module and call the PID ID controller macro
// ------------------------------------------------------------------------------
	if(lsw==0) pi_id.Ref = _IQ(0.05);
	else pi_id.Ref = 0;
	pi_id.Fbk = park1.Ds;
	PI_MACRO(pi_id)

// ------------------------------------------------------------------------------
//  Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds     = pi_id.Out;
	ipark1.Qs     = pi_iq.Out;
	ipark1.Sine   = park1.Sine;
	ipark1.Cosine = park1.Cosine;

	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_CALC module and call the phase voltage macro
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//    Connect inputs of the eSMO_POS module and call the eSMO module
// ------------------------------------------------------------------------------
	// 'startCnt' helps to smoothly change smoFreq during lsw 1->2
	if (lsw != 2)                   startCnt  = _IQ(0.001);
	else if (startCnt < _IQ(1.0))   startCnt += _IQ(0.0001);

	// Low Kslide responds better to loop transients
	// Increase Kslide for better torque response after closing the speed loop
	if ((lsw == 2) && (esmo1.Kslide < smoK_set))
	  esmo1.Kslide += _IQ(0.00001);

	esmo1.Ialpha   = clarke1.Alpha;
	esmo1.Ibeta    = clarke1.Beta;
	esmo1.Valpha   = volt1.Valpha;
	esmo1.Vbeta    = volt1.Vbeta;
	esmo1.runSpeed = _IQmpy(speed3.EstimatedSpeed, startCnt);
	esmo1.cmdSpeed = rc1.SetpointValue;
	eSMO_MODULE(&esmo1);

	/********************************************************************
	 * Supplemental filter to remove jitters from esmo estimated angle
	 * ******************************************************************/
//	esmo1.Theta = angleFilter(&pi_smo, &esmo1);  // optional - uncomment to include

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_EST module and call the estimated speed macro
// ------------------------------------------------------------------------------
	speed3.EstimatedTheta = esmo1.Theta;
	SE_MACRO(speed3)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta  = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	if (lsw != 2) dbc1.gain = 0;
//	else		  dbc1.gain = _IQ(0.5);     // comment this line to tune in debug
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;

	/************************************************************************
	 * PWM Gen w Dead Band Compensation Macro
	 *     if curA > 0   Ta = Ta + Tdt
	 *          else          Ta = Ta - Tdt
	 ************************************************************************/
	dbc1.scale = _IQdiv(_IQdiv2(pwm1.Deadband), dbc1.Ith);  // uncomment to tune in debug
	dbc1.Kdtc  = _IQmpy(dbc1.scale, dbc1.gain);             // uncomment to tune  "  "
	PWMwDBC_MACRO(1,2,3,pwm1,clarke1,dbc1);

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = esmo1.Theta;
	pwmdac1.MfuncC2 = esmo1.Theta;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = speed3.EstimatedTheta;
	PWMDAC_MACRO(7,pwmdac1)	  						// PWMDAC 7A, 7B

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(esmo1.Theta);
	DlogCh2 = (int16)_IQtoIQ15(clarke1.Bs);
	DlogCh3 = (int16)_IQtoIQ15(volt1.VphaseA);
	DlogCh4 = (int16)_IQtoIQ15(qep1.ElecTheta);

#endif // (BUILDLEVEL==LEVEL7)

// =============================== LEVEL 8 ======================================
//	  Level 8 uses high frequency injection to identify rotor position,
//    the user may turn the shaft and view the shaft angle through pwmdac
// ==============================================================================
//  lsw=0: controller in RESET state without powering the motor
//  lsw=1: identify initial rotor position, changes lsw to 0 if IPD fails

    /*************************************************************************
     * REMINDER :-                                                           *
     * The executable for Initial Position Detection (IPD) and               *
     * High Frequency Injection (HFI) are in hfi.lib file, which are not     *
     * part of the downloadable version of control suite.                    *
     * Any attempt to build LEVEL8 code will result in error.                *
     * For access to the library and/or source code, the user is advised     *
     * to contact the nearest TI sales office.                               *
     * ***********************************************************************
     */

#if (BUILDLEVEL==LEVEL8)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw != RUN_STATE) {
		rc1.TargetValue = 0;
		rc1.SetpointValue = 0;
	}
	else
		rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)
	ANGLE_WRAP(rg1.Out);

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_EST module and call the estimated speed macro
// ------------------------------------------------------------------------------
	speed3.EstimatedTheta = hfi1.thetaEst;

	if (lsw != RUN_STATE)
		speed3.OldEstimatedTheta = speed3.EstimatedTheta;
	SE_MACRO(speed3)

// ------------------------------------------------------------------------------
//  Measure the DC bus voltage in _IQ format
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)
//	clarke1.Cs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT3)-offsetC); // Phase C curr.
//	CLARKE1_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha  = clarke1.Alpha;
	park1.Beta   = clarke1.Beta;
	park1.Angle  = speed3.EstimatedTheta;
	park1.Sine   = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);
	PARK_MACRO(park1)

	hpf_Iq.In1 = park1.Qs;

// ------------------------------------------------------------------------------
// State Machine for motor control
// ------------------------------------------------------------------------------
	if(lsw == RESET_STATE)
	{
	  // reset all PI regulators
	  RESET_PI(pi_spd);
	  RESET_PI(pi_id);
	  RESET_PI(pi_iq);

	  HFI_RESET(&hfi1);          // reset HFI module
	  HPF_RESET(&hpf_Iq);        // reset HPF
	  HPF_INIT(&hpf_coeff1);     // optional - to change HPF corner freq during debug

	  dbc1.gain  = 0;
	}

	else if (lsw == IPD_STATE)
	{
	  ZLSPD(&hpf_Iq, &hpf_coeff1, &hfi1, &ns_id1, &clarke1, &volt1);
	  pi_id.Out = hfi1.duty;
	  pi_iq.Out = 0;
	  if(hfi1.HFI_Status == HFI_RESET_STATE)
	  {
		  lsw = RESET_STATE;
	  }
	}

// ------------------------------------------------------------------------------
//  Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds     = pi_id.Out;
	ipark1.Qs     = pi_iq.Out;
	ipark1.Sine   = park1.Sine;
	ipark1.Cosine = park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta  = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;

	if (hfi1.HFI_Status != HFI_NSID_STATE)	    // not NSID state
	{
		/**********************************************
		 * PWM Gen w Dead Band Compensation Macro
		 *     if curA > 0   Ta = Ta + Tdt
		 *     else          Ta = Ta - Tdt
		 **********************************************/
		dbc1.scale = _IQdiv(_IQdiv2(pwm1.Deadband), dbc1.Ith);  // uncomment to tune in debug
		dbc1.Kdtc  = _IQmpy(dbc1.scale, dbc1.gain);             // uncomment to tune  "  "
		PWMwDBC_MACRO(1,2,3,pwm1,clarke1,dbc1);
	}

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = speed3.EstimatedTheta;
	pwmdac1.MfuncC2 = pi_id.Out;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = pi_iq.Out;
	PWMDAC_MACRO(7,pwmdac1)	  						// PWMDAC 7A, 7B

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(rg1.Out);
	DlogCh2 = (int16)_IQtoIQ15(clarke1.Bs);
	DlogCh3 = (int16)_IQtoIQ15(volt1.VphaseA);
	DlogCh4 = (int16)_IQtoIQ15(qep1.ElecTheta);

#endif // (BUILDLEVEL==LEVEL8)


// =============================== LEVEL 9 ======================================
//	  Level 9 uses high frequency injection at lower speeds to identify
//    rotor position, and uses eSMO discussed in LEVEL7 at higher speeds,
//    providing speed control over the entire range
// ==============================================================================
//  lsw=0: controller in RESET state without powering the motor
//  lsw=1: identify initial rotor position, change lsw to 2 if success else to 0
//  lsw=2: motor runs in speed loop

    /*************************************************************************
     * NOTE:-                                                                *
     * The executable for Initial Position Detection (IPD) and               *
     * High Frequency Injection (HFI) are in hfi.lib file, which are not     *
     * part of the downloadable version of control suite.                    *
     * Any attempt to build LEVEL9 code will result in error.                *
     * For access to the library and/or source code, the user is advised     *
     * to contact the nearest TI sales office.                               *
     * ***********************************************************************
     */

#if (BUILDLEVEL==LEVEL9)

// ------------------------------------------------------------------------------
//  Connect inputs of the RMP module and call the ramp control macro
// ------------------------------------------------------------------------------
	if(lsw != RUN_STATE) {
		rc1.TargetValue = 0;
		rc1.SetpointValue = 0;
	}
	else
		rc1.TargetValue = SpeedRef;
	RC_MACRO(rc1)

// ------------------------------------------------------------------------------
//  Connect inputs of the RAMP GEN module and call the ramp generator macro
// ------------------------------------------------------------------------------
	rg1.Freq = rc1.SetpointValue;
	RG_MACRO(rg1)
	ANGLE_WRAP(rg1.Out);

// ------------------------------------------------------------------------------
//    Connect inputs of the SPEED_EST module and call the estimated speed macro
// ------------------------------------------------------------------------------
	transition1.spd      = rc1.SetpointValue;
	transition1.angleHFI = hfi1.thetaEst;
	transition1.angleSMO = esmo1.Theta;
	ANGLE_TRANSIT(&transition1);

	speed3.EstimatedTheta = transition1.angle;

	if (lsw != RUN_STATE)
		speed3.OldEstimatedTheta = speed3.EstimatedTheta;
	SE_MACRO(speed3)

// ------------------------------------------------------------------------------
//  Measure the DC bus voltage in _IQ format
// ------------------------------------------------------------------------------
	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);

// ------------------------------------------------------------------------------
//  Measure phase currents, subtract the offset and normalize from (-0.5,+0.5) to (-1,+1).
//	Connect inputs of the CLARKE module and call the clarke transformation macro
// ------------------------------------------------------------------------------
	clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1)-offsetA); // Phase A curr.
	clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2)-offsetB); // Phase B curr.
	CLARKE_MACRO(clarke1)
//	clarke1.Cs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT3)-offsetC); // Phase C curr.
//	CLARKE1_MACRO(clarke1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PARK module and call the park trans. macro
// ------------------------------------------------------------------------------
	park1.Alpha  = clarke1.Alpha;
	park1.Beta   = clarke1.Beta;
	park1.Angle  = speed3.EstimatedTheta;
	park1.Sine   = _IQsinPU(park1.Angle);
	park1.Cosine = _IQcosPU(park1.Angle);
	PARK_MACRO(park1)

	hpf_Iq.In1 = park1.Qs;

// ------------------------------------------------------------------------------
// State Machine for motor control
// ------------------------------------------------------------------------------
	if(lsw == RESET_STATE)
	{
	  // reset all PI regulators
	  RESET_PI(pi_spd);
	  RESET_PI(pi_id);
	  RESET_PI(pi_iq);

	  HFI_RESET(&hfi1);          // reset HFI module
	  HPF_RESET(&hpf_Iq);        // reset HPF
	  HPF_INIT(&hpf_coeff1);     // optional - to change HPF corner freq during debug

	  dbc1.gain  = 0;
	}

	else if (lsw == IPD_STATE)
	{
	  ZLSPD(&hpf_Iq, &hpf_coeff1, &hfi1, &ns_id1, &clarke1, &volt1);
	  pi_id.Out = hfi1.duty;
	  pi_iq.Out = 0;
	  if(hfi1.HFI_Status == HFI_RUN_STATE)
	  {
		  lsw = RUN_STATE;
		  pi_id.Out = 0;
	  }
	  else if(hfi1.HFI_Status == HFI_RESET_STATE)
	  {
		  lsw = RESET_STATE;
	  }
	}

	else if (lsw == RUN_STATE)
	{
	  // ------------------------------------------------------------------------------
	  //    Connect inputs of the PI module and call the PID speed controller macro
	  // ------------------------------------------------------------------------------
	  if (++SpeedLoopCount >= SpeedLoopPrescaler)
	  {
		pi_spd.Ref = rc1.SetpointValue;
		pi_spd.Fbk = speed3.EstimatedSpeed;
		PI_MACRO(pi_spd);
		SpeedLoopCount=0;
	  }

	  // ------------------------------------------------------------------------------
	  //    Connect inputs of the PI module and call the PID IQ controller macro
	  // ------------------------------------------------------------------------------
	  pi_iq.Ref = pi_spd.Out;
	  pi_iq.Fbk = park1.Qs;
	  PI_MACRO(pi_iq);

	  // ------------------------------------------------------------------------------
	  //    Connect inputs of the PI module and call the PID ID controller macro
	  // ------------------------------------------------------------------------------
	  pi_id.Ref = 0;
	  pi_id.Fbk = park1.Ds;
	  PI_MACRO(pi_id);

	  // Zero and low speed position detection
	  if ( _IQabs(rc1.SetpointValue) <= transition1.spdHi )
	  {
		ZLSPD(&hpf_Iq, &hpf_coeff1, &hfi1, &ns_id1, &clarke1, &volt1);
		pi_id.Out += hfi1.duty;
	  }

//	  dbc1.gain  = _IQ(0.5);             // comment to tune in debug environment
	}

// ------------------------------------------------------------------------------
//  Connect inputs of the INV_PARK module and call the inverse park trans. macro
// ------------------------------------------------------------------------------
	ipark1.Ds     = pi_id.Out;
	ipark1.Qs     = pi_iq.Out;
	ipark1.Sine   = park1.Sine;
	ipark1.Cosine = park1.Cosine;
	IPARK_MACRO(ipark1)

// ------------------------------------------------------------------------------
//    Connect inputs of the VOLT_CALC module and call the phase voltage macro
// ------------------------------------------------------------------------------
//	volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7);
	volt1.MfuncV1 = svgen1.Ta;
	volt1.MfuncV2 = svgen1.Tb;
	volt1.MfuncV3 = svgen1.Tc;
	PHASEVOLT_MACRO(volt1)

// ------------------------------------------------------------------------------
//    Connect inputs of the eSMO_POS module and call the eSMO module
// ------------------------------------------------------------------------------
	if ((lsw == RUN_STATE) && (esmo1.Kslide  < smoK_set))
	  esmo1.Kslide += _IQ(0.00001);
	esmo1.Ialpha   = clarke1.Alpha;
	esmo1.Ibeta    = clarke1.Beta;
	esmo1.Valpha   = volt1.Valpha;
	esmo1.Vbeta    = volt1.Vbeta;
	esmo1.runSpeed = speed3.EstimatedSpeed;
	esmo1.cmdSpeed = rc1.SetpointValue;
	eSMO_MODULE(&esmo1);

	/********************************************************************
	 * Supplemental filter to remove jitters from esmo estimated angle
	 * ******************************************************************/
//	esmo1.Theta = angleFilter(&pi_smo, &esmo1);  // Uncomment to include

// ------------------------------------------------------------------------------
//  Connect inputs of the SVGEN_DQ module and call the space-vector gen. macro
// ------------------------------------------------------------------------------
	svgen1.Ualpha = ipark1.Alpha;
	svgen1.Ubeta  = ipark1.Beta;
	SVGENDQ_MACRO(svgen1)

// ------------------------------------------------------------------------------
//  Connect inputs of the PWM_DRV module and call the PWM signal generation macro
// ------------------------------------------------------------------------------
	if (lsw != RUN_STATE) 	dbc1.gain = 0;
//	else		  dbc1.gain = _IQ(0.5);         // comment line to tune in debug
	pwm1.MfuncC1 = svgen1.Ta;
	pwm1.MfuncC2 = svgen1.Tb;
	pwm1.MfuncC3 = svgen1.Tc;

	if (hfi1.HFI_Status != HFI_NSID_STATE)	    // not NSID state
	{
		/**********************************************
		 * PWM Gen w Dead Band Compensation Macro
		 *     if curA > 0   Ta = Ta + Tdt
		 *     else          Ta = Ta - Tdt
		 **********************************************/
		dbc1.scale = _IQdiv(_IQdiv2(pwm1.Deadband), dbc1.Ith);  // uncomment to tune in debug
		dbc1.Kdtc  = _IQmpy(dbc1.scale, dbc1.gain);             // uncomment to tune  "  "
		PWMwDBC_MACRO(1,2,3,pwm1,clarke1,dbc1);
	}

// ------------------------------------------------------------------------------
//    Connect inputs of the PWMDAC module
// ------------------------------------------------------------------------------
	pwmdac1.MfuncC1 = speed3.EstimatedTheta;
	pwmdac1.MfuncC2 = pi_id.Out;
	PWMDAC_MACRO(6,pwmdac1)	  						// PWMDAC 6A, 6B

	pwmdac1.MfuncC1 = rg1.Out;
	pwmdac1.MfuncC2 = pi_iq.Out;
	PWMDAC_MACRO(7,pwmdac1)	  						// PWMDAC 7A, 7B

// ------------------------------------------------------------------------------
//    Connect inputs of the DATALOG module
// ------------------------------------------------------------------------------
	DlogCh1 = (int16)_IQtoIQ15(rg1.Out);
	DlogCh2 = (int16)_IQtoIQ15(clarke1.Bs);
	DlogCh3 = (int16)_IQtoIQ15(volt1.VphaseA);
	DlogCh4 = (int16)_IQtoIQ15(qep1.ElecTheta);

#endif // (BUILDLEVEL==LEVEL9)



// ------------------------------------------------------------------------------
//    Call the DATALOG update function.
// ------------------------------------------------------------------------------
    dlog.update(&dlog);


// Enable more interrupts from this timer
	AdcRegs.ADCINTFLG.bit.ADCINT1=1;

// Acknowledge interrupt to receive more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}// MainISR Ends Here


/**********************************************************/
/********************Offset Compensation*******************/
/**********************************************************/

interrupt void OffsetISR(void)
{
// Verifying the ISR
    IsrTicker++;

// DC offset measurement for ADC

    if (IsrTicker>=5000)
    	{
    		offsetA= _IQmpy(K1,offsetA)+_IQmpy(K2,_IQ12toIQ(AdcResult.ADCRESULT1)); 		//Phase A offset
    		offsetB= _IQmpy(K1,offsetB)+_IQmpy(K2,_IQ12toIQ(AdcResult.ADCRESULT2));			//Phase B offset
    		offsetC= _IQmpy(K1,offsetC)+_IQmpy(K2,_IQ12toIQ(AdcResult.ADCRESULT3));			//Phase C offset
    	}

	if (IsrTicker > 20000)
	{
		EALLOW;
		PieVectTable.ADCINT1=&MainISR;
		EDIS;
	}

// Enable more interrupts from this timer
	AdcRegs.ADCINTFLG.bit.ADCINT1=1;

// Acknowledge interrupt to recieve more interrupts from PIE group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

//*************** End of Offset Comp. ********************//
/**********************************************************/
/***************Protection Configuration*******************/
/**********************************************************/

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

//************************** End of Prot. Conf. ***************************//
}


//===========================================================================
// No more.
//===========================================================================


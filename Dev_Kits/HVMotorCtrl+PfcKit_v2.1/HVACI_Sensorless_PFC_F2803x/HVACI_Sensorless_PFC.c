/* ==============================================================================
 System Name:  	HVACI_Sensorless PFC

 File Name:	  	HVACI_Sensorless_PFC.c

 Description:	Primary system file for the Real Implementation of Sensorless
 Field Orientation Control for Induction Motors

 =================================================================================  */

// Header files used in project
#include "PeripheralHeaderIncludes.h"
#define   MATH_TYPE      IQ_MATH
#include "IQmathLib.h"
#include "HVACI_Sensorless_PFC.h"
#include "HVACI_Sensorless_PFC-Settings.h"
#include <math.h>
#include "DPlib.h"
#include "SineAnalyzer.h"

/* Function prototypes */
// Device configuration
void DeviceInit(void);

// Memory initialization
#ifdef FLASH
void InitFlash(void);
#endif
void MemCopy();
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;

// DPLib Function Prototypes
void DPL_Init(void);
interrupt void DPL_ISR(void);
void PWM_2ch_UpDwnCnt_CNF(int16 n, int16 period, int16 mode, int16 phase);

// Motor control function prototypes
#ifdef FLASH
#pragma CODE_SECTION(MotorInvISR, "ramfuncs");
#pragma CODE_SECTION(OffsetISR, "ramfuncs");
#endif
interrupt void MotorInvISR(void);
interrupt void OffsetISR(void);
void HVDMC_Protection(void);

/* State machine function prototypes */
// Alpha states
void A0(void); //state A0
void B0(void); //state B0
void C0(void); //state C0

// A branch states
void A1(void); //state A1
void A2(void); //state A2
void A3(void); //state A3

// B branch states
void B1(void); //state B1
void B2(void); //state B2
void B3(void); //state B3

// C branch states
void C1(void); //state C1
void C2(void); //state C2
void C3(void); //state C3

void (*Alpha_State_Ptr)(void); // Base States pointer
void (*A_Task_Ptr)(void); // State pointer A branch
void (*B_Task_Ptr)(void); // State pointer B branch
void (*C_Task_Ptr)(void); // State pointer C branch

int16 VTimer0[4]; // Virtual Timers slaved off CPU Timer 0 (A events)
int16 VTimer1[4]; // Virtual Timers slaved off CPU Timer 1 (B events)
int16 VTimer2[4]; // Virtual Timers slaved off CPU Timer 2 (C events)

// Sampling period (sec), see HVACI_Sensorless_PFC-Settings.h
float32 T = 0.001 / ISR_FREQUENCY;

// ADC configuration & offset calibration
int ChSel[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int TrigSel[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int ACQPS[16] = { 6, 8, 8, 8, 6, 6, 6, 8, 6, 8, 8, 8, 8, 8, 8, 8 };
_iq offsetA = 0;
_iq offsetB = 0;
_iq offsetC = 0;
_iq K1 = _IQ(0.998); 		// Offset filter coefficient K1: 0.05 / (T + 0.05);
_iq K2 = _IQ(0.001999); 	// Offset filter coefficient K2: T / (T + 0.05);

// IQMath tables
extern _iq IQsinTable[];
extern _iq IQcosTable[];

// Software control variables
volatile Uint16 EnableFlag = FALSE;
Uint16 BackTicker = 0;
Uint32 IsrTicker = 0;
Uint16 lsw = 0;					// Speed loop control switch
Uint16 TripFlagDMC = 0; 		// PWM trip status

/* AC motor control modules & variables */
_iq IdRef = _IQ(0.1); 					// Id reference (pu)
_iq IqRef = _IQ(0.05); 					// Iq reference (pu)
_iq SpeedRef = _IQ(0.3); 				// Speed reference (pu)
Uint16 SpeedLoopCount = 1; 				// Speed loop counter
Uint16 SpeedLoopPrescaler = 10; 		// Speed loop pre-scalar

// Instance rotor flux and speed estimations
ACIFE fe1 = ACIFE_DEFAULTS;
ACISE se1 = ACISE_DEFAULTS;

// Instance the constant calculations for rotor flux and speed estimations
ACIFE_CONST fe1_const = ACIFE_CONST_DEFAULTS;
ACISE_CONST se1_const = ACISE_CONST_DEFAULTS;

// Instance a QEP interface driver 
QEP qep1 = QEP_DEFAULTS;

// Instance a few transform objects (ICLARKE is added in SVGEN module)
CLARKE clarke1 = CLARKE_DEFAULTS;
PARK park1 = PARK_DEFAULTS;
IPARK ipark1 = IPARK_DEFAULTS;

// Instance PI regulators to regulate the d and q  axis currents, and speed
PI_CONTROLLER pi_spd = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_id = PI_CONTROLLER_DEFAULTS;
PI_CONTROLLER pi_iq = PI_CONTROLLER_DEFAULTS;

// Instance a PWM driver instance
PWMGEN pwm1 = PWMGEN_DEFAULTS;

// Instance a PWM DAC driver instance
PWMDAC pwmdac1 = PWMDAC_DEFAULTS;

// Instance a Space Vector PWM modulator. This modulator generates a, b and c
// phases based on the d and q stationary reference frame inputs
SVGEN svgen1 = SVGEN_DEFAULTS;

// Instance a ramp controller to smoothly ramp the frequency
RMPCNTL rc1 = RMPCNTL_DEFAULTS;

// Instance a ramp(sawtooth) generator to simulate an angle
RAMPGEN rg1 = RAMPGEN_DEFAULTS;

// Instance a phase voltage calculation
PHASEVOLTAGE volt1 = PHASEVOLTAGE_DEFAULTS;

// Instance a speed calculator based on QEP
SPEED_MEAS_QEP speed1 = SPEED_MEAS_QEP_DEFAULTS;

// Instance a speed calculator based on capture QEP (for eQep of 280x only)
SPEED_MEAS_CAP speed2 = SPEED_MEAS_CAP_DEFAULTS;

// Create an instance of DATALOG Module
int16 DlogCh1 = 0;
int16 DlogCh2 = 0;
int16 DlogCh3 = 0;
int16 DlogCh4 = 0;
DLOG_4CH dlog = DLOG_4CH_DEFAULTS;

/* DPLib Net Pointers & Variables */
// ADC
extern volatile long *ADCDRV_1ch_Rlt4;	// Vbus
extern volatile long *ADCDRV_1ch_Rlt5;	// Ipfc
extern volatile long *ADCDRV_1ch_Rlt6;	// VL_fb
extern volatile long *ADCDRV_1ch_Rlt8;	// VN_fb

// CONTROL_2P2Z - Instance #1
#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct1, "CNTL_2P2Z_Coef");
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct1;
extern volatile long *CNTL_2P2Z_Ref1;
extern volatile long *CNTL_2P2Z_Out1;
extern volatile long *CNTL_2P2Z_Fdbk1;
extern volatile long *CNTL_2P2Z_Coef1;
extern volatile long CNTL_2P2Z_DBUFF1[10];

// CONTROL_2P2Z - Instance #2
#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct2, "CNTL_2P2Z_Coef");
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct2;
extern volatile long *CNTL_2P2Z_Ref2;
extern volatile long *CNTL_2P2Z_Out2;
extern volatile long *CNTL_2P2Z_Fdbk2;
extern volatile long *CNTL_2P2Z_Coef2;
extern volatile long CNTL_2P2Z_DBUFF2[10];

// PFC - ePWM 4
extern volatile long *PWMDRV_2ch_UpDwnCnt_Duty4;

// PFC_ICMD - Instance #1
extern volatile long *PFC_ICMD_Vcmd1;
extern volatile long *PFC_ICMD_VinvSqr1;
extern volatile long *PFC_ICMD_VacRect1;
extern volatile long *PFC_ICMD_Out1;
extern volatile long PFC_ICMD_VmaxOverVmin1;

//MATH_AVG - Instance #1
extern volatile long *MATH_EMAVG_In1;
extern volatile long *MATH_EMAVG_Out1;
extern volatile long MATH_EMAVG_Multiplier1;

//MATH_AVG - Instance #2
extern volatile long *MATH_EMAVG_In2;
extern volatile long *MATH_EMAVG_Out2;
extern volatile long MATH_EMAVG_Multiplier2;

//PFC_InvRmsSqr - Instance #1
extern volatile long *PFC_InvRmsSqr_In1;
extern volatile long *PFC_InvRmsSqr_Out1;
extern volatile long PFC_InvRmsSqr_VminOverVmax1;
extern volatile long PFC_InvRmsSqr_Vmin1;

// DPLib Variables
volatile long Ipfc, Vbus, VL_fb, VN_fb;
volatile long VbusAvg, VbusVcmd, Ipfc_fltr, PFCIcmd;
volatile long Vrect, VrectAvg, VrectRMS, VinvSqr;
volatile long Freq_Vin;
volatile long DutyA;
volatile long VbusTargetSlewed;				// Slewed set point for the PFC voltage loop.
const long VbusSlewRate = 25600; 			// Voltage loop slew rate adjustment (Q24)
const int init_boost = 10240; 				// Small boost command when PFC is enabled the first time.
volatile long pfc_slew_temp; 				// Used only if implementing slew rate.
long Pgain_V, Igain_V, Dgain_V, Dmax_V;		// Voltage loop coefficients
int16 start_flag, run_flag, OV_flag;

SineAnalyzer sine_mainsV = SineAnalyzer_DEFAULTS;
int16 i;

// History arrays are used for running average calculation (boxcar filter).
// For use with CCS Expressions Window and GUI only, not part of control loop processing.
#define HistorySize 8
Uint32 HistPtr;
Uint16 Hist_Vbus[HistorySize];
Uint16 Hist_VrectAvg[HistorySize];
Uint16 Hist_VrectRMS[HistorySize];
Uint16 Hist_Freq_Vin[HistorySize];

// GUI Variables for use in Expressions Window
int16 Gui_Vbus;			// Q06
int16 Gui_VrectAvg;		// Q06
int16 Gui_VrectRMS;		// Q06
int16 Gui_Freq_Vin;		// Q06

// GUI variable scaling constants
const Uint16 K_Vbus = 33215;
const Uint16 K_VrectAvg = 26226;
const Uint16 K_Vrms = 26226;

// Temporary variables used to calculate GUI values
Uint32 temp_Vbus;
Uint32 temp_VrectAvg;
Uint32 temp_VrectRMS;
Uint32 temp_Freq_Vin;

int16 run_motor;

void main(void) {
	DeviceInit(); // Device life support & GPIO

// Only used if running from FLASH
// Note: the variable FLASH is defined by the compiler
#ifdef FLASH
// Copy time critical code and FLASH setup code to RAM
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the linker files. 
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

// Call Flash Initialization to setup flash wait-states
// This function must reside in RAM
	InitFlash(); // Call the flash wrapper init function
#endif //(FLASH)

	// Waiting for enable flag set
	while (EnableFlag == FALSE) {
		BackTicker++;
	}

	// Timing sync for background loops
	// Timer period definitions found in device specific PeripheralHeaderIncludes.h
	CpuTimer0Regs.PRD.all = mSec1;		// A tasks
	CpuTimer1Regs.PRD.all = mSec50;		// B tasks
	CpuTimer2Regs.PRD.all = mSec10;		// C tasks

	// Tasks State-machine init
	Alpha_State_Ptr = &A0;
	A_Task_Ptr = &A1;
	B_Task_Ptr = &B1;
	C_Task_Ptr = &C1;

	/* Initialize PWM modules */
	// Initialize Motor PWM modules
	pwm1.PeriodMax = SYSTEM_FREQUENCY * 1000000 * T / 2; 	// Pre-scalar X1 (T1), ISR period = T x 1
	pwm1.HalfPerMax = pwm1.PeriodMax / 2;
	pwm1.Deadband = 2.0 * SYSTEM_FREQUENCY; 				// 120 counts -> 2.0 usec for TBCLK = SYSCLK/1
	PWM_INIT_MACRO(1, 2, 3, pwm1);

	// Initialize PFC PWM module
	// Configure ePWM4 for 100kHz switching frequency (60 MHz / 100 kHz = 600)
	PWM_2ch_UpDwnCnt_CNF(4, 600, 1, 0);

	// Initialize ADC SOCs
	ChSel[0] = 6; // Dummy meas. avoid 1st sample issue Rev0 Piccolo*/
	ChSel[1] = 1; // ChSelect: ADC A1-> Phase A Current
	ChSel[2] = 9; // ChSelect: ADC B1-> Phase B Current
	ChSel[3] = 3; // ChSelect: ADC A3-> Phase C Current
	ChSel[4] = 4; // ChSelect: ADC A4-> Vpfc feedback
	ChSel[5] = 6; // ChSelect: ADC A6-> Ipfc feedback
	ChSel[6] = 10;// ChSelect: ADC B2-> VL feedback
	ChSel[7] = 7; // ChSelect: ADC A7-> DC Bus  Voltage
	ChSel[8] = 8; // ChSelect: ADC B0-> VN feedback

	TrigSel[0] = ADCTRIG_EPWM4_SOCB;
	TrigSel[1] = ADCTRIG_EPWM1_SOCA;
	TrigSel[2] = ADCTRIG_EPWM1_SOCA;
	TrigSel[3] = ADCTRIG_EPWM1_SOCA;
	TrigSel[7] = ADCTRIG_EPWM1_SOCA;
	TrigSel[4] = ADCTRIG_EPWM4_SOCB;
	TrigSel[5] = ADCTRIG_EPWM4_SOCB;
	TrigSel[6] = ADCTRIG_EPWM4_SOCB;
	TrigSel[8] = ADCTRIG_EPWM4_SOCB;

	ADC_MACRO_INIT(ChSel, TrigSel, ACQPS);

	// Configure ePWM4 to generate ADC SOC pulses
	EPwm4Regs.ETSEL.bit.SOCBEN = 1; 			// Enable ePWM4 SOCA pulse
	EPwm4Regs.ETSEL.bit.SOCBSEL = ET_CTR_ZERO; 	// SOCA from ePWM4 Zero event
	EPwm4Regs.ETPS.bit.SOCBPRD = ET_1ST; 		// Trigger ePWM4 SOCA on every event

	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
	EPwm4Regs.TBCTL.bit.PHSEN = TB_ENABLE;
	EPwm4Regs.TBCTL.bit.PHSDIR = TB_DOWN;
	EPwm4Regs.TBPHS.half.TBPHS = 150;

	EPwm4Regs.ETCLR.bit.SOCB = 1;				// Clear SOCA flag

	/* Initialize PWMDAC module */
	pwmdac1.PeriodMax = 500; 						// @60Mhz, 1500->20kHz, 1000-> 30kHz, 500->60kHz
	pwmdac1.HalfPerMax = pwmdac1.PeriodMax / 2;
	PWMDAC_INIT_MACRO(6, pwmdac1);					// PWM 6A,6B
	PWMDAC_INIT_MACRO(7, pwmdac1);					// PWM 7A,7B

	/* DPlib initialization */
	DPL_Init();

#if (INCR_BUILD == 1)  // Open Loop Debug only
	// Module connections
	// ADC connections
	ADCDRV_1ch_Rlt4 = &Vbus;
	ADCDRV_1ch_Rlt5 = &Ipfc;
	ADCDRV_1ch_Rlt6 = &VL_fb;
	ADCDRV_1ch_Rlt8 = &VN_fb;

	// Math_avg block connections - Instance 2
	MATH_EMAVG_In2 = &Vbus;
	MATH_EMAVG_Out2 = &VbusAvg;
	MATH_EMAVG_Multiplier2 = _IQ30(0.00025);

	// Connect the PWM Driver duty to an input variable, Open Loop System
	PWMDRV_2ch_UpDwnCnt_Duty4 = &DutyA;

	// Variable initialized for open loop test
	DutyA = _IQ24(0.1);
#endif // (INCR_BUILD == 1)

#if (INCR_BUILD == 2) 	// Closed Current Loop, Open Voltage Loop
	// Module connections
	// ADC connections
	ADCDRV_1ch_Rlt4 = &Vbus;
	ADCDRV_1ch_Rlt5 = &Ipfc;
	ADCDRV_1ch_Rlt6 = &VL_fb;
	ADCDRV_1ch_Rlt8 = &VN_fb;

	// Connect the 2P2Z current loop controller
	CNTL_2P2Z_Ref1 = &PFCIcmd;
	CNTL_2P2Z_Out1 = &DutyA;
	CNTL_2P2Z_Fdbk1 = &Ipfc_fltr;
	CNTL_2P2Z_Coef1 = &CNTL_2P2Z_CoefStruct1.b2;

	// Math_Avg block connections - Instance 2
	MATH_EMAVG_In2 = &Vbus;
	MATH_EMAVG_Out2 = &VbusAvg;
	MATH_EMAVG_Multiplier2 = _IQ30(0.00025);

	// INV_RMS_SQR block connections
	VrectRMS = (sine_mainsV.Vrms) << 9; // Q15 --> Q24, (sine_mainsV.Vrms) is in Q15
	PFC_InvRmsSqr_In1 = &VrectRMS;
	PFC_InvRmsSqr_Out1 = &VinvSqr;
	PFC_InvRmsSqr_VminOverVmax1 = _IQ30(0.1956);	// (80V / 409V)
	PFC_InvRmsSqr_Vmin1 = _IQ24(0.1956);			// (80V / 409V)

	// PFC_ICMD block connections
	PFC_ICMD_Vcmd1 = &VbusVcmd;
	PFC_ICMD_VinvSqr1 = &VinvSqr;
	PFC_ICMD_VacRect1 = &Vrect;
	PFC_ICMD_Out1 = &PFCIcmd;
	PFC_ICMD_VmaxOverVmin1 = _IQ24(3.00);

	// PWM connection
	PWMDRV_2ch_UpDwnCnt_Duty4 = &DutyA;
	DutyA = _IQ24(0.0);

	// Math_Avg block connections - Instance 1
	MATH_EMAVG_In1 = &Vrect;
	MATH_EMAVG_Out1 = &VrectAvg;
	MATH_EMAVG_Multiplier1 = _IQ30(0.000030);

	// Controller coefficients & clamp values for current loop.
	// Values adjusted for DMC REV1.1 Board.
	CNTL_2P2Z_CoefStruct1.b2 = 	_IQ26(0.07124);
	CNTL_2P2Z_CoefStruct1.b1 = _IQ26(-0.3822);
	CNTL_2P2Z_CoefStruct1.b0 = _IQ26(0.4104);

	CNTL_2P2Z_CoefStruct1.a2 = _IQ26(-0.116);
	CNTL_2P2Z_CoefStruct1.a1 = _IQ26(1.116);

	CNTL_2P2Z_CoefStruct1.max = _IQ24(0.99);
	CNTL_2P2Z_CoefStruct1.i_min = _IQ24(-0.98);
	CNTL_2P2Z_CoefStruct1.min = _IQ24(0.0);

	// Variable used to test open voltage loop/closed current loop with light load.
	VbusVcmd = _IQ24(0.05);
#endif // (INCR_BUILD == 2)

#if (INCR_BUILD == 3) 	// Closed Current Loop & Closed Voltage Loop
	// Module connections
	// ADC connections
	ADCDRV_1ch_Rlt4 = &Vbus;
	ADCDRV_1ch_Rlt5 = &Ipfc;
	ADCDRV_1ch_Rlt6 = &VL_fb;
	ADCDRV_1ch_Rlt8 = &VN_fb;

	// Connect the 2P2Z current loop controller.
	CNTL_2P2Z_Ref1 = &PFCIcmd;
	CNTL_2P2Z_Out1 = &DutyA;
	CNTL_2P2Z_Fdbk1 = &Ipfc_fltr;
	CNTL_2P2Z_Coef1 = &CNTL_2P2Z_CoefStruct1.b2;

	// Connect the 2P2Z voltage loop controller.
	CNTL_2P2Z_Ref2 = &VbusTargetSlewed;
	CNTL_2P2Z_Out2 = &VbusVcmd;
	CNTL_2P2Z_Fdbk2 = &Vbus;
	CNTL_2P2Z_Coef2 = &CNTL_2P2Z_CoefStruct2.b2;
	VbusVcmd = _IQ24(0.0);

	// Math_Avg block connections - Instance 2
	MATH_EMAVG_In2 = &Vbus;
	MATH_EMAVG_Out2 = &VbusAvg;						// Average PFC bus volt calculated for OV protection
	MATH_EMAVG_Multiplier2 = _IQ30(0.00025);

	// INV_RMS_SQR block connections
	VrectRMS = (sine_mainsV.Vrms) << 9;				// Q15 --> Q24, (sine_mainsV.Vrms) is in Q15
	PFC_InvRmsSqr_In1 = &VrectRMS;
	PFC_InvRmsSqr_Out1 = &VinvSqr;
	PFC_InvRmsSqr_VminOverVmax1 = _IQ30(0.1956);	// (80V / 409V)
	PFC_InvRmsSqr_Vmin1 = _IQ24(0.1956);

	// PFC_ICMD block connections
	PFC_ICMD_Vcmd1 = &VbusVcmd;
	PFC_ICMD_VinvSqr1 = &VinvSqr;
	PFC_ICMD_VacRect1 = &Vrect;
	PFC_ICMD_Out1 = &PFCIcmd;
	PFC_ICMD_VmaxOverVmin1 = _IQ24(3.00);

	// PWM connection
	PWMDRV_2ch_UpDwnCnt_Duty4 = &DutyA;
	DutyA = _IQ24(0.0);

	// Math_Avg block connections - Instance 1
	MATH_EMAVG_In1 = &Vrect;
	MATH_EMAVG_Out1 = &VrectAvg;
	MATH_EMAVG_Multiplier1 = _IQ30(0.000015);

	// Controller coefficients & clamp values for current loop.
	// Values adjusted for DMC REV1.1 Board.
	CNTL_2P2Z_CoefStruct1.b2 = _IQ26(0.07124);
	CNTL_2P2Z_CoefStruct1.b1 = _IQ26(-0.3822);
	CNTL_2P2Z_CoefStruct1.b0 = _IQ26(0.4104);

	CNTL_2P2Z_CoefStruct1.a2 = _IQ26(-0.116);
	CNTL_2P2Z_CoefStruct1.a1 = _IQ26(1.116);

	CNTL_2P2Z_CoefStruct1.max = _IQ24(0.99);
	CNTL_2P2Z_CoefStruct1.i_min = _IQ24(-0.98);
	CNTL_2P2Z_CoefStruct1.min = _IQ24(0.0);

	// Voltage Loop Coefficients (50 kHz loop)
	Dmax_V = _IQ24(0.999);		// Max output from voltage loop controller
	Pgain_V = _IQ26(0.25);		// (P=0.25 & I=0.002 results in THD 4.7% at 110Vin)
	Igain_V = _IQ26(0.00025);
	Dgain_V = _IQ26(0.0);

	// Coefficient values in Q26
	CNTL_2P2Z_CoefStruct2.b2 = Dgain_V;
	CNTL_2P2Z_CoefStruct2.b1 = (Igain_V - Pgain_V - Dgain_V - Dgain_V);
	CNTL_2P2Z_CoefStruct2.b0 = (Pgain_V + Igain_V + Dgain_V);

	CNTL_2P2Z_CoefStruct2.a2 = 0.0;
	CNTL_2P2Z_CoefStruct2.a1 = _IQ26(1.0);

	CNTL_2P2Z_CoefStruct2.max = Dmax_V;
	CNTL_2P2Z_CoefStruct2.i_min = _IQ24(0.0);
	CNTL_2P2Z_CoefStruct2.min = _IQ24(0.0);
#endif // (INCR_BUILD >= 3)

	// SineAnalizer initialization
	sine_mainsV.Vin = 0;
	sine_mainsV.SampleFreq = _IQ15(10000.0);
	sine_mainsV.Threshold = _IQ15(0.1);

	// Initialize general PFC variables.
	PFCIcmd = _IQ24(0.0);
	VbusAvg = VbusTargetSlewed = VinvSqr = _IQ24(0.0);
	Vrect = VrectAvg = VrectRMS = _IQ24(0.0);
	HistPtr = 0;
	OV_flag = 0;
	pfc_slew_temp = 0;
	run_flag = 0;
	run_motor = 0;
	start_flag = 0;

	/* Initialize motor control modules */
	// Initialize QEP module
	qep1.LineEncoder = 2048;
	qep1.MechScaler = _IQ30(0.25 / qep1.LineEncoder);
	qep1.PolePairs = POLES / 2;
	qep1.CalibratedAngle = 0;
	QEP_INIT_MACRO(1, qep1);

	// Initialize CAP module
	CAP_INIT_MACRO(1);

	// Initialize the Speed module for QEP based speed calculation
	speed1.K1 = _IQ21(1 / (BASE_FREQ * T));
	speed1.K2 = _IQ(1 / (1 + T * 2 * PI * 5)); // Low-pass cut-off frequency
	speed1.K3 = _IQ(1) - speed1.K2;
	speed1.BaseRpm = 120 * (BASE_FREQ / POLES);

	// Initialize the Speed module for capture eQEP based speed calculation (low speed range)
	speed2.InputSelect = 1;
	speed2.BaseRpm = 120 * (BASE_FREQ / POLES);
	speed2.SpeedScaler = 60 * (SYSTEM_FREQUENCY * 1000000 / (1 * 2048 * speed2.BaseRpm));

	// Initialize the RAMPGEN module
	rg1.StepAngleMax = _IQ(BASE_FREQ * T);

	// Initialize the ACI flux estimator constants module
	fe1_const.Rs = RS;
	fe1_const.Rr = RR;
	fe1_const.Ls = LS;
	fe1_const.Lr = LR;
	fe1_const.Lm = LM;
	fe1_const.Ib = BASE_CURRENT;
	fe1_const.Vb = BASE_VOLTAGE;
	fe1_const.Ts = T;
	ACIFE_CONST_MACRO(fe1_const)

	// Initialize the ACI flux estimator module
	fe1.K1 = _IQ(fe1_const.K1);
	fe1.K2 = _IQ(fe1_const.K2);
	fe1.K3 = _IQ(fe1_const.K3);
	fe1.K4 = _IQ(fe1_const.K4);
	fe1.K5 = _IQ(fe1_const.K5);
	fe1.K6 = _IQ(fe1_const.K6);
	fe1.K7 = _IQ(fe1_const.K7);
	fe1.K8 = _IQ(fe1_const.K8);
	fe1.Kp = _IQ(2.8);
	fe1.Ki = _IQ(T / 0.45);

	// Initialize the ACI speed estimator constants module
	se1_const.Rr = RR;
	se1_const.Lr = LR;
	se1_const.fb = BASE_FREQ;
	se1_const.fc = 3;
	se1_const.Ts = T;
	ACISE_CONST_MACRO(se1_const)

	// Initialize the ACI speed estimator module
	se1.K1 = _IQ(se1_const.K1);
	se1.K2 = _IQ21(se1_const.K2);
	se1.K3 = _IQ(se1_const.K3);
	se1.K4 = _IQ(se1_const.K4);
	se1.BaseRpm = 120 * BASE_FREQ / POLES;

	// Initialize Id PI module
	pi_spd.Kp = _IQ(2.0);
	pi_spd.Ki = _IQ(T * SpeedLoopPrescaler / 0.5);
	pi_spd.Umax = _IQ(0.95);
	pi_spd.Umin = _IQ(-0.95);

	// Initialize Iq PI module
	pi_id.Kp = _IQ(1.0);
	pi_id.Ki = _IQ(T / 0.004);
	pi_id.Umax = _IQ(0.3);
	pi_id.Umin = _IQ(-0.3);

	// Initialize speed PI module
	pi_iq.Kp = _IQ(1.0);
	pi_iq.Ki = _IQ(T / 0.004);
	pi_iq.Umax = _IQ(0.8);
	pi_iq.Umin = _IQ(-0.8);

	// Initialize DATALOG module
	dlog.iptr1 = &DlogCh1;
	dlog.iptr2 = &DlogCh2;
	dlog.iptr3 = &DlogCh3;
	dlog.iptr4 = &DlogCh4;
	dlog.trig_value = 0x1;
	dlog.size = 0x0C8;
	dlog.prescalar = 5;
	dlog.init(&dlog);

	// Note that the vectorial sum of d-q PI outputs should be less than 1.0 which refers to maximum duty cycle for SVGEN.
	// Another duty cycle limiting factor is current sense through shunt resistors which depends on hardware/software implementation.
	// Depending on the application requirements 3,2 or a single shunt resistor can be used for current waveform reconstruction.
	// The higher number of shunt resistors allow the higher duty cycle operation and better dc bus utilization.
	// The users should adjust the PI saturation levels carefully during open loop tests (i.e pi_id.Umax, pi_iq.Umax and Umins) as in project manuals.
	// Violation of this procedure yields distorted current waveforms and unstable closed loop operations which may damage the inverter.

	//Call HVDMC Protection function
	HVDMC_Protection();

	// This is needed to write to EALLOW protected registers
	EALLOW;
	// Reassign ISRs.
	PieVectTable.ADCINT1 = &DPL_ISR;
	PieVectTable.ADCINT2 = &OffsetISR;	// Reassigned to MotorInvISR after calculating feedback offsets

	// Enable PIE group 1 interrupt 1 for ADC1_INT & ADC2_INT
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
	PieCtrlRegs.PIEIER1.bit.INTx2 = 1;

	// Enable EOC interrupt
	AdcRegs.ADCINTOVFCLR.bit.ADCINT1 = 1;
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
	AdcRegs.INTSEL1N2.bit.INT1CONT = 1;
	AdcRegs.INTSEL1N2.bit.INT1SEL = 4;
	AdcRegs.INTSEL1N2.bit.INT1E = 1;

	AdcRegs.ADCINTOVFCLR.bit.ADCINT2 = 1;
	AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
	AdcRegs.INTSEL1N2.bit.INT2CONT = 1;
	AdcRegs.INTSEL1N2.bit.INT2SEL = 7;
	AdcRegs.INTSEL1N2.bit.INT2E = 1;

	// Enable CPU INT1 for ADC1_INT & ADC2_INT
	IER |= M_INT1;

	// Enable global Interrupts and higher priority real-time debug events:
	EINT;		// Enable Global interrupt INTM
	ERTM;		// Enable Global real-time interrupt DBGM

	EDIS;

	// IDLE loop. Just sit and loop forever:
	for (;;) {
		// State machine entry & exit point
		(*Alpha_State_Ptr)(); // jump to an Alpha state (A0,B0,...)
	}
} //END MAIN CODE

//=================================================================================
//	STATE-MACHINE SEQUENCING AND SYNCRONIZATION FOR SLOW BACKGROUND TASKS
//=================================================================================
void A0(void) {
	// loop rate synchronizer for A-tasks
	if (CpuTimer0Regs.TCR.bit.TIF == 1) {
		CpuTimer0Regs.TCR.bit.TIF = 1; // clear flag

		//-----------------------------------------------------------
		(*A_Task_Ptr)(); // jump to an A Task (A1,A2,A3,...)
		//-----------------------------------------------------------

		VTimer0[0]++; // virtual timer 0, instance 0 (spare)
	}

	Alpha_State_Ptr = &B0; // Comment out to allow only A tasks
}

void B0(void) {
	// loop rate synchronizer for B-tasks
	if (CpuTimer1Regs.TCR.bit.TIF == 1) {
		CpuTimer1Regs.TCR.bit.TIF = 1; // clear flag

		//-----------------------------------------------------------
		(*B_Task_Ptr)(); // jump to a B Task (B1,B2,B3,...)
		//-----------------------------------------------------------
		VTimer1[0]++; // virtual timer 1, instance 0 (spare)
	}

	Alpha_State_Ptr = &C0; // Allow C state tasks
}

void C0(void) {
	// loop rate synchronizer for C-tasks
	if (CpuTimer2Regs.TCR.bit.TIF == 1) {
		CpuTimer2Regs.TCR.bit.TIF = 1; // clear flag

		//-----------------------------------------------------------
		(*C_Task_Ptr)(); // jump to a C Task (C1,C2,C3,...)
		//-----------------------------------------------------------
		VTimer2[0]++; //virtual timer 2, instance 0 (spare)
	}

	Alpha_State_Ptr = &A0; // Back to State A0
}

//=================================================================================
//	A - TASKS (executed in every 1 msec)
//=================================================================================
//--------------------------------------------------------
void A1(void) { // SPARE (not used)
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A2
	A_Task_Ptr = &A2;
	//-------------------
}

//-----------------------------------------------------------------
void A2(void) { // SPARE (not used)
	//-------------------
	//the next time CpuTimer0 'counter' reaches Period value go to A3
	A_Task_Ptr = &A3;
	//-------------------
}

//-----------------------------------------
void A3(void) { // SPARE (not used)
	//-----------------
	//the next time CpuTimer0 'counter' reaches Period value go to A1
	A_Task_Ptr = &A1;
	//-----------------
}

//=================================================================================
//	B - TASKS (executed in every 50 msec)
//=================================================================================
//----------------------------------------
void B1(void) {
	if (HistPtr >= HistorySize)
		HistPtr = 0;

	// BoxCar Averages - Input Raw samples into BoxCar arrays
	Hist_Vbus[HistPtr] = AdcResult.ADCRESULT4;	// Vbus

	// VrectAvg and VrectRMS are Q24: convert to Q12
	Hist_VrectAvg[HistPtr] = (int16) (VrectAvg >> 12);
	Hist_VrectRMS[HistPtr] = (int16) (VrectRMS >> 12);

	// Freq_Vin is Q15; convert to Q12
	Hist_Freq_Vin[HistPtr] = (int16) (Freq_Vin >> 12);

	// Measurements
	// View the following variables in Expressions Window as:
	// Gui_Ipfc, Gui_Vbus, Gui_VrectAvg, Gui_VrectRMS, Gui_Freq_Vin (All are Q06)

	// Calculate GUI values which can be seen in the Expressions Window.
	temp_Vbus = temp_VrectAvg = temp_VrectRMS = temp_Freq_Vin = 0;
	for (i = 0; i < HistorySize; ++i){
		temp_Vbus += Hist_Vbus[i];
		temp_VrectAvg += Hist_VrectAvg[i];
		temp_VrectRMS += Hist_VrectRMS[i];
		temp_Freq_Vin += Hist_Freq_Vin[i];
	}

	Gui_Vbus = ((long) temp_Vbus * (long) K_Vbus) >> 15;
	Gui_VrectAvg = ((long) temp_VrectAvg * (long) K_VrectAvg) >> 15;
	Gui_VrectRMS = ((long) temp_VrectRMS * (long) K_Vrms) >> 15;
	Gui_Freq_Vin = ((long) temp_Freq_Vin);

	++HistPtr;

	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B2
	B_Task_Ptr = &B2;
	//-----------------
}

//----------------------------------------
void B2(void) { //  SPARE
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B3
	B_Task_Ptr = &B3;
	//-----------------
}

//----------------------------------------
void B3(void) { //  SPARE
	//-----------------
	//the next time CpuTimer1 'counter' reaches Period value go to B1
	B_Task_Ptr = &B1;
	//-----------------
}

//=================================================================================
//	C - TASKS (executed in every 10 msec)
//=================================================================================
//----------------------------------------
void C1(void) { // Toggle GPIO-34
	// TripZ for PWMs is low (fault trip)
	if (EPwm1Regs.TZFLG.bit.OST == 0x1) {
		TripFlagDMC = 1;
		GpioDataRegs.GPBTOGGLE.bit.GPIO42 = 1;
	}

	// Over Current Prot. for Integrated Power Module is high (fault trip)
	if (GpioDataRegs.GPADAT.bit.GPIO15 == 1) {
		TripFlagDMC = 1;
		GpioDataRegs.GPBTOGGLE.bit.GPIO44 = 1;
	}

	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; // Turn on/off LD3 on the controlCARD

#if(INCR_BUILD == 3)
	if (VbusTargetSlewed == 0) {
		CNTL_2P2Z_Ref2 = 0; // Slewed Voltage Command
	}

	/* Use the "start_flag" within the CCS Expressions Window to start PFC.
	 * "start_flag" starts the boost to target voltage.
	 * The flag ensures the ramp-up only occurs once during execution.
	 * The system must satisfy the two following conditions before using "start_flag":
	 * 	1.  Vbus voltage must be > VBUS_MIN (100 V)
	 * 	2.  VbusTarget command goes from zero to a value > 150 V.
	 */
	if (start_flag == 1 && Vbus > VBUS_MIN) {
		// Start slewing the boost command from a value slightly greater than the PFC output voltage.
		VbusTargetSlewed = Vbus + init_boost;
		CNTL_2P2Z_Ref2 = &VbusTargetSlewed; 		// Slewed Voltage Command

		//-----------------
		//the next time CpuTimer2 'counter' reaches Period value go to C2
		C_Task_Ptr = &C2;
		//-----------------
	}
	else {
		//-----------------
		//the next time CpuTimer2 'counter' reaches Period value go to C3
		C_Task_Ptr = &C3;
		//-----------------
	}
#endif
}

//----------------------------------------
void C2(void) {
	// pfcSlewRate must be a positive value
	pfc_slew_temp = VBUS_TARGET - VbusTargetSlewed;

	// Positive Command - Slewed Vbus is less than VBUS_TARGET, so increase it.
	// This is will implement soft-start for Vbus.  VbusSlewRate is initialized at the beginning of this file.
	if (pfc_slew_temp >= VbusSlewRate){
		VbusTargetSlewed = VbusTargetSlewed + VbusSlewRate;
	}
	else {
		// Soft-start is complete - Set the flag for RUN mode.
		VbusTargetSlewed = VBUS_TARGET;
		start_flag = 0;
		run_flag = 1;
		//-----------------
		//the next time CpuTimer2 'counter' reaches Period value go to C3
		C_Task_Ptr = &C3;
		//-----------------
	}
}

//-----------------------------------------
void C3(void) { // Spare
	//-----------------
	//the next time CpuTimer2 'counter' reaches Period value go to C1
	C_Task_Ptr = &C1;
	//-----------------
}

//MotorInvISR
interrupt void MotorInvISR(void) {
	// Motor ISR is interruptible by the digital power ISR
	EINT;

	// Verifying the ISR
	IsrTicker++;

#if(INCR_BUILD == 3)
	if ((run_flag == 1) && (VbusAvg >= VBUS_MOTOR_START) && (run_motor == 1)) {
		// lsw = 0 (close current loop); lsw = 1 (close speed loop)
		if (rc1.SetpointValue >= _IQ(0.05))
			lsw = 1;

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
		clarke1.As = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT1) - offsetA); // Phase A curr.
		clarke1.Bs = _IQmpy2(_IQ12toIQ(AdcResult.ADCRESULT2) - offsetB); // Phase B curr.
		CLARKE_MACRO(clarke1)

		// ------------------------------------------------------------------------------
		//  Connect inputs of the PARK module and call the park trans. macro
		// ------------------------------------------------------------------------------
		park1.Alpha = clarke1.Alpha;
		park1.Beta = clarke1.Beta;
		park1.Angle = (lsw == 0) ? rg1.Out : fe1.ThetaFlux;
		park1.Sine = _IQsinPU(park1.Angle);
		park1.Cosine = _IQcosPU(park1.Angle);
		PARK_MACRO(park1)

		// ------------------------------------------------------------------------------
		//  Connect inputs of the PI module and call the PID SPD controller macro
		// ------------------------------------------------------------------------------
		if (SpeedLoopCount == SpeedLoopPrescaler) {
			pi_spd.Ref = rc1.SetpointValue;
			pi_spd.Fbk = se1.WrHat;
			PI_MACRO(pi_spd)
			SpeedLoopCount = 1;
		} else
			SpeedLoopCount++;

		if (lsw == 0) {
			pi_spd.ui = 0;
			pi_spd.i1 = 0;
		}

		// ------------------------------------------------------------------------------
		//  Connect inputs of the PI module and call the PID ID controller macro
		// ------------------------------------------------------------------------------
		pi_iq.Ref = (lsw == 0) ? IqRef : pi_spd.Out;
		pi_iq.Fbk = park1.Qs;
		PI_MACRO(pi_iq)

		// ------------------------------------------------------------------------------
		//  Connect inputs of the PI module and call the PID ID controller macro
		// ------------------------------------------------------------------------------
		pi_id.Ref = IdRef;
		pi_id.Fbk = park1.Ds;
		PI_MACRO(pi_id)

		// ------------------------------------------------------------------------------
		//	Connect inputs of the INV_PARK module and call the inverse park trans. macro
		// ------------------------------------------------------------------------------
		ipark1.Ds = pi_id.Out;
		ipark1.Qs = pi_iq.Out;
		ipark1.Sine = park1.Sine;
		ipark1.Cosine = park1.Cosine;
		IPARK_MACRO(ipark1)

		// ------------------------------------------------------------------------------
		//  Call the QEP macro (if incremental encoder used for speed sensing)
		//  Connect inputs of the SPEED_FR module and call the speed calculation macro
		// ------------------------------------------------------------------------------
		QEP_MACRO(1, qep1)

		speed1.ElecTheta = _IQ24toIQ((int32) qep1.ElecTheta);
		speed1.DirectionQep = (int32) (qep1.DirectionQep);
		SPEED_FR_MACRO(speed1)

		// ------------------------------------------------------------------------------
		//  Connect inputs of the VOLT_CALC module and call the phase voltage calc. macro
		// ------------------------------------------------------------------------------
		volt1.DcBusVolt = _IQ12toIQ(AdcResult.ADCRESULT7); // DC Bus voltage meas.
		volt1.MfuncV1 = svgen1.Ta;
		volt1.MfuncV2 = svgen1.Tb;
		volt1.MfuncV3 = svgen1.Tc;
		PHASEVOLT_MACRO(volt1)

		// ------------------------------------------------------------------------------
		//    Connect inputs of the ACI module and call the flux estimation macro
		// ------------------------------------------------------------------------------
		fe1.UDsS = volt1.Valpha;
		fe1.UQsS = volt1.Vbeta;
		fe1.IDsS = clarke1.Alpha;
		fe1.IQsS = clarke1.Beta;
		ACIFE_MACRO(fe1)

		// ------------------------------------------------------------------------------
		//    Connect inputs of the ACI module and call the speed estimation macro
		// ------------------------------------------------------------------------------
		se1.IDsS = clarke1.Alpha;
		se1.IQsS = clarke1.Beta;
		se1.PsiDrS = fe1.PsiDrS;
		se1.PsiQrS = fe1.PsiQrS;
		se1.ThetaFlux = fe1.ThetaFlux;
		ACISE_MACRO(se1)

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
		// Calculate the new PWM compare values
		PWM_MACRO(1, 2, 3, pwm1);

		// ------------------------------------------------------------------------------
		//    Connect inputs of the PWMDAC module
		// ------------------------------------------------------------------------------
		// PWMDAC 6A, 6B
		pwmdac1.MfuncC1 = fe1.PsiQrS;
		pwmdac1.MfuncC2 = fe1.PsiDrS;
		PWMDAC_MACRO(6, pwmdac1);

		// PWMDAC 7A, 7B
		pwmdac1.MfuncC1 = rg1.Out;
		pwmdac1.MfuncC2 = fe1.ThetaFlux;
		PWMDAC_MACRO(7, pwmdac1);
	}
#endif

	// Check for PFC over voltage
#if(INCR_BUILD == 1)
	if(Vbus > VBUS_OVP_THRSHLD)
#else
	if(VbusAvg > VBUS_OVP_THRSHLD)
#endif
	{
		OV_flag = 1;
		EALLOW;
		EPwm4Regs.TZFRC.bit.OST = 1;	// Software forced PWM trip
		EDIS;

		VbusTargetSlewed = 0;
	}

	// Calculate RMS input voltage frequency
	sine_mainsV.Vin = Vrect >> 9;			// IQ15 format
	SineAnalyzer_MACRO(sine_mainsV);
	VrectRMS = (sine_mainsV.Vrms) << 9;		// Convert from Q15 to Q24 and save as VrectRMS
	Freq_Vin = sine_mainsV.SigFreq;			// Q15

	// ------------------------------------------------------------------------------
	//    Connect inputs of the DATALOG module
	// ------------------------------------------------------------------------------
	DlogCh1 = (int16) _IQtoIQ15(clarke1.Bs);
	DlogCh2 = (int16) _IQtoIQ15(clarke1.As);
	DlogCh3 = (int16) _IQtoIQ15(fe1.ThetaFlux);
	DlogCh4 = (int16) _IQtoIQ15(volt1.VphaseA);

	// ------------------------------------------------------------------------------
	//    Call the DATALOG update function.
	// ------------------------------------------------------------------------------
	dlog.update(&dlog);

	// Enable more interrupts from this timer
	AdcRegs.ADCINTFLG.bit.ADCINT2 = 1;

	// Acknowledge interrupt to receive more interrupts from PIE group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

} // End of MainISR

/* Offset Compensation */
interrupt void OffsetISR(void) {
	EINT;

	// Verifying the ISR
	IsrTicker++;

	// DC offset measurement for ADC
	if (IsrTicker >= 5000) {
		offsetA = _IQmpy(K1,offsetA) + _IQmpy(K2,_IQ12toIQ(AdcResult.ADCRESULT1)); //Phase A offset
		offsetB = _IQmpy(K1,offsetB) + _IQmpy(K2,_IQ12toIQ(AdcResult.ADCRESULT2)); //Phase B offset
		offsetC = _IQmpy(K1,offsetC) + _IQmpy(K2,_IQ12toIQ(AdcResult.ADCRESULT3)); //Phase C offset
	}

	if (IsrTicker > 20000) {
		EALLOW;
		PieVectTable.ADCINT2 = &MotorInvISR;
		EDIS;
	}

	// Enable more interrupts from this timer
	AdcRegs.ADCINTFLG.bit.ADCINT2 = 1;

	// Acknowledge interrupt to receive more interrupts from PIE group 1
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
} // End of Offset Compensation

/* Protection Configuration */
void HVDMC_Protection(void) {
	// Configure Trip Mechanism for the Motor control software
	// -Cycle by cycle trip on CPU halt
	// -One shot IPM trip zone trip

	// Trip configuration: EPwm1, 2, 3 & 4
	EALLOW;
	// CPU Halt Trip
	EPwm1Regs.TZSEL.bit.CBC6 = 0x1;
	EPwm2Regs.TZSEL.bit.CBC6 = 0x1;
	EPwm3Regs.TZSEL.bit.CBC6 = 0x1;
	EPwm4Regs.TZSEL.bit.CBC6 = 0x1;

	EPwm1Regs.TZSEL.bit.OSHT1 = 1; //enable TZ1 for OSHT
	EPwm2Regs.TZSEL.bit.OSHT1 = 1; //enable TZ1 for OSHT
	EPwm3Regs.TZSEL.bit.OSHT1 = 1; //enable TZ1 for OSHT
	EPwm4Regs.TZSEL.bit.OSHT1 = 1; //enable TZ1 for OSHT

	// What do we want the OST/CBC events to do?
	// TZA events can force EPWMxA
	// TZB events can force EPWMxB
	EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
	EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

	EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
	EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

	EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
	EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low

	EPwm4Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWMxA will go low
	EPwm4Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWMxB will go low
	EDIS;

	// Clear any spurious OV trip
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm2Regs.TZCLR.bit.OST = 1;
	EPwm3Regs.TZCLR.bit.OST = 1;
	EPwm4Regs.TZCLR.bit.OST = 1;
} // End of Protection Configuration

//===========================================================================
// No more.
//===========================================================================

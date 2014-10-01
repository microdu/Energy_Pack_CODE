// TI File $Revision: /main/2 $
// Checkin $Date: July 31, 2009   14:26:37 $
//###########################################################################
//
// FILE:	Example_280xHRPWM.c
//
// TITLE:	DSP280x Device HRPWM example
//
// ASSUMPTIONS:
//
//
//    This program requires the DSP280x header files.
//
//    Monitor ePWM1-ePWM4 pins on an oscilloscope as described
//    below.
//
//       EPWM1A is on GPIO0
//       EPWM1B is on GPIO1
//
//       EPWM2A is on GPIO2
//       EPWM2B is on GPIO3
//
//       EPWM3A is on GPIO4
//       EPWM3B is on GPIO5
//
//    As supplied, this project is configured for "boot to SARAM"
//    operation.  The 280x Boot Mode table is shown below.
//    For information on configuring the boot mode of an eZdsp,
//    please refer to the documentation included with the eZdsp,
//
//       Boot      GPIO18     GPIO29    GPIO34
//       Mode      SPICLKA    SCITXDA
//                  SCITXB
//       -------------------------------------
//       Flash       1          1        1
//       SCI-A       1          1        0
//       SPI-A       1          0        1
//       I2C-A       1          0        0
//       ECAN-A      0          1        1
//       SARAM       0          1        0  <- "boot to SARAM"
//       OTP         0          0        1
//       I/0         0          0        0
//
//
//
// DESCRIPTION:
//
//       This example modifies the MEP control registers to show edge displacement
//       due to the HRPWM control extension of the respective ePWM module
//       All ePWM1A,2A,3A,4A channels (GPIO0, GPIO2, GPIO4, GPIO6) will have fine edge movement
//       due to HRPWM logic
//
//            1. 10MHz PWM (for 100MHz SYSCLKOUT) or 6 MHz PWM (for 60 MHz SYSCLKOUT),
//                 ePWM1A toggle low/high with MEP control on rising edge
//               10MHz PWM (for 100MHz SYSCLKOUT) or 6 MHz PWM (for 60 MHz SYSCLKOUT),
//                 ePWM1B toggle low/high with NO HRPWM control
//
//            2.  5MHz PWM(for 100MHz SYSCLKOUT) or 3 MHz PWM (for 60 MHz SYSCLKOUT),
//                  ePWM2A toggle low/high with MEP control on rising edge
//                5MHz PWM(for 100MHz SYSCLKOUT) or 3 MHz PWM (for 60 MHz SYSCLKOUT),
//                  ePWM2B toggle low/high with NO HRPWM control
//
//            3. 10MHz PWM(for 100MHz SYSCLKOUT) or 6 MHz PWM (for 60 MHz SYSCLKOUT),
//                 ePWM3A toggle as high/low with MEP control on falling edge
//               10MHz PWM(for 100MHz SYSCLKOUT) or 6 MHz PWM (for 60 MHz SYSCLKOUT),
//                 ePWM3B toggle low/high with NO HRPWM control
//
//            4.  5MHz PWM(for 100MHz SYSCLKOUT) or 3 MHz PWM (for 60 MHz SYSCLKOUT),
//                  ePWM4A toggle as high/low with MEP control on falling edge
//                5MHz PWM(for 100MHz SYSCLKOUT) or 3 MHz PWM (for 60 MHz SYSCLKOUT),
//                  ePWM4B toggle low/high with NO HRPWM control
//
//
//###########################################################################
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"     	// DSP280x Headerfile
#include "DSP280x_Examples.h"       // DSP280x Examples Headerfile
#include "DSP280x_EPwm_defines.h" 	// useful defines for initialization


// Declare your function prototypes here
//---------------------------------------------------------------

void HRPWM1_Config(int);
void HRPWM2_Config(int);
void HRPWM3_Config(int);
void HRPWM4_Config(int);

// General System nets - Useful for debug
Uint16 i,j,	DutyFine, n,update;

Uint32 temp;

void main(void)
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP280x_SysCtrl.c file.
   InitSysCtrl();

// Step 2. Initalize GPIO:
// This example function is found in the DSP280x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();  // Skipped for this example
// For this case, just init GPIO for ePWM1-ePWM4

// For this case just init GPIO pins for ePWM1, ePWM2, ePWM3, ePWM4
// These functions are in the DSP280x_EPwm.c file
   InitEPwm1Gpio();
   InitEPwm2Gpio();
   InitEPwm3Gpio();
   InitEPwm4Gpio();

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
   DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP280x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP280x_DefaultIsr.c.
// This function is found in DSP280x_PieVect.c.
   InitPieVectTable();

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP280x_InitPeripherals.c
// InitPeripherals();  // Not required for this example

// For this example, only initialize the ePWM
// Step 5. User specific code, enable interrupts:

   update =1;
   DutyFine =0;

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
   EDIS;

// Some useful Period vs Frequency values
//  SYSCLKOUT =     100MHz         60 MHz
//  ------------------------------------------------
//	Period	        Frequency      Frequency
//	1000	        100 KHz        60 kHz
//	800		        125 KHz        75 kHz
//	600		        167 KHz        100 kHz
//	500		        200 KHz        120 kHz
//	250		        400 KHz        240 kHz
//	200		        500 KHz        300 kHz
//	100		        1.0 MHz        600 kHz
//	50		        2.0 MHz        1.2 Mhz
//	25		        4.0 MHz        2.4 Mhz
//	20		        5.0 MHz        3.0 Mhz
//	12		        8.33 MHz       5.0 MHz
//	10		        10.0 MHz       6.0 MHz
//	9		        11.1 MHz       6.7 MHz
//	8		        12.5 MHz       7.5 MHz
//	7		        14.3 MHz       8.6 MHz
//	6		        16.7 MHz       10.0 MHz
//	5		        20.0 MHz       12.0 MHz


//====================================================================
// ePWM and HRPWM register initializaition
//====================================================================
   HRPWM1_Config(10);	    // ePWM1 target, 10 MHz PWM (SYSCLK=100MHz), 6 MHz PWM (SYSCLK=60MHz)
   HRPWM2_Config(20);	    // ePWM2 target,  5 MHz PWM (SYSCLK=100MHz), 3 MHz PWM (SYSCLK=60MHz)
   HRPWM3_Config(10);	    // ePWM3 target, 10 MHz PWM (SYSCLK=100MHz), 6 MHz PWM (SYSCLK=60MHz)
   HRPWM4_Config(20);	    // ePWM4 target,  5 MHz PWM (SYSCLK=100MHz), 3 MHz PWM (SYSCLK=60MHz)

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;

   while (update ==1)

	{

        for(DutyFine =1; DutyFine <256 ;DutyFine ++)
        {

        // Example, write to the HRPWM extension of CMPA
        EPwm1Regs.CMPA.half.CMPAHR = DutyFine << 8;     // Left shift by 8 to write into MSB bits
        EPwm2Regs.CMPA.half.CMPAHR = DutyFine << 8;     // Left shift by 8 to write into MSB bits

        // Example, 32-bit write to CMPA:CMPAHR
        EPwm3Regs.CMPA.all = ((Uint32)EPwm3Regs.CMPA.half.CMPA << 16) + (DutyFine << 8);
        EPwm4Regs.CMPA.all = ((Uint32)EPwm4Regs.CMPA.half.CMPA << 16) + (DutyFine << 8);

		for (i=0;i<10000;i++){}                         // Dummy delay between MEP changes
		}
	}

}


void HRPWM1_Config(period)
{
// ePWM1 register configuration with HRPWM
// ePWM1A toggle low/high with MEP control on Rising edge

	EPwm1Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;	        // set Immediate load
	EPwm1Regs.TBPRD = period-1;		                    // PWM frequency = 1 / period
	EPwm1Regs.CMPA.half.CMPA = period / 2;              // set duty 50% initially
    EPwm1Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	EPwm1Regs.CMPB = period / 2;	                    // set duty 50% initially
	EPwm1Regs.TBPHS.all = 0;
	EPwm1Regs.TBCTR = 0;

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;		       // EPWM1 is the Master
	EPwm1Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	EPwm1Regs.AQCTLA.bit.ZRO = AQ_CLEAR;               // PWM toggle low/high
	EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;
	EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;

	EALLOW;
	EPwm1Regs.HRCNFG.all = 0x0;
	EPwm1Regs.HRCNFG.bit.EDGMODE = HR_REP;				//MEP control on Rising edge
	EPwm1Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm1Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;
	EDIS;
}

void HRPWM2_Config(period)
{
// ePWM2 register configuration with HRPWM
// ePWM2A toggle low/high with MEP control on Rising edge

	EPwm2Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;	        // set Immediate load
	EPwm2Regs.TBPRD = period-1;		                    // PWM frequency = 1 / period
	EPwm2Regs.CMPA.half.CMPA = period / 2;              // set duty 50% initially
    EPwm1Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	EPwm2Regs.CMPB = period / 2;	                    // set duty 50% initially
	EPwm2Regs.TBPHS.all = 0;
	EPwm2Regs.TBCTR = 0;

	EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;		         // ePWM2 is the Master
	EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	EPwm2Regs.AQCTLA.bit.ZRO = AQ_CLEAR;                  // PWM toggle low/high
	EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;
	EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR;
	EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;

	EALLOW;
	EPwm2Regs.HRCNFG.all = 0x0;
	EPwm2Regs.HRCNFG.bit.EDGMODE = HR_REP;                //MEP control on Rising edge
	EPwm2Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm2Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;

	EDIS;

}
void HRPWM3_Config(period)
{
// ePWM3 register configuration with HRPWM
// ePWM3A toggle high/low with MEP control on falling edge

	EPwm3Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;	        // set Immediate load
	EPwm3Regs.TBPRD = period-1;		                    // PWM frequency = 1 / period
	EPwm3Regs.CMPA.half.CMPA = period / 2;              // set duty 50% initially
	EPwm3Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	EPwm3Regs.TBPHS.all = 0;
	EPwm3Regs.TBCTR = 0;

	EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;		        // ePWM3 is the Master
	EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;                  // PWM toggle high/low
	EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm3Regs.AQCTLB.bit.ZRO = AQ_SET;
	EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;

	EALLOW;
	EPwm3Regs.HRCNFG.all = 0x0;
	EPwm3Regs.HRCNFG.bit.EDGMODE = HR_FEP;               //MEP control on falling edge
	EPwm3Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm3Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;
	EDIS;
}

void HRPWM4_Config(period)
{
// ePWM4 register configuration with HRPWM
// ePWM4A toggle high/low with MEP control on falling edge

	EPwm4Regs.TBCTL.bit.PRDLD = TB_IMMEDIATE;	        // set Immediate load
	EPwm4Regs.TBPRD = period-1;		                    // PWM frequency = 1 / period
	EPwm4Regs.CMPA.half.CMPA = period / 2;              // set duty 50% initially
	EPwm4Regs.CMPA.half.CMPAHR = (1 << 8);              // initialize HRPWM extension
	EPwm4Regs.CMPB = period / 2;	                    // set duty 50% initially
	EPwm4Regs.TBPHS.all = 0;
	EPwm4Regs.TBCTR = 0;

	EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;
	EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;		        // ePWM4 is the Master
	EPwm4Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
	EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV1;

	EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
	EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

	EPwm4Regs.AQCTLA.bit.ZRO = AQ_SET;                  // PWM toggle high/low
	EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR;
	EPwm4Regs.AQCTLB.bit.ZRO = AQ_SET;
	EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR;

	EALLOW;
	EPwm4Regs.HRCNFG.all = 0x0;
	EPwm4Regs.HRCNFG.bit.EDGMODE = HR_FEP;              //MEP control on falling edge
	EPwm4Regs.HRCNFG.bit.CTLMODE = HR_CMP;
	EPwm4Regs.HRCNFG.bit.HRLOAD  = HR_CTR_ZERO;
	EDIS;
}












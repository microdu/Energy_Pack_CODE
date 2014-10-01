// TI File $Revision: /main/8 $
// Checkin $Date: March 27, 2007   15:30:32 $
//###########################################################################
//
// FILE:   Example_280xAdc.c
//
// TITLE:  DSP280x ADC Example Program.
//
// ASSUMPTIONS:
//
//   This program requires the DSP280x header files.
//
//   Make sure the CPU clock speed is properly defined in
//   DSP280x_Examples.h before compiling this example.
//
//   Connect signals to be converted to A2 and A3.
//
//    As supplied, this project is configured for "boot to SARAM"
//    operation.  The 280x Boot Mode table is shown below.
//    For information on configuring the boot mode of an eZdsp,
//    please refer to the documentation included with the eZdsp,
//
//       Boot      GPIO18     GPIO29    GPIO34
//       Mode      SPICLKA    SCITXDA
//                 SCITXB
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
// DESCRIPTION:
//
//   This example sets up the PLL in x10/2 mode (for 100 Mhz devices)
//   or x6/2 mode (for 60 Mhz devices), divides SYSCLKOUT
//   by eight to reach a 12.5Mhz (100 Mhz devices) or a 7.5 Mhz (60 Mhz
//   devices) HSPCLK (assuming a 20Mhz XCLKIN)
//   Interrupts are enabled and the ePWM1 is setup to generate a periodic
//   ADC SOC on SEQ1. Two channels are converted, ADCINA3 and ADCINA2.
//
//   Watch Variables:
//
//         Voltage1[10]     Last 10 ADCRESULT0 values
//         Voltage2[10]     Last 10 ADCRESULT1 values
//         ConversionCount  Current result number 0-9
//         LoopCount        Idle loop counter
//
//
//###########################################################################
//
// Original Author: D.F.
//
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "DSP280x_Examples.h"   // DSP280x Examples Include File

// Prototype statements for functions found within this file.
interrupt void adc_isr(void);

// Global variables used in this example:
Uint16 LoopCount;
Uint16 ConversionCount;
Uint16 Voltage1[10];
Uint16 Voltage2[10];


main()
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP280x_SysCtrl.c file.
   InitSysCtrl();

// For this example, set HSPCLK to SYSCLKOUT / 8 (12.5Mhz assuming 100Mhz SYSCLKOUT or 7.5 MHz assuming 60 MHz SYSCLKOUT)
   EALLOW;
   SysCtrlRegs.HISPCP.all = 0x4;  // HSPCLK = SYSCLKOUT/8
   EDIS;

// Step 2. Initialize GPIO:
// This example function is found in the DSP280x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();  // Skipped for this example

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

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
   EALLOW;  // This is needed to write to EALLOW protected register
   PieVectTable.ADCINT = &adc_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP280x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
   InitAdc();  // For this example, init the ADC

// Step 5. User specific code, enable interrupts:

// Enable ADCINT in PIE
   PieCtrlRegs.PIEIER1.bit.INTx6 = 1;
   IER |= M_INT1; // Enable CPU Interrupt 1
   EINT;          // Enable Global interrupt INTM
   ERTM;          // Enable Global realtime interrupt DBGM

   LoopCount = 0;
   ConversionCount = 0;

// Configure ADC
   AdcRegs.ADCMAXCONV.all = 0x0001;       // Setup 2 conv's on SEQ1
   AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x3; // Setup ADCINA3 as 1st SEQ1 conv.
   AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x2; // Setup ADCINA2 as 2nd SEQ1 conv.
   AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;// Enable SOCA from ePWM to start SEQ1
   AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)

// Assumes ePWM1 clock is already enabled in InitSysCtrl();
   EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // Enable SOC on A group
   EPwm1Regs.ETSEL.bit.SOCASEL = 4;       // Select SOC from from CPMA on upcount
   EPwm1Regs.ETPS.bit.SOCAPRD = 1;        // Generate pulse on 1st event
   EPwm1Regs.CMPA.half.CMPA = 0x0080;	  // Set compare A value
   EPwm1Regs.TBPRD = 0xFFFF;              // Set period for ePWM1
   EPwm1Regs.TBCTL.bit.CTRMODE = 0;		  // count up and start

// Wait for ADC interrupt
   for(;;)
   {
      LoopCount++;
   }

}


interrupt void  adc_isr(void)
{

  Voltage1[ConversionCount] = AdcRegs.ADCRESULT0 >>4;
  Voltage2[ConversionCount] = AdcRegs.ADCRESULT1 >>4;

  // If 40 conversions have been logged, start over
  if(ConversionCount == 9)
  {
     ConversionCount = 0;
  }
  else ConversionCount++;

  // Reinitialize for next ADC sequence
  AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
  AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
  PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

  return;
}




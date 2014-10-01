// TI File $Revision: /main/8 $
// Checkin $Date: April 4, 2007   17:18:33 $
//###########################################################################
//
// FILE:    Example_280xECap_apwm.c
//
// TITLE:   DSP280x ECAP APWM Example
//
// ASSUMPTIONS:
//
//    This program requires the DSP280x header files.
//
//    Monitor eCAP1 - eCAP4 pins on a oscilloscope as
//    described below.
//
//       eCAP1 on GPIO24
//       eCAP2 on GPIO7
//       eCAP3 on GPIO9
//       eCAP4 on GPIO11
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
//
// DESCRIPTION:
//
//    This program sets up the eCAP pins in the APWM mode.
//    This program runs at 100 MHz SYSCLKOUT assuming a 20 MHz
//    XCLKIN by DEFAULT.
//    --------------------------------------------------------------
//    CODE MODIFICATIONS ARE REQUIRED FOR 60 MHZ DEVICES (In
//    DSP280x_Examples.h in the common/include/ directory, set
//    #define CPU_FRQ_60MHZ to 1, and #define CPU_FRQ_100MHZ to 0).
//    --------------------------------------------------------------
//    For 100 MHz devices:
//
//    eCAP1 will come out on the GPIO24 pin
//    This pin is configured to vary between 5 Hz and 10 Hz using
//    the shadow registers to load the next period/compare values
//
//    eCAP2 will come out on the GPIO7 pin
//    this pin is configured as a 5 Hz output
//
//    eCAP3 will come out on the GPIO9 pin
//    this pin is configured as a 1 Hz output
//
//    eCAP4 will come out on the GPIO11 pin
//    this pin is configured as a 20kHz output
//
//    All frequencies assume a 20 Mhz input clock. The XCLKOUT pin
//    should show 100Mhz.
//
//    --------------------------------------------------------------
//    For 60 MHz devices:
//
//    eCAP1 will come out on the GPIO24 pin
//    This pin is configured to vary between 3 Hz and 6 Hz using
//    the shadow registers to load the next period/compare values
//
//    eCAP2 will come out on the GPIO7 pin
//    this pin is configured as a 3 Hz output
//
//    eCAP3 will come out on the GPIO9 pin
//    this pin is configured as a 1 Hz output
//    * See below - uncomment code for 60 MHz and comment
//                  code for 100 MHz
//
//    eCAP4 will come out on the GPIO11 pin
//    this pin is configured as a 12 kHz output
//
//    All frequencies assume a 20 Mhz input clock. The XCLKOUT pin
//    should show 60Mhz.
//    --------------------------------------------------------------
//    Watch Variables:
//
//       ERR_LOG (bit 15 is set to remind user to visually
//       check the PWM action on the CAP pins. The default
//       configuration in this example should output a 5 Hz
//       PWM signal on the ECAP1 and the ECAP2 pins with a
//       100Mhz SYSCLKOUT, or a 3 MHz PWM signal with a
//       60 MHz SYSCLKOUT assuming a 20Mhz input clock is used.
//
//###########################################################################
// Original Author: D.F.
//
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "DSP280x_Examples.h"   // DSP280x Examples Include File

// Global variables
Uint16 direction = 0;

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

// Initialize the GPIO pins for eCAP.
// This function is found in the DSP280x_ECap.c file
   InitECapGpio();

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
// No interrupts used for this example.

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP280x_InitPeripherals.c
// InitPeripherals(); // Not required for this example

// Step 5. User specific code


   // Setup APWM mode on CAP1, set period and compare registers
   ECap1Regs.ECCTL2.bit.CAP_APWM = 1;	// Enable APWM mode
   ECap1Regs.CAP1 = 0x01312D00;			// Set Period value
   ECap1Regs.CAP2 = 0x00989680;			// Set Compare value
   ECap1Regs.ECCLR.all = 0x0FF;			// Clear pending interrupts
   ECap1Regs.ECEINT.bit.CTR_EQ_CMP = 1; // enable Compare Equal Int

   // Setup APWM mode on CAP2, set period and compare registers
   ECap2Regs.ECCTL2.bit.CAP_APWM = 1;	// Enable APWM mode
   ECap2Regs.CAP1 = 0x01312D00;			// Set Period value
   ECap2Regs.CAP2 = 0x00989680;			// Set Compare value
   ECap2Regs.ECCLR.all = 0x0FF;			// Clear pending interrupts
   ECap1Regs.ECEINT.bit.CTR_EQ_CMP = 1; // enable Compare Equal Int

   // Setup APWM mode on CAP3, set period and compare registers
   ECap3Regs.ECCTL2.bit.CAP_APWM = 1;	// Enable APWM mode

   #if (CPU_FRQ_100MHZ)
     ECap3Regs.CAP1 = 0x05F5E100;			// Set Period value
     ECap3Regs.CAP2 = 0x02FAF080;			// Set Compare value
   #endif

   #if (CPU_FRQ_60MHZ)
     ECap3Regs.CAP1 = 0x03938700;			// Set Period value
     ECap3Regs.CAP2 = 0x01C9C380;			// Set Compare value
   #endif

   ECap3Regs.ECCLR.all = 0x0FF;			// Clear pending interrupts
   ECap1Regs.ECEINT.bit.CTR_EQ_CMP = 1; // enable Compare Equal Int

   // Setup APWM mode on CAP4, set period and compare registers
   ECap4Regs.ECCTL2.bit.CAP_APWM = 1;	// Enable APWM mode
   ECap4Regs.CAP1 = 0x00001388;			// Set Period value
   ECap4Regs.CAP2 = 0x000009C4;			// Set Compare value
   ECap4Regs.ECCLR.all = 0x0FF;			// Clear pending interrupts
   ECap1Regs.ECEINT.bit.CTR_EQ_CMP = 1; // enable Compare Equal Int

   // Start counters
   ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;
   ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;
   ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;
   ECap4Regs.ECCTL2.bit.TSCTRSTOP = 1;

   for(;;)
   {
      // set next duty cycle to 50%
      ECap1Regs.CAP4 = ECap1Regs.CAP1 >> 1;

      // vary freq between 5 Hz and 10 Hz
      if(ECap1Regs.CAP1 >= 0x01312D00)
      {
         direction = 0;
      } else if (ECap1Regs.CAP1 <= 0x00989680)
      {
         direction = 1;
      }

      if(direction == 0)
      {
         ECap1Regs.CAP3 = ECap1Regs.CAP1 - 500000;
      } else
      {
         ECap1Regs.CAP3 = ECap1Regs.CAP1 + 500000;
      }
   }

}



//===========================================================================
// No more.
//===========================================================================

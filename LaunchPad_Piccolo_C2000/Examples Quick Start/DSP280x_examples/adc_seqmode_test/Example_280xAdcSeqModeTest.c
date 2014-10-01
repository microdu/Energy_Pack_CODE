// TI File $Revision: /main/7 $
// Checkin $Date: March 27, 2007   15:24:36 $
//###########################################################################
//
// FILE:   Example_280xAdcSeqModeTest.c
//
// TITLE:  DSP280x ADC Seq Mode Test.
//
// ASSUMPTIONS:
//
//    This program requires the DSP280x header files.
//
//    Make sure the CPU clock speed is properly defined in
//    DSP280x_Examples.h before compiling this example.
//
//    Connect the signal to be converted to channel A0.
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
//    Channel A0 is converted forever and logged in a buffer (SampleTable)
//
//    Open a memory window to SampleTable to observe the buffer
//    RUN for a while and stop and see the table contents.
//
//       Watch Variables:
//          SampleTable - Log of converted values.
//
//###########################################################################
//
// Original source by: S.S.
//
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "DSP280x_Examples.h"   // DSP280x Examples Include File

// ADC start parameters
#define ADC_MODCLK 0x4   // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 100/(2*4)             = 12.5MHz
                         //    for 60 MHz devices:    HSPCLK =  60/(2*4)             = 7.5 MHz
#define ADC_CKPS   0x1   // ADC module clock = HSPCLK/2*ADC_CKPS   = 12.5MHz/(1*2)   = 6.25MHz
                         //    for 60 MHz devices: ADC module clk = 7.5MHz/(1*2)     = 3.75MHz
#define ADC_SHCLK  0xf   // S/H width in ADC module periods                          = 16 ADC clocks
#define AVG        1000  // Average sample limit
#define ZOFFSET    0x00  // Average Zero offset
#define BUF_SIZE   2048  // Sample buffer size

// Global variable for this example
Uint16 SampleTable[BUF_SIZE];

main()
{
   Uint16 i;

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP280x_SysCtrl.c file.
   InitSysCtrl();

// Specific clock setting for this example:
   EALLOW;
   SysCtrlRegs.HISPCP.all = ADC_MODCLK;	// HSPCLK = SYSCLKOUT/ADC_MODCLK
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

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP280x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
   InitAdc();  // For this example, init the ADC

// Specific ADC setup for this example:
   AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;
   AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;
   AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;        // 1  Cascaded mode
   AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
   AdcRegs.ADCTRL1.bit.CONT_RUN = 1;       // Setup continuous run


// Step 5. User specific code, enable interrupts:


// Clear SampleTable
   for (i=0; i<BUF_SIZE; i++)
   {
     SampleTable[i] = 0;
   }

   // Start SEQ1
   AdcRegs.ADCTRL2.all = 0x2000;

   // Take ADC data and log the in SampleTable array
   for(;;)
   {
     for (i=0; i<AVG; i++)
     {
        while (AdcRegs.ADCST.bit.INT_SEQ1== 0) {} // Wait for interrupt
        AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
        SampleTable[i] =((AdcRegs.ADCRESULT0>>4) );
     }
   }
}

//===========================================================================
// No more.
//===========================================================================


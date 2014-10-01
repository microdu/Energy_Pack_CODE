// TI File $Revision: /main/1 $
// Checkin $Date: April 9, 2007   17:05:00 $
//###########################################################################
//
// FILE:    Example_280xIdleWake.c
//
// TITLE:   Device Idle Mode and Wakeup Program.
//
// ASSUMPTIONS:
//
//    This program requires the DSP280x header files.
//
//    GPIO0 is configured as an XINT1 pin to trigger a
//    XINT1 interrupt upon detection of a falling edge.
//    Initially, pull GPIO0 high externally. To wake device 
//    from idle mode by triggering an XINT1 interrupt, 
//    pull GPIO0 low (falling edge) 
//
//    To observe when device wakes from IDLE mode, monitor
//    GPIO1 with an oscilloscope (set to 1 in XINT1 ISR)
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
//    This example puts the device into IDLE mode.
//
//    The example then wakes up the device from IDLE using XINT1
//    which triggers on a falling edge from GPIO0.
//    This pin must be pulled from high to low by an external agent for
//    wakeup.
//
//    To observe the device wakeup from IDLE mode, monitor GPIO1 with
//    an oscilloscope, which goes high in the XINT_1_ISR.
//
//###########################################################################
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"
#include "DSP280x_Examples.h"

// Prototype statements for functions found within this file.
interrupt void XINT_1_ISR(void);  	// ISR 

void main()

{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP280x_SysCtrl.c file.
   InitSysCtrl();

// Step 2. Initalize GPIO:
// This example function is found in the DSP280x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();  // Skipped for this example
    
    EALLOW;
	GpioCtrlRegs.GPAPUD.all = 0;                    // Enable all Pull-ups
	GpioCtrlRegs.GPBPUD.all = 0;	
	GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 0;		// Choose GPIO0 as the XINT1 pin.
	GpioCtrlRegs.GPADIR.all = 0xFFFFFFFE;	        // All pins are outputs except 0
	GpioDataRegs.GPADAT.all = 0x00000000;	        // All I/O pins are driven low
    EDIS;

    XIntruptRegs.XINT1CR.bit.ENABLE = 1; 	        // Enable XINT1 pin
	XIntruptRegs.XINT1CR.bit.POLARITY = 0;	        // Interrupt triggers on falling edge

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
   EALLOW;  // This is needed to write to EALLOW protected registers   	
   PieVectTable.XINT1 = &XINT_1_ISR;    
   EDIS;

// Step 4. Initialize all the Device Peripherals:
// Not applicable for this example.

// Step 5. User specific code, enable interrupts:

// Enable CPU INT1 which is connected to WakeInt:
   IER |= M_INT1;

// Enable XINT1 in the PIE: Group 1 interrupt 4
   PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
   PieCtrlRegs.PIEACK.bit.ACK1 = 1;

// Enable global Interrupts:
   EINT;   // Enable Global interrupt INTM 

// Write the LPM code value	
  EALLOW;
  if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 1) // Only enter Idle mode when PLL is not in limp mode.
  {
     SysCtrlRegs.LPMCR0.bit.LPM = 0x0000;  // LPM mode = Idle     
  }   
  EDIS; 
  asm(" IDLE");                            // Device waits in IDLE until XINT1 interrupts  								
  while(1){}
}


interrupt void XINT_1_ISR(void)
{
   GpioDataRegs.GPASET.bit.GPIO1 = 1;	// GPIO1 is driven high upon exiting IDLE.          									
   PieCtrlRegs.PIEACK.bit.ACK1 = 1;
   EINT;
   return;
}

	


	
	
	

    





	 

	 

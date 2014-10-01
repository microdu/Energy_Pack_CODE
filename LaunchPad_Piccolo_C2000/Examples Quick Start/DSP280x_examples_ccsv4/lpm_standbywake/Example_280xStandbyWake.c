// TI File $Revision: /main/2 $
// Checkin $Date: July 31, 2009   14:26:50 $
//###########################################################################
//
// FILE:    Example_280xStandbyWake.c
//
// TITLE:   Device Standby Mode and Wakeup Program.
//
// ASSUMPTIONS:
//
//    This program requires the DSP280x header files.
//
//    GPIO0 is configured as the LPM wakeup pin to trigger a
//    WAKEINT interrupt upon detection of a low pulse.
//    Initially, pull GPIO0 high externally. To wake device 
//    from standby mode, pull GPIO0 low for at least (2+QUALSTDBY)
//    OSCLKS, then pull it high again.
//
//    To observe when device wakes from STANDBY mode, monitor
//    GPIO1 with an oscilloscope (set to 1 in WAKEINT ISR)
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
//    This example puts the device into STANDBY mode. If the lowest
//    possible current consumption in STANDBY mode is desired, the
//    JTAG connector must be removed from the device board while
//    the device is in STANDBY mode.
//
//    The example then wakes up the device from STANDBY using GPIO0.
//    GPIO0 wakes the device from STANDBY mode when a low pulse
//    (signal goes high->low->high)is detected on the pin.
//    This pin must be pulsed by an external agent for wakeup.
//
//    As soon as GPIO0 goes high again after the pulse, the device
//    should wake up, and GPIO1 can be observed to go high.
//
//
//###########################################################################
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"
#include "DSP280x_Examples.h"

// Prototype statements for functions found within this file.

interrupt void WAKE_ISR(void);  	// ISR for WAKEINT

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
	GpioCtrlRegs.GPAPUD.all = 0;            // Enable all Pull-ups
	GpioCtrlRegs.GPBPUD.all = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;	    // GPIO1 set in the ISR to indicate device woken up.
    GpioIntRegs.GPIOLPMSEL.bit.GPIO0 = 1;	// Choose GPIO0 pin for wakeup 
	EDIS;

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
   PieVectTable.WAKEINT = &WAKE_ISR;
   EDIS;

// Step 4. Initialize all the Device Peripherals:
// Not applicable for this example.

// Step 5. User specific code, enable interrupts:

// Enable CPU INT1 which is connected to WakeInt:
   IER |= M_INT1;

// Enable WAKEINT in the PIE: Group 1 interrupt 8
   PieCtrlRegs.PIEIER1.bit.INTx8 = 1;
   PieCtrlRegs.PIEACK.bit.ACK1 = 1;
// Enable global Interrupts:
   EINT;   // Enable Global interrupt INTM

// Choose qualification cycles in LPMCR0 register 
	SysCtrlRegs.LPMCR0.bit.QUALSTDBY = 0;	// The wakeup signal should be (2+QUALSTDBY) OSCCLKs wide.

// Write the LPM code value
    EALLOW;
	if (SysCtrlRegs.PLLSTS.bit.MCLKSTS != 1) // Only enter Standby mode when PLL is not in limp mode.
	{
      SysCtrlRegs.LPMCR0.bit.LPM = 0x0001;   // LPM mode = Standby 
    }
    EDIS;
// Force device into STANDBY

	asm(" IDLE");                           // Device waits in IDLE until falling edge on GPIO0/XNMI pin
	                                        // wakes device from Standby mode.
	while(1){}								// Loop here after wake-up.

}

/* ----------------------------------------------- */
/* ISR for WAKEINT - Will be executed when         */
/* low pulse triggered on GPIO0 pin                */
/* ------------------------------------------------*/
interrupt void WAKE_ISR(void)
{
   GpioDataRegs.GPASET.bit.GPIO1 = 1;	// Set GPIO1 in the ISR - monitored with oscilloscope
   PieCtrlRegs.PIEACK.bit.ACK1 = 1;
   return;
}

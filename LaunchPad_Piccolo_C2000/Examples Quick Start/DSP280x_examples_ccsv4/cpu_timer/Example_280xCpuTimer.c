// TI File $Revision: /main/3 $
// Checkin $Date: July 31, 2009   14:26:06 $
//###########################################################################
//
// FILE:    Example_280xCpuTimer.c
//
// TITLE:   DSP280x Device Getting Started Program.
//
// ASSUMPTIONS:
//
//    This program requires the DSP280x header files.  
//
//    Other then boot mode configuration, no other hardware configuration
//    is required.   
//
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
//    This example configures CPU Timer0 and increments
//    a counter each time the timer asserts an interrupt.
//      
//       Watch Variables:
//          CpuTimer0.InterruptCount
//
//###########################################################################
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################


#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "DSP280x_Examples.h"   // DSP280x Examples Include File

// Prototype statements for functions found within this file.
interrupt void cpu_timer0_isr(void);

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
   PieVectTable.TINT0 = &cpu_timer0_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP280x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
   InitCpuTimers();   // For this example, only initialize the Cpu Timers

// Configure CPU-Timer 0 to interrupt every second:
// 100MHz CPU Freq, 1 second Period (in uSeconds)
   ConfigCpuTimer(&CpuTimer0, 100, 1000000);
   StartCpuTimer0();

// Step 5. User specific code, enable interrupts:


// Enable CPU INT1 which is connected to CPU-Timer 0:
   IER |= M_INT1;

// Enable TINT0 in the PIE: Group 1 interrupt 7
   PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;);

} 


interrupt void cpu_timer0_isr(void)
{
   CpuTimer0.InterruptCount++;

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//===========================================================================
// No more.
//===========================================================================

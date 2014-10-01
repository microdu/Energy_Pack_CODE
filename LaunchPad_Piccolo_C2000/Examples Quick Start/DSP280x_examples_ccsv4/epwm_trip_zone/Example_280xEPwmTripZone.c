// TI File $Revision: /main/2 $
// Checkin $Date: July 31, 2009   14:26:20 $
//###########################################################################
//
// FILE:    Example_280xEpwmTripZone.c
//
// TITLE:   Check PWM Trip Zone Test
//
// ASSUMPTIONS:
//
//    This program requires the DSP280x header files.  
//
//    Initially tie TZ1 (GPIO12) and TZ2 (GPIO13) high. 
//
//    During the test, monitor ePWM1 or ePWM2 outputs 
//    on a scope Pull TZ1 or TZ2 low to see the effect. 
//
//       EPWM1A is on GPIO0
//       EPWM1B is on GPIO1
//       EPWM2A is on GPIO2
//       EPWM2B is on GPIO3
//
//    ePWM1 will react as a 1 shot trip
// 
//    ePWM2 will react as a cycle by cycle trip and will be
//    cleared if TZ1 and TZ2 are both pulled back high. 
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
//    This example configures ePWM1 and ePWM2 
// 
//    2 Examples are included:
//    * ePWM1 has TZ1 and TZ2 as one shot trip sources
//    * ePWM2 has TZ1 and TZ2 as cycle by cycle trip sources  
//
//    Each ePWM is configured to interrupt on the 3rd zero event
//    when this happens the deadband is modified such that
//    0 <= DB <= DB_MAX.  That is, the deadband will move up and
//    down between 0 and the maximum value. 
//
//
//    View the EPWM1A/B, EPWM2A/B waveforms 
//    via an oscilloscope to see the effect of TZ1 and TZ2
//
//
//###########################################################################
// $TI Release: DSP280x C/C++ Header Files V1.70 $
// $Release Date: July 27, 2009 $
//###########################################################################

#include "DSP280x_Device.h"     // DSP280x Headerfile Include File
#include "DSP280x_Examples.h"   // DSP280x Examples Include File

// Prototype statements for functions found within this file.
void InitEPwm1Example(void);
void InitEPwm2Example(void);
interrupt void epwm1_tzint_isr(void);
interrupt void epwm2_tzint_isr(void);


// Global variables used in this example
Uint32  EPwm1TZIntCount;
Uint32  EPwm2TZIntCount;

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

// For this case just init GPIO pins for ePWM1, ePWM2, and TZ pins
   InitEPwm1Gpio();
   InitEPwm2Gpio();
   InitTzGpio();
   
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
   PieVectTable.EPWM1_TZINT = &epwm1_tzint_isr;
   PieVectTable.EPWM2_TZINT = &epwm2_tzint_isr;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP280x_InitPeripherals.c
// InitPeripherals();  // Not required for this example


   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
   EDIS;

   InitEPwm1Example();    
   InitEPwm2Example();

   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;
   
// Step 5. User specific code, enable interrupts
// Initalize counters:   
   EPwm1TZIntCount = 0;
   EPwm2TZIntCount = 0;
   
// Enable CPU INT3 which is connected to EPWM1-3 INT:
   IER |= M_INT2;

// Enable EPWM INTn in the PIE: Group 2 interrupt 1-3
   PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
   PieCtrlRegs.PIEIER2.bit.INTx2 = 1;

// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM



// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;)
   {
       asm("          NOP");
   }

} 

interrupt void epwm1_tzint_isr(void)
{
   EPwm1TZIntCount++;

// Leave these flags set so we only take this
// interrupt once
//
// EALLOW;
// EPwm1Regs.TZCLR.bit.OST = 1;
// EPwm1Regs.TZCLR.bit.INT = 1;
// EDIS;

   // Acknowledge this interrupt to receive more interrupts from group 2
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

}

interrupt void epwm2_tzint_isr(void)
{

   EPwm2TZIntCount++;

// Clear the flags - we will continue to take
// this interrupt until the TZ pin goes high
//
   EALLOW;
   EPwm2Regs.TZCLR.bit.CBC = 1;   
   EPwm2Regs.TZCLR.bit.INT = 1;
   EDIS;

   // Acknowledge this interrupt to receive more interrupts from group 2
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;

}


void InitEPwm1Example()
{
   // Enable TZ1 and TZ2 as one shot trip sources
   EALLOW;
   EPwm1Regs.TZSEL.bit.OSHT1 = 1;
   EPwm1Regs.TZSEL.bit.OSHT2 = 1;
   
   // What do we want the TZ1 and TZ2 to do?
   EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_HI;
   EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
   
   // Enable TZ interrupt
   EPwm1Regs.TZEINT.bit.OST = 1;
   EDIS;
   
   EPwm1Regs.TBPRD = 6000;                         // Set timer period
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;            // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                       // Clear counter

   // Setup TBCLK
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV4;

   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;   

   // Setup compare 
   EPwm1Regs.CMPA.half.CMPA = 3000;

   // Set actions
   EPwm1Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM1A on Zero
   EPwm1Regs.AQCTLA.bit.CAD = AQ_CLEAR;
   

   EPwm1Regs.AQCTLB.bit.CAU = AQ_CLEAR;          // Set PWM1A on Zero
   EPwm1Regs.AQCTLB.bit.CAD = AQ_SET;

}


void InitEPwm2Example()
{

   // Enable TZ1 and TZ2 as one cycle-by-cycle trip sources
   EALLOW;
   EPwm2Regs.TZSEL.bit.CBC1 = 1;
   EPwm2Regs.TZSEL.bit.CBC2 = 1;
   
   // What do we want the TZ1 and TZ2 to do?
   EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_HI;
   EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO;
   
   // Enable TZ interrupt
   EPwm2Regs.TZEINT.bit.CBC = 1;
   EDIS;

   EPwm2Regs.TBPRD = 6000;                        // Set timer period
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                      // Clear counter
   
   // Setup TBCLK
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV4;       // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV4;          // Slow just to observe on the scope

   // Setup compare 
   EPwm2Regs.CMPA.half.CMPA = 3000;

   // Set actions
   EPwm2Regs.AQCTLA.bit.CAU = AQ_SET;             // Set PWM2A on Zero
   EPwm2Regs.AQCTLA.bit.CAD = AQ_CLEAR;
   

   EPwm2Regs.AQCTLB.bit.CAU = AQ_CLEAR;           // Set PWM2A on Zero
   EPwm2Regs.AQCTLB.bit.CAD = AQ_SET;
}




//===========================================================================
// No more.
//===========================================================================

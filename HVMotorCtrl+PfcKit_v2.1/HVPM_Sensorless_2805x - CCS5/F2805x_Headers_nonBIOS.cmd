/*
// TI File $Revision: /main/4 $
// Checkin $Date: August 31, 2011   17:51:12 $
//###########################################################################
//
// FILE:    F2805x_Headers_nonBIOS.cmd
//
// TITLE:   F2805x Peripheral registers linker command file
//
// DESCRIPTION:
//
//          This file is for use in Non-BIOS applications.
//
//          Linker command file to place the peripheral structures
//          used within the F2805x headerfiles into the correct memory
//          mapped locations.
//
//          This version of the file includes the PieVectorTable structure.
//          For BIOS applications, please use the F2805x_Headers_BIOS.cmd file
//          which does not include the PieVectorTable structure.
//
//###########################################################################
// $TI Release:  $
// $Release Date:  $
//###########################################################################
*/

MEMORY
{
 PAGE 0:    /* Program Memory */

 PAGE 1:    /* Data Memory */

   DEV_EMU      : origin = 0x000880, length = 0x000105     /* device emulation registers */
   SYS_PWR_CTL  : origin = 0x000985, length = 0x000003     /* System power control registers */
   FLASH_REGS   : origin = 0x000A80, length = 0x000060     /* FLASH registers */
                
   ADC_RESULT   : origin = 0x000B00, length = 0x000020     /* ADC Results register mirror */
                
   DCSM_Z1      : origin = 0x000B80, length = 0x000030     /* Zone 1 Dual code security module registers */
   DCSM_Z2      : origin = 0x000BC0, length = 0x000030     /* Zone 2 Dual code security module registers */
   DCSM_COMMON  : origin = 0x000BF0, length = 0x000010     /* Common Dual code security module registers */
                
   CPU_TIMER0   : origin = 0x000C00, length = 0x000008     /* CPU Timer0 registers */
   CPU_TIMER1   : origin = 0x000C08, length = 0x000008     /* CPU Timer0 registers (CPU Timer1 & Timer2 reserved TI use)*/
   CPU_TIMER2   : origin = 0x000C10, length = 0x000008     /* CPU Timer0 registers (CPU Timer1 & Timer2 reserved TI use)*/
                
   PIE_CTRL     : origin = 0x000CE0, length = 0x000020     /* PIE control registers */
   PIE_VECT     : origin = 0x000D00, length = 0x000100     /* PIE Vector Table */
                
   CLA1         : origin = 0x001400, length = 0x000080     /* CLA registers */
                
   ECANA        : origin = 0x006000, length = 0x000040     /* eCAN-A control and status registers */
   ECANA_LAM    : origin = 0x006040, length = 0x000040     /* eCAN-A local acceptance masks */
   ECANA_MOTS   : origin = 0x006080, length = 0x000040     /* eCAN-A message object time stamps */
   ECANA_MOTO   : origin = 0x0060C0, length = 0x000040     /* eCAN-A object time-out registers */
   ECANA_MBOX   : origin = 0x006100, length = 0x000100     /* eCAN-A mailboxes */
                
   ANASUBSYS    : origin = 0x006400, length = 0x000100     /* Analog Subsystem registers */
                
   EPWM1        : origin = 0x006800, length = 0x000040     /* Enhanced PWM 1 registers */
   EPWM2        : origin = 0x006840, length = 0x000040     /* Enhanced PWM 2 registers */
   EPWM3        : origin = 0x006880, length = 0x000040     /* Enhanced PWM 3 registers */
   EPWM4        : origin = 0x0068C0, length = 0x000040     /* Enhanced PWM 4 registers */
   EPWM5        : origin = 0x006900, length = 0x000040     /* Enhanced PWM 5 registers */
   EPWM6        : origin = 0x006940, length = 0x000040     /* Enhanced PWM 6 registers */
   EPWM7        : origin = 0x006980, length = 0x000040     /* Enhanced PWM 7 registers */
                
   ECAP1        : origin = 0x006A00, length = 0x000020     /* Enhanced Capture 1 registers */
                
   EQEP1        : origin = 0x006B00, length = 0x000040     /* Enhanced QEP 1 registers */
                
   GPIOCTRL     : origin = 0x006F80, length = 0x000040     /* GPIO control registers */
   GPIODAT      : origin = 0x006FC0, length = 0x000020     /* GPIO data registers */
   GPIOINT      : origin = 0x006FE0, length = 0x000020     /* GPIO interrupt/LPM registers */

   SYSTEM       : origin = 0x007010, length = 0x000020     /* System control registers */

   SPIA         : origin = 0x007040, length = 0x000010     /* SPI-A registers */
   SCIA         : origin = 0x007050, length = 0x000010     /* SCI-A registers */

   NMIINTRUPT  : origin = 0x007060, length = 0x000010     /* NMI Watchdog Interrupt Registers */
   XINTRUPT     : origin = 0x007070, length = 0x000010     /* external interrupt registers */

   ADC          : origin = 0x007100, length = 0x000080     /* ADC registers */
   
   SCIB         : origin = 0x007750, length = 0x000010     /* SCI-B registers */
   SCIC         : origin = 0x007770, length = 0x000010     /* SCI-A registers */

   I2CA         : origin = 0x007900, length = 0x000040     /* I2C-A registers */
  
   DCSM_OTP_Z2  : origin = 0x3D7800, length = 0x000006     /* Part of Z1 OTP.  LinkPointer/JTAG lock/ Boot Mode */
   DCSM_OTP_Z1  : origin = 0x3D7A00, length = 0x000006     /* Part of Z2 OTP.  LinkPointer/JTAG lock */
   
   /* The below address ORIGIN's are MOVABLE depending on placement of Zone Select Blocks in OTP */
   DCSM_ZSEL_Z1 : origin = 0x3D7A10, length = 0x000010     /* Part of Z1 OTP.  Z1 password locations / Flash and RAM partitioning */
   DCSM_ZSEL_Z2 : origin = 0x3D7810, length = 0x000010     /* Part of Z2 OTP.  Z2 password locations / Flash and RAM partitioning  */

}


SECTIONS
{
/*** PIE Vect Table and Boot ROM Variables Structures ***/
  UNION run = PIE_VECT, PAGE = 1
   {
      PieVectTableFile
      GROUP
      {
         EmuKeyVar
         EmuBModeVar
         FlashCallbackVar
         FlashScalingVar
      }
   }

/*** Peripheral Frame 0 Register Structures ***/
   DevEmuRegsFile       : > DEV_EMU,     PAGE = 1
   SysPwrCtrlRegsFile   : > SYS_PWR_CTL, PAGE = 1
   FlashRegsFile        : > FLASH_REGS,  PAGE = 1
   AdcResultFile        : > ADC_RESULT,  PAGE = 1
   DcsmRegsZ1File       : > DCSM_Z1,     PAGE = 1
   DcsmRegsZ2File       : > DCSM_Z2,     PAGE = 1
   DcsmRegsCommonFile   : > DCSM_COMMON, PAGE = 1
   CpuTimer0RegsFile    : > CPU_TIMER0,  PAGE = 1
   CpuTimer1RegsFile    : > CPU_TIMER1,  PAGE = 1
   CpuTimer2RegsFile    : > CPU_TIMER2,  PAGE = 1
   PieCtrlRegsFile      : > PIE_CTRL,    PAGE = 1
   Cla1RegsFile         : > CLA1,        PAGE = 1

/*** Peripheral Frame 1 Register Structures ***/
   ECanaRegsFile        : > ECANA,        PAGE = 1
   ECanaLAMRegsFile     : > ECANA_LAM,    PAGE = 1
   ECanaMboxesFile      : > ECANA_MBOX,   PAGE = 1
   ECanaMOTSRegsFile    : > ECANA_MOTS,   PAGE = 1
   ECanaMOTORegsFile    : > ECANA_MOTO,   PAGE = 1
   GpioCtrlRegsFile     : > GPIOCTRL,     PAGE = 1
   GpioDataRegsFile     : > GPIODAT,      PAGE = 1
   GpioIntRegsFile      : > GPIOINT,      PAGE = 1

/*** Peripheral Frame 2 Register Structures ***/
   SysCtrlRegsFile      : > SYSTEM,       PAGE = 1
   SpiaRegsFile         : > SPIA,         PAGE = 1
   SciaRegsFile         : > SCIA,         PAGE = 1
   ScibRegsFile         : > SCIB,         PAGE = 1
   ScicRegsFile         : > SCIC,         PAGE = 1
   NmiIntruptRegsFile   : > NMIINTRUPT,   PAGE = 1
   XIntruptRegsFile     : > XINTRUPT,     PAGE = 1
   AdcRegsFile          : > ADC,          PAGE = 1
   I2caRegsFile         : > I2CA,         PAGE = 1

/*** Peripheral Frame 3 Register Structures ***/
   AnalogSubsysRegsFile : > ANASUBSYS,    PAGE = 1
   EPwm1RegsFile        : > EPWM1,        PAGE = 1
   EPwm2RegsFile        : > EPWM2,        PAGE = 1
   EPwm3RegsFile        : > EPWM3,        PAGE = 1
   EPwm4RegsFile        : > EPWM4,        PAGE = 1
   EPwm5RegsFile        : > EPWM5,        PAGE = 1
   EPwm6RegsFile        : > EPWM6,        PAGE = 1
   EPwm7RegsFile        : > EPWM7,        PAGE = 1
   ECap1RegsFile        : > ECAP1,        PAGE = 1
   EQep1RegsFile        : > EQEP1,        PAGE = 1

/*** Dual Code Security Module Register Structures ***/
   DcsmOtpZ1File        : > DCSM_OTP_Z1,  PAGE = 1
   DcsmZoneselZ1File    : > DCSM_ZSEL_Z1, PAGE = 1
   DcsmOtpZ2File        : > DCSM_OTP_Z2,  PAGE = 1
   DcsmZoneselZ2File    : > DCSM_ZSEL_Z2, PAGE = 1

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
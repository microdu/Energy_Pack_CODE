// TI File $Revision: /main/2 $
// Checkin $Date: August 31, 2011   17:59:23 $
//###########################################################################
//
// FILE:    F2805x_GlobalVariableDefs.c
//
// TITLE:   F2805x Global Variables and Data Section Pragmas.
//
//###########################################################################
// $TI Release: 2805x Internal Release 2 $
// $Release Date: November 11, 2008 $
//###########################################################################

#include "F2805x_Device.h"     // F2805x Headerfile Include File

//---------------------------------------------------------------------------
// Define Global Peripheral Variables:
//
//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("AdcRegsFile")
#else
#pragma DATA_SECTION(AdcRegs,"AdcRegsFile");
#endif
volatile struct ADC_REGS AdcRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("AdcResultFile")
#else
#pragma DATA_SECTION(AdcResult,"AdcResultFile");
#endif
volatile struct ADC_RESULT_REGS AdcResult;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("AnalogSubsysRegsFile")
#else
#pragma DATA_SECTION(AnalogSubsysRegs,"AnalogSubsysRegsFile");
#endif
volatile struct ANALOG_SUBSYS_REGS AnalogSubsysRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("CpuTimer0RegsFile")
#else
#pragma DATA_SECTION(CpuTimer0Regs,"CpuTimer0RegsFile");
#endif
volatile struct CPUTIMER_REGS CpuTimer0Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("CpuTimer1RegsFile")
#else
#pragma DATA_SECTION(CpuTimer1Regs,"CpuTimer1RegsFile");
#endif
volatile struct CPUTIMER_REGS CpuTimer1Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("CpuTimer2RegsFile")
#else
#pragma DATA_SECTION(CpuTimer2Regs,"CpuTimer2RegsFile");
#endif
volatile struct CPUTIMER_REGS CpuTimer2Regs;

//----------------------------------------
//#ifdef __cplusplus
//#pragma DATA_SECTION("DcsmZoneselZ1File")
//#else
//#pragma DATA_SECTION(DcsmZoneselZ1,"DcsmZoneselZ1File");
//#endif
//volatile struct DCSM_ZONESEL_Z1 DcsmZoneselZ1;

//----------------------------------------
//#ifdef __cplusplus
//#pragma DATA_SECTION("DcsmZoneselZ2File")
//#else
//#pragma DATA_SECTION(DcsmZoneselZ2,"DcsmZoneselZ2File");
//#endif
//volatile struct DCSM_ZONESEL_Z2 DcsmZoneselZ2;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("DcsmRegsZ1File")
#else
#pragma DATA_SECTION(DcsmRegsZ1,"DcsmRegsZ1File");
#endif
volatile struct DCSM_REGS_Z1 DcsmRegsZ1;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("DcsmRegsZ2File")
#else
#pragma DATA_SECTION(DcsmRegsZ2,"DcsmRegsZ2File");
#endif
volatile struct DCSM_REGS_Z2 DcsmRegsZ2;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("DcsmOtpZ1File")
#else
#pragma DATA_SECTION(DcsmOtpZ1,"DcsmOtpZ1File");
#endif
volatile struct DCSM_OTP_Z1 DcsmOtpZ1;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("DcsmOtpZ2File")
#else
#pragma DATA_SECTION(DcsmOtpZ2,"DcsmOtpZ2File");
#endif
volatile struct DCSM_OTP_Z2 DcsmOtpZ2;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("DcsmRegsCommonFile")
#else
#pragma DATA_SECTION(DcsmRegsCommon,"DcsmRegsCommonFile");
#endif
volatile struct DCSM_REGS_COMMON DcsmRegsCommon;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("DevEmuRegsFile")
#else
#pragma DATA_SECTION(DevEmuRegs,"DevEmuRegsFile");
#endif
volatile struct DEV_EMU_REGS DevEmuRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ECanaRegsFile")
#else
#pragma DATA_SECTION(ECanaRegs,"ECanaRegsFile");
#endif
volatile struct ECAN_REGS ECanaRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ECanaMboxesFile")
#else
#pragma DATA_SECTION(ECanaMboxes,"ECanaMboxesFile");
#endif
volatile struct ECAN_MBOXES ECanaMboxes;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ECanaLAMRegsFile")
#else
#pragma DATA_SECTION(ECanaLAMRegs,"ECanaLAMRegsFile");
#endif
volatile struct LAM_REGS ECanaLAMRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ECanaMOTSRegsFile")
#else
#pragma DATA_SECTION(ECanaMOTSRegs,"ECanaMOTSRegsFile");
#endif
volatile struct MOTS_REGS ECanaMOTSRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ECanaMOTORegsFile")
#else
#pragma DATA_SECTION(ECanaMOTORegs,"ECanaMOTORegsFile");
#endif
volatile struct MOTO_REGS ECanaMOTORegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("Cla1RegsFile")
#else
#pragma DATA_SECTION(Cla1Regs,"Cla1RegsFile");
#endif
volatile struct CLA_REGS Cla1Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm1RegsFile")
#else
#pragma DATA_SECTION(EPwm1Regs,"EPwm1RegsFile");
#endif
volatile struct EPWM_REGS EPwm1Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm2RegsFile")
#else
#pragma DATA_SECTION(EPwm2Regs,"EPwm2RegsFile");
#endif
volatile struct EPWM_REGS EPwm2Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm3RegsFile")
#else
#pragma DATA_SECTION(EPwm3Regs,"EPwm3RegsFile");
#endif
volatile struct EPWM_REGS EPwm3Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm4RegsFile")
#else
#pragma DATA_SECTION(EPwm4Regs,"EPwm4RegsFile");
#endif
volatile struct EPWM_REGS EPwm4Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm5RegsFile")
#else
#pragma DATA_SECTION(EPwm5Regs,"EPwm5RegsFile");
#endif
volatile struct EPWM_REGS EPwm5Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm6RegsFile")
#else
#pragma DATA_SECTION(EPwm6Regs,"EPwm6RegsFile");
#endif
volatile struct EPWM_REGS EPwm6Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EPwm7RegsFile")
#else
#pragma DATA_SECTION(EPwm7Regs,"EPwm7RegsFile");
#endif
volatile struct EPWM_REGS EPwm7Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ECap1RegsFile")
#else
#pragma DATA_SECTION(ECap1Regs,"ECap1RegsFile");
#endif
volatile struct ECAP_REGS ECap1Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EQep1RegsFile")
#else
#pragma DATA_SECTION(EQep1Regs,"EQep1RegsFile");
#endif
volatile struct EQEP_REGS EQep1Regs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("GpioCtrlRegsFile")
#else
#pragma DATA_SECTION(GpioCtrlRegs,"GpioCtrlRegsFile");
#endif
volatile struct GPIO_CTRL_REGS GpioCtrlRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("GpioDataRegsFile")
#else
#pragma DATA_SECTION(GpioDataRegs,"GpioDataRegsFile");
#endif
volatile struct GPIO_DATA_REGS GpioDataRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("GpioIntRegsFile")
#else
#pragma DATA_SECTION(GpioIntRegs,"GpioIntRegsFile");
#endif
volatile struct GPIO_INT_REGS GpioIntRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("I2caRegsFile")
#else
#pragma DATA_SECTION(I2caRegs,"I2caRegsFile");
#endif
volatile struct I2C_REGS I2caRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("NmiIntruptRegsFile")
#else
#pragma DATA_SECTION(NmiIntruptRegs,"NmiIntruptRegsFile");
#endif
volatile struct NMI_INTRUPT_REGS NmiIntruptRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("PieCtrlRegsFile")
#else
#pragma DATA_SECTION(PieCtrlRegs,"PieCtrlRegsFile");
#endif
volatile struct PIE_CTRL_REGS PieCtrlRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("PieVectTableFile")
#else
#pragma DATA_SECTION(PieVectTable,"PieVectTableFile");
#endif
struct PIE_VECT_TABLE PieVectTable;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("SciaRegsFile")
#else
#pragma DATA_SECTION(SciaRegs,"SciaRegsFile");
#endif
volatile struct SCI_REGS SciaRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ScibRegsFile")
#else
#pragma DATA_SECTION(ScibRegs,"ScibRegsFile");
#endif
volatile struct SCI_REGS ScibRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("ScicRegsFile")
#else
#pragma DATA_SECTION(ScicRegs,"ScicRegsFile");
#endif
volatile struct SCI_REGS ScicRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("SpiaRegsFile")
#else
#pragma DATA_SECTION(SpiaRegs,"SpiaRegsFile");
#endif
volatile struct SPI_REGS SpiaRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("SysCtrlRegsFile")
#else
#pragma DATA_SECTION(SysCtrlRegs,"SysCtrlRegsFile");
#endif
volatile struct SYS_CTRL_REGS SysCtrlRegs;

//----------------------------------------

 #ifdef __cplusplus
#pragma DATA_SECTION("SysPwrCtrlRegsFile")
#else
#pragma DATA_SECTION(SysPwrCtrlRegs,"SysPwrCtrlRegsFile");
#endif
volatile struct SYS_PWR_CTRL_REGS SysPwrCtrlRegs; 

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("FlashRegsFile")
#else
#pragma DATA_SECTION(FlashRegs,"FlashRegsFile");
#endif
volatile struct FLASH_REGS FlashRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("XIntruptRegsFile")
#else
#pragma DATA_SECTION(XIntruptRegs,"XIntruptRegsFile");
#endif
volatile struct XINTRUPT_REGS XIntruptRegs;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EmuKeyVar");
#else
#pragma DATA_SECTION(EmuKey,"EmuKeyVar");
#endif
Uint16 EmuKey;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("EmuBModeVar");
#else
#pragma DATA_SECTION(EmuBMode,"EmuBModeVar");
#endif
Uint16 EmuBMode;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("FlashScalingVar");
#else
#pragma DATA_SECTION(Flash_CPUScaleFactor, "FlashScalingVar");
#endif
Uint32 Flash_CPUScaleFactor;

//----------------------------------------
#ifdef __cplusplus
#pragma DATA_SECTION("FlashCallbackVar");
#else
#pragma DATA_SECTION(Flash_CallbackPtr, "FlashCallbackVar");
#endif
void (*Flash_CallbackPtr) (void);


//===========================================================================
// End of file.
//===========================================================================


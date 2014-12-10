################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_UPPER_SRCS += \
../F28035_FLASH_BLDC_Int_GUI_DRV83xx.CMD 

LIB_SRCS += \
C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/lib/C28x_InstaSPIN_BLDC_Lib.lib \
C:/ti/controlSUITE_working/libs/math/IQmath/v15c/lib/IQmath.lib \
../commros_28xx_c_full.lib 

C_SRCS += \
../BLDC_Int_GUI_DRV83xx-DevInit_F2803x.c \
../BLDC_Int_GUI_DRV83xx.c \
../Commros_user.c \
../DRV8301_SPI.c \
C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CpuTimers.c \
C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/source/DSP2803x_GlobalVariableDefs.c \
../Graph_Data.c 

ASM_SRCS += \
C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm \
../DSP2803x_usDelay.asm 

CMD_SRCS += \
C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/cmd/DSP2803x_Headers_nonBIOS.cmd 

ASM_DEPS += \
./DSP2803x_CodeStartBranch.pp \
./DSP2803x_usDelay.pp 

OBJS += \
./BLDC_Int_GUI_DRV83xx-DevInit_F2803x.obj \
./BLDC_Int_GUI_DRV83xx.obj \
./Commros_user.obj \
./DRV8301_SPI.obj \
./DSP2803x_CodeStartBranch.obj \
./DSP2803x_CpuTimers.obj \
./DSP2803x_GlobalVariableDefs.obj \
./DSP2803x_usDelay.obj \
./Graph_Data.obj 

C_DEPS += \
./BLDC_Int_GUI_DRV83xx-DevInit_F2803x.pp \
./BLDC_Int_GUI_DRV83xx.pp \
./Commros_user.pp \
./DRV8301_SPI.pp \
./DSP2803x_CpuTimers.pp \
./DSP2803x_GlobalVariableDefs.pp \
./Graph_Data.pp 

OBJS__QTD += \
".\BLDC_Int_GUI_DRV83xx-DevInit_F2803x.obj" \
".\BLDC_Int_GUI_DRV83xx.obj" \
".\Commros_user.obj" \
".\DRV8301_SPI.obj" \
".\DSP2803x_CodeStartBranch.obj" \
".\DSP2803x_CpuTimers.obj" \
".\DSP2803x_GlobalVariableDefs.obj" \
".\DSP2803x_usDelay.obj" \
".\Graph_Data.obj" 

ASM_DEPS__QTD += \
".\DSP2803x_CodeStartBranch.pp" \
".\DSP2803x_usDelay.pp" 

C_DEPS__QTD += \
".\BLDC_Int_GUI_DRV83xx-DevInit_F2803x.pp" \
".\BLDC_Int_GUI_DRV83xx.pp" \
".\Commros_user.pp" \
".\DRV8301_SPI.pp" \
".\DSP2803x_CpuTimers.pp" \
".\DSP2803x_GlobalVariableDefs.pp" \
".\Graph_Data.pp" 

C_SRCS_QUOTED += \
"../BLDC_Int_GUI_DRV83xx-DevInit_F2803x.c" \
"../BLDC_Int_GUI_DRV83xx.c" \
"../Commros_user.c" \
"../DRV8301_SPI.c" \
"C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CpuTimers.c" \
"C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/source/DSP2803x_GlobalVariableDefs.c" \
"../Graph_Data.c" 

ASM_SRCS_QUOTED += \
"C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm" \
"../DSP2803x_usDelay.asm" 



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_UPPER_SRCS += \
../F28035_FLASH_BridgelessPFC.CMD 

LIB_SRCS += \
C:/TI/controlSUITE/libs/math/IQmath/v15b/lib/IQmath.lib 

C_SRCS += \
C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/C/ADC_SOC_Cnf.c \
../BridgelessPFC-DevInit_F2803x.c \
../BridgelessPFC-Main.c \
C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/source/DSP2803x_GlobalVariableDefs.c \
C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/C/PWM_1ch_UpDwnCnt_Cnf.c \
C:/TI/controlSUITE/development_kits/~SupportFiles/source/SciCommsGui.c 

ASM_SRCS += \
../BridgelessPFC-DPL-ISR.asm \
C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm \
C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_usDelay.asm 

CMD_SRCS += \
C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/cmd/DSP2803x_Headers_nonBIOS.cmd 

ASM_DEPS += \
./BridgelessPFC-DPL-ISR.pp \
./DSP2803x_CodeStartBranch.pp \
./DSP2803x_usDelay.pp 

OBJS += \
./ADC_SOC_Cnf.obj \
./BridgelessPFC-DPL-ISR.obj \
./BridgelessPFC-DevInit_F2803x.obj \
./BridgelessPFC-Main.obj \
./DSP2803x_CodeStartBranch.obj \
./DSP2803x_GlobalVariableDefs.obj \
./DSP2803x_usDelay.obj \
./PWM_1ch_UpDwnCnt_Cnf.obj \
./SciCommsGui.obj 

C_DEPS += \
./ADC_SOC_Cnf.pp \
./BridgelessPFC-DevInit_F2803x.pp \
./BridgelessPFC-Main.pp \
./DSP2803x_GlobalVariableDefs.pp \
./PWM_1ch_UpDwnCnt_Cnf.pp \
./SciCommsGui.pp 

OBJS__QTD += \
".\ADC_SOC_Cnf.obj" \
".\BridgelessPFC-DPL-ISR.obj" \
".\BridgelessPFC-DevInit_F2803x.obj" \
".\BridgelessPFC-Main.obj" \
".\DSP2803x_CodeStartBranch.obj" \
".\DSP2803x_GlobalVariableDefs.obj" \
".\DSP2803x_usDelay.obj" \
".\PWM_1ch_UpDwnCnt_Cnf.obj" \
".\SciCommsGui.obj" 

ASM_DEPS__QTD += \
".\BridgelessPFC-DPL-ISR.pp" \
".\DSP2803x_CodeStartBranch.pp" \
".\DSP2803x_usDelay.pp" 

C_DEPS__QTD += \
".\ADC_SOC_Cnf.pp" \
".\BridgelessPFC-DevInit_F2803x.pp" \
".\BridgelessPFC-Main.pp" \
".\DSP2803x_GlobalVariableDefs.pp" \
".\PWM_1ch_UpDwnCnt_Cnf.pp" \
".\SciCommsGui.pp" 

C_SRCS_QUOTED += \
"C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/C/ADC_SOC_Cnf.c" \
"../BridgelessPFC-DevInit_F2803x.c" \
"../BridgelessPFC-Main.c" \
"C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/source/DSP2803x_GlobalVariableDefs.c" \
"C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/C/PWM_1ch_UpDwnCnt_Cnf.c" \
"C:/TI/controlSUITE/development_kits/~SupportFiles/source/SciCommsGui.c" 

ASM_SRCS_QUOTED += \
"../BridgelessPFC-DPL-ISR.asm" \
"C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm" \
"C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_usDelay.asm" 


# Each subdirectory must supply rules for building sources it contributes
ADC_SOC_Cnf.obj: C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/C/ADC_SOC_Cnf.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="ADC_SOC_Cnf.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BridgelessPFC-DPL-ISR.obj: ../BridgelessPFC-DPL-ISR.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="BridgelessPFC-DPL-ISR.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BridgelessPFC-DevInit_F2803x.obj: ../BridgelessPFC-DevInit_F2803x.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="BridgelessPFC-DevInit_F2803x.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BridgelessPFC-Main.obj: ../BridgelessPFC-Main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="BridgelessPFC-Main.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_CodeStartBranch.obj: C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="DSP2803x_CodeStartBranch.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_GlobalVariableDefs.obj: C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/source/DSP2803x_GlobalVariableDefs.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="DSP2803x_GlobalVariableDefs.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_usDelay.obj: C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_usDelay.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="DSP2803x_usDelay.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

PWM_1ch_UpDwnCnt_Cnf.obj: C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/C/PWM_1ch_UpDwnCnt_Cnf.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="PWM_1ch_UpDwnCnt_Cnf.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

SciCommsGui.obj: C:/TI/controlSUITE/development_kits/~SupportFiles/source/SciCommsGui.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define="_DEBUG" --define="FLASH" --define="LARGE_MODEL" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/TI/controlSUITE/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/TI/controlSUITE/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/include" --include_path="C:/TI/controlSUITE/libs/app_libs/digital_power/f2803x_v3.2/asm" --include_path="C:/TI/controlSUITE/libs/math/IQmath/v15c/include" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --diag_warning=225 --large_memory_model --cla_support=cla0 --preproc_with_compile --preproc_dependency="SciCommsGui.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_UPPER_SRCS += \
../F28027_RAM_PFC2PHIL.CMD 

LIB_SRCS += \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/lib/IQmath.lib 

C_SRCS += \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/C/ADC_SOC_Cnf.c \
../Comp_DrvGlobalVars.c \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/source/DSP2802x_GlobalVariableDefs.c \
../DacDrvCnf.c \
../PFC2PHIL-DevInit_F2802x.c \
../PFC2PhiL-Main.c \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/C/PWM_PFC2PhiL_Cnf.c 

ASM_SRCS += \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_CodeStartBranch.asm \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_usDelay.asm \
../PFC2PhiL-DPL-ISR.asm 

CMD_SRCS += \
C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/cmd/DSP2802x_Headers_nonBIOS.cmd 

ASM_DEPS += \
./DSP2802x_CodeStartBranch.pp \
./DSP2802x_usDelay.pp \
./PFC2PhiL-DPL-ISR.pp 

OBJS += \
./ADC_SOC_Cnf.obj \
./Comp_DrvGlobalVars.obj \
./DSP2802x_CodeStartBranch.obj \
./DSP2802x_GlobalVariableDefs.obj \
./DSP2802x_usDelay.obj \
./DacDrvCnf.obj \
./PFC2PHIL-DevInit_F2802x.obj \
./PFC2PhiL-DPL-ISR.obj \
./PFC2PhiL-Main.obj \
./PWM_PFC2PhiL_Cnf.obj 

C_DEPS += \
./ADC_SOC_Cnf.pp \
./Comp_DrvGlobalVars.pp \
./DSP2802x_GlobalVariableDefs.pp \
./DacDrvCnf.pp \
./PFC2PHIL-DevInit_F2802x.pp \
./PFC2PhiL-Main.pp \
./PWM_PFC2PhiL_Cnf.pp 

OBJS__QTD += \
".\ADC_SOC_Cnf.obj" \
".\Comp_DrvGlobalVars.obj" \
".\DSP2802x_CodeStartBranch.obj" \
".\DSP2802x_GlobalVariableDefs.obj" \
".\DSP2802x_usDelay.obj" \
".\DacDrvCnf.obj" \
".\PFC2PHIL-DevInit_F2802x.obj" \
".\PFC2PhiL-DPL-ISR.obj" \
".\PFC2PhiL-Main.obj" \
".\PWM_PFC2PhiL_Cnf.obj" 

ASM_DEPS__QTD += \
".\DSP2802x_CodeStartBranch.pp" \
".\DSP2802x_usDelay.pp" \
".\PFC2PhiL-DPL-ISR.pp" 

C_DEPS__QTD += \
".\ADC_SOC_Cnf.pp" \
".\Comp_DrvGlobalVars.pp" \
".\DSP2802x_GlobalVariableDefs.pp" \
".\DacDrvCnf.pp" \
".\PFC2PHIL-DevInit_F2802x.pp" \
".\PFC2PhiL-Main.pp" \
".\PWM_PFC2PhiL_Cnf.pp" 

C_SRCS_QUOTED += \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/C/ADC_SOC_Cnf.c" \
"../Comp_DrvGlobalVars.c" \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/source/DSP2802x_GlobalVariableDefs.c" \
"../DacDrvCnf.c" \
"../PFC2PHIL-DevInit_F2802x.c" \
"../PFC2PhiL-Main.c" \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/C/PWM_PFC2PhiL_Cnf.c" 

ASM_SRCS_QUOTED += \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_CodeStartBranch.asm" \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_usDelay.asm" \
"../PFC2PhiL-DPL-ISR.asm" 


# Each subdirectory must supply rules for building sources it contributes
ADC_SOC_Cnf.obj: C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/C/ADC_SOC_Cnf.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="ADC_SOC_Cnf.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

Comp_DrvGlobalVars.obj: ../Comp_DrvGlobalVars.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="Comp_DrvGlobalVars.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

DSP2802x_CodeStartBranch.obj: C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_CodeStartBranch.asm $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="DSP2802x_CodeStartBranch.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

DSP2802x_GlobalVariableDefs.obj: C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/source/DSP2802x_GlobalVariableDefs.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="DSP2802x_GlobalVariableDefs.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

DSP2802x_usDelay.obj: C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/source/DSP2802x_usDelay.asm $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="DSP2802x_usDelay.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

DacDrvCnf.obj: ../DacDrvCnf.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="DacDrvCnf.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

PFC2PHIL-DevInit_F2802x.obj: ../PFC2PHIL-DevInit_F2802x.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="PFC2PHIL-DevInit_F2802x.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

PFC2PhiL-DPL-ISR.obj: ../PFC2PhiL-DPL-ISR.asm $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="PFC2PhiL-DPL-ISR.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

PFC2PhiL-Main.obj: ../PFC2PhiL-Main.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="PFC2PhiL-Main.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '

PWM_PFC2PhiL_Cnf.obj: C:/New_Projects_070108/PFC\ Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/C/PWM_PFC2PhiL_Cnf.c $(GEN_OPTS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	$(shell echo --silicon_version=28 > ccsCompiler.opt)
	$(shell echo -g >> ccsCompiler.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsCompiler.opt)
	$(shell echo --include_path="C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_common/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/development_kits/~SupportFiles/F2802x_headers" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/include" --include_path="C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/app_libs/digital_power/f2802x_v2.0/asm" >> ccsCompiler.opt)
	$(shell echo --diag_warning=225 >> ccsCompiler.opt)
	$(shell echo --large_memory_model >> ccsCompiler.opt)
	$(shell echo --cla_support=cla0 >> ccsCompiler.opt)
	$(shell echo --preproc_with_compile >> ccsCompiler.opt)
	$(shell echo --preproc_dependency="PWM_PFC2PhiL_Cnf.pp" >> ccsCompiler.opt)
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> ccsCompiler.opt))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> ccsCompiler.opt))
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsCompiler.opt
	@echo 'Finished building: $<'
	@echo ' '



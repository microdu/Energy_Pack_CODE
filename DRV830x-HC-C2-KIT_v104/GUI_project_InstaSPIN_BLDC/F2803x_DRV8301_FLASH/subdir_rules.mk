################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
BLDC_Int_GUI_DRV83xx-DevInit_F2803x.obj: ../BLDC_Int_GUI_DRV83xx-DevInit_F2803x.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="BLDC_Int_GUI_DRV83xx-DevInit_F2803x.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

BLDC_Int_GUI_DRV83xx.obj: ../BLDC_Int_GUI_DRV83xx.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="BLDC_Int_GUI_DRV83xx.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

Commros_user.obj: ../Commros_user.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="Commros_user.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DRV8301_SPI.obj: ../DRV8301_SPI.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="DRV8301_SPI.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_CodeStartBranch.obj: C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CodeStartBranch.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="DSP2803x_CodeStartBranch.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_CpuTimers.obj: C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/source/DSP2803x_CpuTimers.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="DSP2803x_CpuTimers.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_GlobalVariableDefs.obj: C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/source/DSP2803x_GlobalVariableDefs.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="DSP2803x_GlobalVariableDefs.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

DSP2803x_usDelay.obj: ../DSP2803x_usDelay.asm $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="DSP2803x_usDelay.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

Graph_Data.obj: ../Graph_Data.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8301" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --include_path="C:/ti/controlSUITE_working/libs/math/IQmath/v15c/include" --include_path="C:/ti/controlSUITE_working/development_kits/~SupportFiles/F2803x_headers" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/include" --include_path="C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_common/include" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/math_blocks/v3.1" --include_path="C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/include" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data --preproc_with_compile --preproc_dependency="Graph_Data.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '



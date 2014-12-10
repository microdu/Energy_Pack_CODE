################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

EMPTY := 
SPACE := $(EMPTY) $(EMPTY)
CG_TOOL_ROOT := C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000

ORDERED_OBJS += \
$(GEN_CMDS_QUOTED) \
"./Graph_Data.obj" \
"./DSP2803x_usDelay.obj" \
"./DSP2803x_GlobalVariableDefs.obj" \
"./DSP2803x_CpuTimers.obj" \
"./DSP2803x_CodeStartBranch.obj" \
"./DRV8301_SPI.obj" \
"./Commros_user.obj" \
"./BLDC_Int_GUI_DRV83xx.obj" \
"./BLDC_Int_GUI_DRV83xx-DevInit_F2803x.obj" \
-l"libc.a" \
"C:/ti/controlSUITE_working/libs/app_libs/motor_control/libs/InstaSPIN-BLDC/v100/lib/C28x_InstaSPIN_BLDC_Lib.lib" \
"C:/ti/controlSUITE_working/device_support/f2803x/v124/DSP2803x_headers/cmd/DSP2803x_Headers_nonBIOS.cmd" \
"../F28035_FLASH_BLDC_Int_GUI_DRV83xx.CMD" \
"C:/ti/controlSUITE_working/libs/math/IQmath/v15c/lib/IQmath.lib" \
"../commros_28xx_c_full.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: InstaSPIN_BLDC_GUI_F2803x_DRV8302.out

# Tool invocations
InstaSPIN_BLDC_GUI_F2803x_DRV8302.out: $(OBJS) $(CMD_SRCS) $(CMD_UPPER_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: Linker'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/bin/cl2000" --silicon_version=28 -g --define=FLASH --define="DRV8302" --diag_warning=225 --large_memory_model --unified_memory --cla_support=cla0 --cdebug_asm_data -z -m"BLDC_Int_GUI_F2803x_DRV8302.map" --stack_size=0x380 --warn_sections -i"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/lib" -i"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/c2000/include" --reread_libs --rom_model -o "InstaSPIN_BLDC_GUI_F2803x_DRV8302.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C2000_EXECUTABLE_OUTPUTS__QTD) "InstaSPIN_BLDC_GUI_F2803x_DRV8302.out"
	-$(RM) ".\BLDC_Int_GUI_DRV83xx-DevInit_F2803x.obj" ".\BLDC_Int_GUI_DRV83xx.obj" ".\Commros_user.obj" ".\DRV8301_SPI.obj" ".\DSP2803x_CodeStartBranch.obj" ".\DSP2803x_CpuTimers.obj" ".\DSP2803x_GlobalVariableDefs.obj" ".\DSP2803x_usDelay.obj" ".\Graph_Data.obj" 
	-$(RM) ".\DSP2803x_CodeStartBranch.pp" ".\DSP2803x_usDelay.pp" 
	-$(RM) ".\BLDC_Int_GUI_DRV83xx-DevInit_F2803x.pp" ".\BLDC_Int_GUI_DRV83xx.pp" ".\Commros_user.pp" ".\DRV8301_SPI.pp" ".\DSP2803x_CpuTimers.pp" ".\DSP2803x_GlobalVariableDefs.pp" ".\Graph_Data.pp" 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

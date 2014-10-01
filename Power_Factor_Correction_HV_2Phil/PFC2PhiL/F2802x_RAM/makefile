################################################################################
# Automatically-generated file. Do not edit!
################################################################################

EMPTY := 
SPACE := $(EMPTY) $(EMPTY)

ORDERED_OBJS += \
$(GEN_CMDS_QUOTED) \
"./PWM_PFC2PhiL_Cnf.obj" \
"./PFC2PhiL-Main.obj" \
"./PFC2PhiL-DPL-ISR.obj" \
"./PFC2PHIL-DevInit_F2802x.obj" \
"./DacDrvCnf.obj" \
"./DSP2802x_usDelay.obj" \
"./DSP2802x_GlobalVariableDefs.obj" \
"./DSP2802x_CodeStartBranch.obj" \
"./Comp_DrvGlobalVars.obj" \
"./ADC_SOC_Cnf.obj" \
-l"libc.a" \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/device_support/f2802x/v125/DSP2802x_headers/cmd/DSP2802x_Headers_nonBIOS.cmd" \
"../F28027_RAM_PFC2PHIL.CMD" \
"C:/New_Projects_070108/PFC Controller/ControlSuite_061510/PowerLibInControlSuite/controlSUITE/libs/math/IQmath/v15b/lib/IQmath.lib" \

-include ../makefile.init

RM := DEL /F

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
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
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
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
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: PFC2PhiL.out

# Tool invocations
PFC2PhiL.out: $(GEN_CMDS) $(CMD_SRCS) $(CMD_UPPER_SRCS) $(LIB_SRCS) $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Linker'
	$(shell echo --silicon_version=28 > ccsLinker.opt)
	$(shell echo -g >> ccsLinker.opt)
	$(shell echo --define="_DEBUG" --define="LARGE_MODEL" >> ccsLinker.opt)
	$(shell echo --diag_warning=225 >> ccsLinker.opt)
	$(shell echo --large_memory_model >> ccsLinker.opt)
	$(shell echo --cla_support=cla0 >> ccsLinker.opt)
	$(shell echo -z >> ccsLinker.opt)
	$(shell echo -m"PFC2PhiL.map" >> ccsLinker.opt)
	$(shell echo --stack_size=0x380 >> ccsLinker.opt)
	$(shell echo --warn_sections >> ccsLinker.opt)
	$(shell echo -i"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/lib" -i"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/include" >> ccsLinker.opt)
	$(shell echo --reread_libs >> ccsLinker.opt)
	$(shell echo --rom_model >> ccsLinker.opt)
	$(shell type ccsObjs.opt >> ccsLinker.opt)
	"C:/Program Files/Texas Instruments/CCSV413/ccsv4/tools/compiler/c2000/bin/cl2000" -@ccsLinker.opt -o "PFC2PhiL.out"
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C2000_EXECUTABLE_OUTPUTS__QTD) "PFC2PhiL.out"
	-$(RM) ".\ADC_SOC_Cnf.obj" ".\Comp_DrvGlobalVars.obj" ".\DSP2802x_CodeStartBranch.obj" ".\DSP2802x_GlobalVariableDefs.obj" ".\DSP2802x_usDelay.obj" ".\DacDrvCnf.obj" ".\PFC2PHIL-DevInit_F2802x.obj" ".\PFC2PhiL-DPL-ISR.obj" ".\PFC2PhiL-Main.obj" ".\PWM_PFC2PhiL_Cnf.obj" 
	-$(RM) ".\DSP2802x_CodeStartBranch.pp" ".\DSP2802x_usDelay.pp" ".\PFC2PhiL-DPL-ISR.pp" 
	-$(RM) ".\ADC_SOC_Cnf.pp" ".\Comp_DrvGlobalVars.pp" ".\DSP2802x_GlobalVariableDefs.pp" ".\DacDrvCnf.pp" ".\PFC2PHIL-DevInit_F2802x.pp" ".\PFC2PhiL-Main.pp" ".\PWM_PFC2PhiL_Cnf.pp" 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

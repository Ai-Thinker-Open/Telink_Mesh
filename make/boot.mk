################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /proj/mcu_spec

BOOT_FLAG := -DMCU_STARTUP_8258_RET_16K

OBJS += $(OUT_PATH)/proj/mcu_spec/cstartup_8258_RET_16K.o 
OBJS += $(OUT_PATH)/proj/mcu_spec/div_mod.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/proj/mcu_spec/%.o: $(TEL_PATH)/SDK1O/proj/mcu_spec/%.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(BOOT_FLAG) -c -o"$@" "$<"


################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /vendor/bqb /vendor/common

OBJS +=$(OUT_PATH)/vendor/bqb/phytest.o 
OBJS +=$(OUT_PATH)/vendor/bqb/rf_test.o 

OBJS +=$(OUT_PATH)/vendor/common/app_att_light.o 
OBJS +=$(OUT_PATH)/vendor/common/common.o 
OBJS +=$(OUT_PATH)/vendor/common/crc.o 
OBJS +=$(OUT_PATH)/vendor/common/cust_irq.o 
OBJS +=$(OUT_PATH)/vendor/common/device_led.o 
OBJS +=$(OUT_PATH)/vendor/common/device_power.o 
OBJS +=$(OUT_PATH)/vendor/common/dual_mode_adapt.o 
OBJS +=$(OUT_PATH)/vendor/common/emi.o 
OBJS +=$(OUT_PATH)/vendor/common/factory_reset.o 
OBJS +=$(OUT_PATH)/vendor/common/pm_test.o 
OBJS +=$(OUT_PATH)/vendor/common/rtc.o 
OBJS +=$(OUT_PATH)/vendor/common/scene.o 
OBJS +=$(OUT_PATH)/vendor/common/tl_audio.o 



# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/vendor/%.o: $(TEL_PATH)/SDK1O/vendor/%.c $(common_dir)
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


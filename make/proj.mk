################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OUT_DIR += /proj/app /proj/common /proj/drivers /proj/ext_drv /proj/mcu

OBJS += $(OUT_PATH)/proj/app/usbaud.o
OBJS += $(OUT_PATH)/proj/app/usbkb.o 
OBJS += $(OUT_PATH)/proj/app/usbmouse.o 

OBJS += $(OUT_PATH)/proj/common/breakpoint.o 
OBJS += $(OUT_PATH)/proj/common/compatibility.o 
OBJS += $(OUT_PATH)/proj/common/list.o 
OBJS += $(OUT_PATH)/proj/common/log.o 
OBJS += $(OUT_PATH)/proj/common/mempool.o 
OBJS += $(OUT_PATH)/proj/common/mmem.o 
OBJS += $(OUT_PATH)/proj/common/printf.o 
OBJS += $(OUT_PATH)/proj/common/u_printf.o 
OBJS += $(OUT_PATH)/proj/common/qsort.o 
OBJS += $(OUT_PATH)/proj/common/selection_sort.o 
OBJS += $(OUT_PATH)/proj/common/string.o 
OBJS += $(OUT_PATH)/proj/common/utility.o 

OBJS += $(OUT_PATH)/proj/drivers/adc_8258.o 
OBJS += $(OUT_PATH)/proj/drivers/adc_8263.o 
OBJS += $(OUT_PATH)/proj/drivers/adc_8266.o 
OBJS += $(OUT_PATH)/proj/drivers/adc_8267.o 
OBJS += $(OUT_PATH)/proj/drivers/adc_8269.o 
OBJS += $(OUT_PATH)/proj/drivers/airmouse.o 
OBJS += $(OUT_PATH)/proj/drivers/airmouse_cali.o 
OBJS += $(OUT_PATH)/proj/drivers/audio.o 
OBJS += $(OUT_PATH)/proj/drivers/battery.o 
OBJS += $(OUT_PATH)/proj/drivers/battery_8258.o 
OBJS += $(OUT_PATH)/proj/drivers/dfifo.o 
OBJS += $(OUT_PATH)/proj/drivers/eth_hw.o 
OBJS += $(OUT_PATH)/proj/drivers/ethernet.o 
OBJS += $(OUT_PATH)/proj/drivers/flash.o 
OBJS += $(OUT_PATH)/proj/drivers/i2c.o 
OBJS += $(OUT_PATH)/proj/drivers/id.o 
OBJS += $(OUT_PATH)/proj/drivers/ip_adapt.o 
OBJS += $(OUT_PATH)/proj/drivers/keyboard.o 
OBJS += $(OUT_PATH)/proj/drivers/mic.o 
OBJS += $(OUT_PATH)/proj/drivers/pwm.o 
OBJS += $(OUT_PATH)/proj/drivers/spi.o 
OBJS += $(OUT_PATH)/proj/drivers/syshw.o 
OBJS += $(OUT_PATH)/proj/drivers/uart_8258.o 
OBJS += $(OUT_PATH)/proj/drivers/usb.o 
OBJS += $(OUT_PATH)/proj/drivers/usbdesc.o 
OBJS += $(OUT_PATH)/proj/drivers/usbhw.o 


OBJS += $(OUT_PATH)/proj/mcu/analog.o 
OBJS += $(OUT_PATH)/proj/mcu/anareg.o 
OBJS += $(OUT_PATH)/proj/mcu/audhw.o 
OBJS += $(OUT_PATH)/proj/mcu/clock.o 
OBJS += $(OUT_PATH)/proj/mcu/cpu.o 
OBJS += $(OUT_PATH)/proj/mcu/otp.o 
OBJS += $(OUT_PATH)/proj/mcu/putchar.o 
OBJS += $(OUT_PATH)/proj/mcu/random.o 
OBJS += $(OUT_PATH)/proj/mcu/register.o 
OBJS += $(OUT_PATH)/proj/mcu/swire.o 

OBJS += $(OUT_PATH)/proj/mcu_spec/gpio_8258.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/proj/%.o: $(TEL_PATH)/SDK1O/proj/%.c 
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"

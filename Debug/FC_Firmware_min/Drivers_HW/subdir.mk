################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FC_Firmware_min/Drivers_HW/drv_motor.c \
../FC_Firmware_min/Drivers_HW/drv_rc.c \
../FC_Firmware_min/Drivers_HW/drv_spi.c 

C_DEPS += \
./FC_Firmware_min/Drivers_HW/drv_motor.d \
./FC_Firmware_min/Drivers_HW/drv_rc.d \
./FC_Firmware_min/Drivers_HW/drv_spi.d 

OBJS += \
./FC_Firmware_min/Drivers_HW/drv_motor.o \
./FC_Firmware_min/Drivers_HW/drv_rc.o \
./FC_Firmware_min/Drivers_HW/drv_spi.o 


# Each subdirectory must supply rules for building sources it contributes
FC_Firmware_min/Drivers_HW/%.o FC_Firmware_min/Drivers_HW/%.su FC_Firmware_min/Drivers_HW/%.cyclo: ../FC_Firmware_min/Drivers_HW/%.c FC_Firmware_min/Drivers_HW/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Flight_Math" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Sensors" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Drivers_HW" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Flight_Math" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Sensors" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/System_App" -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Drivers_HW" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Flight_Math" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/System_App" -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FC_Firmware_min-2f-Drivers_HW

clean-FC_Firmware_min-2f-Drivers_HW:
	-$(RM) ./FC_Firmware_min/Drivers_HW/drv_motor.cyclo ./FC_Firmware_min/Drivers_HW/drv_motor.d ./FC_Firmware_min/Drivers_HW/drv_motor.o ./FC_Firmware_min/Drivers_HW/drv_motor.su ./FC_Firmware_min/Drivers_HW/drv_rc.cyclo ./FC_Firmware_min/Drivers_HW/drv_rc.d ./FC_Firmware_min/Drivers_HW/drv_rc.o ./FC_Firmware_min/Drivers_HW/drv_rc.su ./FC_Firmware_min/Drivers_HW/drv_spi.cyclo ./FC_Firmware_min/Drivers_HW/drv_spi.d ./FC_Firmware_min/Drivers_HW/drv_spi.o ./FC_Firmware_min/Drivers_HW/drv_spi.su

.PHONY: clean-FC_Firmware_min-2f-Drivers_HW


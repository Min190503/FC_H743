################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FC_Firmware_min/Flight_Math/madgwick_filter.c 

C_DEPS += \
./FC_Firmware_min/Flight_Math/madgwick_filter.d 

OBJS += \
./FC_Firmware_min/Flight_Math/madgwick_filter.o 


# Each subdirectory must supply rules for building sources it contributes
FC_Firmware_min/Flight_Math/%.o FC_Firmware_min/Flight_Math/%.su FC_Firmware_min/Flight_Math/%.cyclo: ../FC_Firmware_min/Flight_Math/%.c FC_Firmware_min/Flight_Math/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Flight_Math" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Sensors" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Drivers_HW" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Flight_Math" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Sensors" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/System_App" -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Drivers_HW" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/Flight_Math" -I"/home/min/FC_mamab_4_H743_V2/FC_mamba/FC_Firmware_min/System_App" -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-FC_Firmware_min-2f-Flight_Math

clean-FC_Firmware_min-2f-Flight_Math:
	-$(RM) ./FC_Firmware_min/Flight_Math/madgwick_filter.cyclo ./FC_Firmware_min/Flight_Math/madgwick_filter.d ./FC_Firmware_min/Flight_Math/madgwick_filter.o ./FC_Firmware_min/Flight_Math/madgwick_filter.su

.PHONY: clean-FC_Firmware_min-2f-Flight_Math


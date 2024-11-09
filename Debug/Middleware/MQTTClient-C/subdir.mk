################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/MQTTClient-C/MQTTClient.c 

OBJS += \
./Middleware/MQTTClient-C/MQTTClient.o 

C_DEPS += \
./Middleware/MQTTClient-C/MQTTClient.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/MQTTClient-C/%.o Middleware/MQTTClient-C/%.su Middleware/MQTTClient-C/%.cyclo: ../Middleware/MQTTClient-C/%.c Middleware/MQTTClient-C/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -DUSE_WIFI -DMSG_ERROR -DUSE_TIMEDATE -DGENERICMQTT -DUSE_CLEAR_TIMEDATE -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/B-L475E-IOT01 -I../Drivers/BSP/Components/es_wifi -I../Drivers/BSP/Components/Common -I../WiFi -I../Common/Shared/Inc -I../Middleware/cJSON -I../Middleware/mbedTLS/Inc -I../Middleware/MQTTPacket -I../Middleware/MQTTClient-C -I../Common/GenericMQTT/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-MQTTClient-2d-C

clean-Middleware-2f-MQTTClient-2d-C:
	-$(RM) ./Middleware/MQTTClient-C/MQTTClient.cyclo ./Middleware/MQTTClient-C/MQTTClient.d ./Middleware/MQTTClient-C/MQTTClient.o ./Middleware/MQTTClient-C/MQTTClient.su

.PHONY: clean-Middleware-2f-MQTTClient-2d-C


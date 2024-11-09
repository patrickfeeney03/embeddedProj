################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/cJSON/cJSON.c \
../Middleware/cJSON/cJSON_Utils.c 

OBJS += \
./Middleware/cJSON/cJSON.o \
./Middleware/cJSON/cJSON_Utils.o 

C_DEPS += \
./Middleware/cJSON/cJSON.d \
./Middleware/cJSON/cJSON_Utils.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/cJSON/%.o Middleware/cJSON/%.su Middleware/cJSON/%.cyclo: ../Middleware/cJSON/%.c Middleware/cJSON/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -DUSE_WIFI -DMSG_ERROR -DUSE_TIMEDATE -DGENERICMQTT -DUSE_CLEAR_TIMEDATE -DSTM32_THREAD_SAFE_STRATEGY=4 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Drivers/BSP/B-L475E-IOT01 -I../Drivers/BSP/Components/es_wifi -I../Drivers/BSP/Components/Common -I../WiFi -I../Common/Shared/Inc -I../Middleware/cJSON -I../Middleware/mbedTLS/Inc -I../Middleware/MQTTPacket -I../Middleware/MQTTClient-C -I../Common/GenericMQTT/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Core/ThreadSafe -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-cJSON

clean-Middleware-2f-cJSON:
	-$(RM) ./Middleware/cJSON/cJSON.cyclo ./Middleware/cJSON/cJSON.d ./Middleware/cJSON/cJSON.o ./Middleware/cJSON/cJSON.su ./Middleware/cJSON/cJSON_Utils.cyclo ./Middleware/cJSON/cJSON_Utils.d ./Middleware/cJSON/cJSON_Utils.o ./Middleware/cJSON/cJSON_Utils.su

.PHONY: clean-Middleware-2f-cJSON

